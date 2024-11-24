#include <stdlib.h>
#include <string.h>
#include "doubleTools.h"
#include "constants.h"

#ifndef BASE_H
#define BASE_H

unsigned char inputError = 0;
char errorMessage[256] = {0};
unsigned char printFull = 0;
unsigned char noPrintFull = 0;

typedef enum {
    NONE,
    BRACKET,
    NUMBER,
    OPERATION,
    FUNCTION,
    SEPARATOR,
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
    ARCSINE,
    ARCCOSINE,
    ARCTANGENT,
    LOGARITHM,
    NATURALLOG,
    GRAPH,
    DESCRIBE,
    RADTODEG,
    DEGTORAD,
    ABSOLUTEFN,
    ROUND,
    SGN
} functionType;

typedef struct {
    double value;
    unsigned char bracketDepth;
    tokenType type;
} token;

typedef struct {
    token data[MAX_INPUT_LENGTH];
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
    if(functionList) return;
    functionList = malloc(sizeof(functionArray));
    functionList->functions = malloc(sizeof(function) * MAX_FUNCTIONS);
    functionList->len = 0;
}

void destroyFunctionArray() {
    if(!functionList) return;
    free(functionList->functions);
    free(functionList);
}

void quit(int exitCode) {
    destroyFunctionArray();
    exit(exitCode);
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
    if(amount <= 0 || index < 0 || array->length + amount >= MAX_INPUT_LENGTH) return;

    int i;
    for(i=array->length-1;i>=index;i--) array->data[i + amount] = array->data[i];

    array->length += amount;
}

void printToken(tokenArray in, int index) {
    switch(in.data[index].type) {
        case NONE: break;
        case BRACKET:
            // The line below will be 1 if the bracket being checked is open, 0 if not
            // The line with the two printf statements checks against that and if the first line is 1 prints "(" else ")"
            ((index == 0) ? (in.data[index].bracketDepth > 0) : (in.data[index].bracketDepth > in.data[index-1].bracketDepth))
            ? printf("(") : printf(")");
            break;
        case NUMBER:
            // Check whether to wrap the number in brackets
            if(index != 0) if(in.data[index-1].type == NUMBER) printf("(");

            char* numStr = compactDouble(in.data[index].value);

            (in.data[index].value < 0) ? printf("(") : 0;
            printf("%s", numStr);
            (in.data[index].value < 0) ? printf(")") : 0;

            free(numStr);

            // Check whether to wrap the number in brackets
            if(index != 0) { if(in.data[index-1].type == NUMBER) printf(")");
                else if(index + 1 < in.length && in.data[index+1].type == NUMBER) printf(" "); }

            break;
        case OPERATION:
            // Check specifically for the case where we may be looking at the '-' in (-4)
            /* To break this down, as it is very convoluted and hard to read at first, it first checks if index > 1
               If so, skip everything else. If not, check if there was an opening bracket immediately before the token,
               the token is a subtract token, and there is either a number or variable immediately after.
               If that check passes, we then print '-' and break by sending a 1 to the if making the statement if(1)
               breaking out of the switch statement. If not, we don't print anything and send a 0 to the if,
               so we continue execution.

               This can be more easily visualized by breaking it down into the much less visually appealing,
               but easier to understand statement below:
               if(index > 1) {
                   if((in.data[index-1].type == BRACKET && in.data[index-1].bracketDepth == in.data[index].bracketDepth)
                     && in.data[index].value == SUBTRACT
                     && (in.data[index+1].type == NUMBER || in.data[index+1].type == OTHER)) {
                       printf("-");
                       break;
                   }
               } */
            if(((index > 1) ? ((in.data[index-1].type == BRACKET && in.data[index-1].bracketDepth > in.data[index-2].bracketDepth)
                              && in.data[index].value == SUBTRACT && (in.data[index+1].type == NUMBER || in.data[index+1].type == OTHER))
                              : 0)
            ? printf("-"), 1 : 0) break;

            // Check whether to print a space before the operation
            ((index != 0) ? ((in.data[index-1].type == NUMBER || in.data[index-1].type == OTHER || in.data[index-1].type == BRACKET)
                             && in.data[index].value != EXPONENT
                             && in.data[index].value != ROOT
                             && in.data[index].value != FACTORIAL
                             && in.data[index].value != EQUALS) : 0)
            ? printf(" ") : 0;

            switch((int)in.data[index].value) {
                case ADD:       printf("+ ");     break;
                case SUBTRACT:  printf("- ");     break;
                case MULTIPLY:  printf("* ");     break;
                case DIVIDE:    printf("/ ");     break;
                case EXPONENT:  printf("^");      break;
                case ROOT:      printf("root ");  break;
                case EQUALS:    printf(" = ");    break;
                case FACTORIAL: printf("!");     break;
            }
            break;
        case FUNCTION:
            switch((int)in.data[index].value) {
                case SQRT:       printf("sqrt ");       break;
                case SINE:       printf("sine ");       break;
                case COSINE:     printf("cosine ");     break;
                case TANGENT:    printf("tangent ");    break;
                case ARCSINE:    printf("arcsine ");    break;
                case ARCCOSINE:  printf("arccosine ");  break;
                case ARCTANGENT: printf("arctangent "); break;
                case LOGARITHM:
                    // Check if the logarithm has a base, if so print logarithm[base] [number], else print logarithm [number]
                    ((index+2 < in.length) && in.data[index+1].type == NUMBER && in.data[index+2].type == NUMBER)
                    ? printf("log") : printf("log ");
                    break;
                case NATURALLOG: printf("ln ");         break;
                case GRAPH:      printf("graph ");      break;
                case DESCRIBE:   printf("describe ");   break;
                case RADTODEG:   printf("radToDeg ");   break;
                case DEGTORAD:   printf("degToRad ");   break;
                case ABSOLUTEFN: printf("abs ");        break;
                case ROUND:      printf("round ");      break;
                case SGN:        printf("sgn ");        break;
            }
            break;
        case OTHER: printf("%c", (char)in.data[index].value); break;
    }
}

void printArray(tokenArray array) {
    int i;
    for(i=0;i<array.length;i++) {
        printToken(array, i);
    }
    printf("\n");
}

void printFullExpression(tokenArray* array) {
    int foundNumber = 0, i;
    if(array->length != 1 && printFull && !noPrintFull) {
        // Now check that it's not just brackets and a single number
        for(i=0;i<array->length;i++) {
            if(array->data[i].type == BRACKET) continue;
            if(array->data[i].type == NUMBER) { if(foundNumber) { foundNumber = 2; break; } else foundNumber = 1; continue; }
            foundNumber = 2;
            break;
        }
        if(foundNumber != 2) return;

        printf("= "); printArray(*array);
    }
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

    if(functionList->len == MAX_FUNCTIONS-1) return -1;
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

void setupFunction(tokenArray* array, function functionIn, double* arguments) {
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
                        setupFunction(&tempArray, functionList->functions[j], arguments);

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
            while(i < array->length && array->data[i].type == OTHER && nameLen < 31)
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
                // Handle negative arguments
                else if(array->data[i].type == OPERATION && array->data[i].value == SUBTRACT
                     && array->data[i+1].type == NUMBER) { arguments[argumentsLen++] = -array->data[i+1].value; i+=2; }
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
                    setupFunction(&tempArray, functionList->functions[j], arguments);

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

                    printFullExpression(array);

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