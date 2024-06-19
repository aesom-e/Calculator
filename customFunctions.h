#include <stdlib.h>
#include <string.h>

#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

// These need to be here to avoid an include loop as this file needs access to tokenArray
#define MAXLENGTH 4096
#define MAXFUNCTIONS 64

unsigned char inputError = 0;
char errorMessage[256] = {0};

typedef enum {
    NONE,
    BRACKET,
    NUMBER,
    OPERATION,
    FUNCTION,
    OTHER
} tokenType;

typedef enum {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    EXPONENT,
    ROOT,
    EQUALS,
    FACTORIAL
} operationType;

typedef enum {
    SQRT,
    SINE,
    COSINE,
    TANGENT,
    LOGARITHM,
    NATURALLOG,
    GRAPH
} functionType;

typedef struct {
    double value;
    unsigned char bracketDepth;
    tokenType type;
} token;

typedef struct {
    token data[MAXLENGTH];
    int length;
} tokenArray;


typedef struct {
    char name[32];
    char nameLen;
    char arguments[16];
    char argumentsLen;
    tokenArray math;
} function;

typedef struct {
    function* functions;
    int len;
}* functionArray;

functionArray functionList = NULL;

void createFunctionArray() {
    if(functionList != NULL) return;
    functionList = malloc(sizeof(functionArray));
    functionList->functions = malloc(sizeof(function) * MAXFUNCTIONS);
    functionList->len = 0;
}

tokenArray tokenArrayDup(tokenArray* array, int start) {
    tokenArray ret = {0};
    ret.length = array->length - start;

    int i;
    for(i=start;i<array->length;i++) ret.data[i-start] = array->data[i];

    return ret;
}

void shiftLeft(tokenArray* array, int index, int amount) {
    if (amount <= 0 || index < 0 || index + amount > array->length) return;

    int i;
    for(i=index;i<array->length-amount;i++) array->data[i] = array->data[i+amount];

    array->length -= amount;
}

void shiftRight(tokenArray* array, int index, int amount) {
    if(amount <= 0 || index < 0 || array->length + amount >= MAXLENGTH) return;

    int i;
    for(i=array->length-1;i>=index;i--) array->data[i + amount] = array->data[i];

    array->length += amount;
}

int addFunction(tokenArray* array) {
    function newFunction = {0};

    int i = 0;
    char name[32] = {0}, nameLen = 0, arguments[16] = {0}, argumentsLen = 0;
    while(array->data[i].type == OTHER) name[nameLen++] = (char)array->data[i++].value;
    i++;
    while(array->data[i].type == OTHER) arguments[argumentsLen++] = (char)array->data[i++].value;

    strcpy(newFunction.name, name);
    newFunction.nameLen = nameLen;
    strcpy(newFunction.arguments, arguments);
    newFunction.argumentsLen = argumentsLen;
    newFunction.math = tokenArrayDup(array, i + 2);

    // Check for a redefinition
    for(i=0;i<functionList->len;i++) {
        if(strcmp(functionList->functions[i].name, name) == 0) {
            functionList->functions[i] = newFunction;
            return i;
        }
    }

    functionList->functions[functionList->len++] = newFunction;
    return functionList->len-1;
}

double getArgumentValue(function functionIn, double* arguments, char argumentName) {
    int i;
    for(i=0;i<functionIn.argumentsLen;i++) {
        if(argumentName == functionIn.arguments[i]) return arguments[i];
    }
    return 0;
}

void runFunction(tokenArray* array, function functionIn, double* arguments) {
    *array = functionIn.math;

    // Set the arguments values correctly within array
    int i;
    for(i=0;i<array->length;i++) {
        if(array->data[i].type == OTHER) {
            // Check that the character is a valid argument
            if(!strchr(functionIn.arguments, (char)array->data[i].value)) continue;
            array->data[i].type = NUMBER;
            array->data[i].value = getArgumentValue(functionIn, arguments, (char)array->data[i].value);
        }
    }

    // Expand functions to their full definition
    int step = 0, j, functionStart;
    char name[32] = {0}, nameLen = 0, funcArgsLen = 0;
    double funcArgs[16];

    for(i=0;i<array->length;i++) {
        switch(step) {
            case 0:
                if(array->data[i].type == OTHER) {
                    step++;
                    // Get function name
                    while(i < array->length && array->data[i].type == OTHER)
                        name[nameLen++] = (char)array->data[i++].value;

                    functionStart = --i;
                }
                else step = 0;
                continue;
            case 1:
                if(array->data[i].type == BRACKET
                && array->data[i].bracketDepth > array->data[i-1].bracketDepth) step++;
                else step = 0;
                continue;
            case 2:
                if(array->data[i].type == NUMBER) step++;
                else step = 0;
                funcArgs[funcArgsLen++] = array->data[i].value;
                continue;
            case 3:
                if(array->data[i].type == BRACKET
                && array->data[i].bracketDepth < array->data[i-1].bracketDepth) step++;
                else if(array->data[i].type != NUMBER) step = 0;
                else funcArgs[funcArgsLen++] = array->data[i].value;
                continue;
            case 4:
                for(j=0;j<functionList->len;j++) {
                    if(strncmp(functionList->functions[j].name, name, nameLen) == 0) {
                        // Function found

                        tokenArray tempArray = {0};
                        runFunction(&tempArray, functionList->functions[j], arguments);

                        // Insert tempArray into array
                        shiftLeft(array, functionStart, i-functionStart-2);

                        // Add opening and closing brackets to the return of the function
                        array->data[functionStart] = (token){
                                0,
                                (functionStart == 0) ? 1 : array->data[functionStart-1].bracketDepth+1,
                                BRACKET
                        };

                        array->data[functionStart+1] = (token){
                                0,
                                array->data[functionStart].bracketDepth-1,
                                BRACKET
                        };

                        shiftRight(array, functionStart+1, tempArray.length);
                        // j is unused at this point, so we can reuse it to save memory
                        for(j=0;j<tempArray.length;j++) {
                            array->data[functionStart+1+j] = tempArray.data[j];
                            // Set the bracket depth correctly
                            array->data[functionStart+1+j].bracketDepth += array->data[functionStart].bracketDepth;
                        }

                        step = 0;
                        break;
                    }
                }
                continue;
        }
    }
    // Never nesters are crying at this code
}

// Returns 1 for no error, 0 for an error
int checkForFunctionCall(tokenArray* array) {
    int i = 0, j, functionStart;

    while(i < array->length) {
        if(array->data[i].type == OTHER) {
            functionStart = i;

            char name[32] = {0}, nameLen = 0, argumentsLen = 0;
            double arguments[16];

            // Get function name
            while(i < array->length && array->data[i].type == OTHER)
                name[nameLen++] = (char)array->data[i++].value;
            name[nameLen] = 0;

            // Check for opening parenthesis
            if(i < array->length && array->data[i].type == BRACKET
            && array->data[i].bracketDepth > array->data[i-1].bracketDepth) {
                i++;
            } else {
                inputError = 1;
                strcpy(errorMessage, "Missing opening parenthesis in function call");
                return 0;
            }

            // Get function arguments
            while(i < array->length && array->data[i].type != BRACKET) {
                if(array->data[i].type == NUMBER) arguments[argumentsLen++] = array->data[i++].value;
                else {
                    inputError = 1;
                    strcpy(errorMessage, "Non-number argument in function call");
                    return 0;
                }
            }

            // Check for closing parenthesis
            if (i < array->length && array->data[i].type == BRACKET
            && array->data[i].bracketDepth < array->data[i-1].bracketDepth) i++;
            else {
                inputError = 1;
                strcpy(errorMessage, "Missing closing parenthesis in function call");
                return 0;
            }

            int found = 0;
            for(j=0;j<functionList->len;j++) {
                if(strncmp(functionList->functions[j].name, name, nameLen) == 0) {
                    if(functionList->functions[j].argumentsLen != argumentsLen) {
                        inputError = 1;
                        strcpy(errorMessage, "Incorrect number of arguments");
                        return 0;
                    }
                    found = 1;

                    tokenArray tempArray = {0};
                    runFunction(&tempArray, functionList->functions[j], arguments);

                    // Insert the functions contents back into the tokenArray
                    shiftLeft(array, functionStart, i-functionStart-2);

                    // Add opening and closing brackets to the return of the function
                    array->data[functionStart] = (token){
                            0,
                            (functionStart == 0) ? 1 : array->data[functionStart-1].bracketDepth+1,
                            BRACKET
                    };

                    array->data[functionStart+1] = (token){
                            0,
                            array->data[functionStart].bracketDepth-1,
                            BRACKET
                    };

                    shiftRight(array, functionStart, tempArray.length);

                    // j is unused at this point, so we can reuse it to save memory
                    for(j=0;j<tempArray.length;j++) {
                        array->data[functionStart+1+j] = tempArray.data[j];
                        // Set the bracket depth correctly
                        array->data[functionStart+1+j].bracketDepth += array->data[functionStart].bracketDepth;
                    }
                    i = functionStart;
                    break;
                }
            }

            if(!found) {
                inputError = 1;
                strcpy(errorMessage, "Function not found");
                return 0;
            }
        } else i++;
    }
    return 1;
}

void printFunction(function functionIn) {
    int i;
    printf("%s(", functionIn.name);
    for(i=0;i<functionIn.argumentsLen;i++) {
        if(i!=0) printf(", ");
        printf("%c", functionIn.arguments[i]);
    }
    printf(") ");
}

#endif
