#include <math.h>
#include "lexer.h"
#include "tokenArrayTools.h"
#include "customFunctions.h"
#include "graphWin.h"
#include "graphLinux.h"

#ifndef SOLVER_H
#define SOLVER_H

unsigned char noPrint = 0;

// Solves a single equation.
int solveEquation(tokenArray* array, int index) {

    // Verify correct number of parameters
    if(!((array->length >= index+2
    && array->data[index].type != FUNCTION)
    || (array->length >= index+1 &&
    (array->data[index].type == FUNCTION
    || (array->data[index+1].type == OPERATION
    && array->data[index+1].value == FACTORIAL))))) return 1;

    // This is a mess, but what it does it verify that what is passed in is in the correct format
    // NUMBER OPERATION NUMBER, FUNCTION NUMBER or NUMBER FACTORIAL
    if(!((array->data[index].type == NUMBER
    && array->data[index+1].type == OPERATION
    && array->data[index+2].type == NUMBER) ||
    (array->data[index].type == FUNCTION
    && array->data[index+1].type == NUMBER) ||
    (array->data[index+1].type == OPERATION
    && array->data[index+1].value == FACTORIAL))) return 2;

    double solution = 0;

    if(array->data[index].type == FUNCTION) {
        switch((int)array->data[index].value) {
            case SQRT:       solution = sqrt(array->data[index+1].value);   break;
            case SINE:       solution = sin(array->data[index+1].value);    break;
            case COSINE:     solution = cos(array->data[index+1].value);    break;
            case TANGENT:    solution = tan(array->data[index+1].value);    break;
            case LOGARITHM:
                // Allows the user to take the logarithm of an arbitrary base
                if(array->length >= index+2 && array->data[index+2].type == NUMBER) {
                    array->data[index].value = log(array->data[index+2].value) / log(array->data[index+1].value);
                    shiftLeft(array, index+1, 2);
                    array->data[index].type = NUMBER;
                    return 0;
                }
                else solution = log10(array->data[index+1].value);
                break;
            case NATURALLOG: solution = log(array->data[index+1].value);    break;
        }
    } else {
        switch((int)array->data[index+1].value) {
            case ADD:       solution = array->data[index].value + array->data[index+2].value;           break;
            case SUBTRACT:  solution = array->data[index].value - array->data[index+2].value;           break;
            case MULTIPLY:  solution = array->data[index].value * array->data[index+2].value;           break;
            case DIVIDE:
                if(array->data[index+2].value == 0) { solution = 0; break; } // Avoid a division by 0
                solution = array->data[index].value / array->data[index+2].value;
                break;
            case EXPONENT:  solution = pow(array->data[index].value, array->data[index+2].value);     break;
            case ROOT:      solution = pow(array->data[index+2].value, 1 / array->data[index].value); break;
            case FACTORIAL: solution = tgamma(array->data[index].value + 1); break;
        }
    }

    array->data[index].value = solution;
    if(array->data[index].type == FUNCTION
    || (array->data[index+1].type == OPERATION
    && array->data[index+1].value == FACTORIAL)) shiftLeft(array, index+1, 1);
    else shiftLeft(array, index+1, 2);
    array->data[index].type = NUMBER;
    return 0;
}

// Order of operations: Brackets, functionList, exponents, multiply/divide, add/subtract
double solve(tokenArray* array) {
    noPrint = 0;

    // Check for function declaration
    // OTHER ( OTHER ) = MATH
    int i, step = 0, newFuncId, lastFuncId = functionList->len;
    for(i=0;i<array->length;i++) {
        switch(step) {
            case 0:
                if(array->data[i].type == OTHER) step++;
                else i = MAXLENGTH + 1; // Essentially break out of the for loop
                continue;
            case 1:
                if(array->data[i].type == BRACKET
                && array->data[i].bracketDepth > array->data[i-1].bracketDepth) step++;
                continue;
            case 2:
                if(array->data[i].type == OTHER) step++;
                else i = MAXLENGTH + 1;
                continue;
            case 3:
                if(array->data[i].type == BRACKET
                   && array->data[i].bracketDepth < array->data[i-1].bracketDepth) step++;
                continue;
            case 4:
                if(array->data[i].type == OPERATION
                && array->data[i].value == EQUALS) step++;
                else i = MAXLENGTH + 1;
                continue;
            case 5:
                newFuncId = addFunction(array);
                function newFunction = functionList->functions[newFuncId];
                printFunction(newFunction);
                newFuncId >= lastFuncId ? printf("defined\n") : printf("redefined\n");
                noPrint = 1;
                return 1;
        }
    }

    // Handle graphing functions
    if(array->data[0].type == FUNCTION
       && array->data[0].value == GRAPH
       && array->data[1].type == OTHER) {
        i = 1;
        char name[32] = {0}, nameLen = 0;
        while(array->data[i].type == OTHER) name[nameLen++] = (char)array->data[i++].value;
        for(i=0;i<functionList->len;i++)
            if(strncmp(functionList->functions[i].name, name, functionList->functions[i].nameLen) == 0) {
                noPrint = 1;

                if(functionList->functions[i].argumentsLen != 1) {
                    printf("Error: ");
                    printFunction(functionList->functions[i]);
                    printf("contains more than one parameter\n");
                    return 1;
                }

                printf("Graphing ");
                printFunction(functionList->functions[i]);
                printf("\n");

                double data[graphWidth];

                double lastAns = ans;
                tokenArray* math = malloc(sizeof(tokenArray));
                double* argument = malloc(sizeof(double));
                function functionToRun = functionList->functions[i];
                int index = 0;

                for(i=-(graphWidth/2);i<graphWidth/2;i++) {
                    *math = tokenArrayDup(&functionToRun.math, 0);
                    *argument = (double)i;
                    runFunction(math, functionToRun, argument);
                    data[index++] = solve(math);
                }
                ans = lastAns;

                free(math);
                free(argument);

                graph(functionToRun, data);

                noPrint = 1;
                return 1;
            }
    }

    if(!checkForFunctionCall(array)) return 0;

    // Handle cases where the user wants to add to the previous answer
    if(array->data[0].type == OPERATION) {
        shiftRight(array, 0, 1);
        array->data[0].type = NUMBER;
        array->data[0].value = ans;
    }

    if(!validateArray(array)) return 0;

    int index, found, foundOnce;
    short bracketDepth;

    // Loop from the max bracketDepth to 0 decrementing when no more math can possibly be done
    for(bracketDepth=(short)getMaxBracketDepth(array);bracketDepth>=0;bracketDepth--) {
        foundOnce = 0;
        // Handle cases like (-2) turning it into -2
        while(array->length > 1) {
            found = 0;
            for(index=array->length-4;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index].type == BRACKET
                   && array->data[index].bracketDepth == array->data[index+1].bracketDepth // index is opening bracket
                   && array->data[index+1].type == OPERATION && array->data[index+1].value == SUBTRACT
                   && array->data[index+2].type == NUMBER
                   && array->data[index+3].type == BRACKET) {
                    foundOnce = 1;
                    found = 1;
                    array->data[index+2].value *= -1;
                    array->data[index] = array->data[index+2];
                    array->data[index].bracketDepth--;
                    shiftLeft(array, index+1, 3);
                    break;
                }
            }
            if(!found) break;
        }

        // Handle cases like (2) turning it into 2
        while(array->length > 1) {
            found = 0;
            for(index=array->length-3;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index].type == BRACKET
                   && array->data[index].bracketDepth == array->data[index+1].bracketDepth // array->data[index] is opening bracket
                   && array->data[index+1].type == NUMBER
                   && array->data[index+2].type == BRACKET) {
                    foundOnce = 1;
                    found = 1;
                    array->data[index] = array->data[index+1];
                    array->data[index].bracketDepth--;
                    shiftLeft(array, index+1, 2);
                    break;
                }
            }
            if(!found) break;
        }

        // Handle cases like sine -pi turning it into sine -3.1415926535...
        while(array->length > 1) {
            found = 0;
            for(index=array->length-3;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index].type != NUMBER
                   && array->data[index+1].type == OPERATION && array->data[index+1].value == SUBTRACT
                   && array->data[index+2].type == NUMBER) {
                    foundOnce = 1;
                    found = 1;
                    array->data[index+2].value *= -1;
                    shiftLeft(array, index+1, 1);
                }
            }
            if(!found) break;
        }

        // Handle functionList
        while(array->length > 1) {
            found = 0;
            for(index=array->length-2;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index].type == FUNCTION) {
                    foundOnce = 1;
                    found = 1;
                    solveEquation(array, index);
                    break;
                }
            }
            if(!found) break;
        }

        // Handle root
        while(array->length > 1) {
            found = 0;
            for(index=array->length-3;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index+1].type == OPERATION && array->data[index+1].value == ROOT) {
                    foundOnce = 1;
                    found = 1;
                    solveEquation(array, index);
                    break;
                }
            }
            if(!found) break;
        }

        // Handle factorials
        while(array->length > 1) {
            found = 0;
            for(index=array->length-2;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index+1].type == OPERATION && array->data[index+1].value == FACTORIAL) {
                    foundOnce = 1;
                    found = 1;
                    solveEquation(array, index);
                    break;
                }
            }
            if(!found) break;
        }

        // Handle ^
        while(array->length > 1) {
            found = 0;
            for(index=array->length-3;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index+1].type == OPERATION && array->data[index+1].value == EXPONENT) {
                    foundOnce = 1;
                    found = 1;
                    solveEquation(array, index);
                    break;
                }
            }
            if(!found) break;
        }

        // Handle cases like 2pi turning it into 6.28318...
        while(array->length > 1) {
            found = 0;
            for(index=array->length-2;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index].type == NUMBER
                   && array->data[index+1].type == NUMBER) {
                    foundOnce = 1;
                    found = 1;
                    array->data[index].value *= array->data[index+1].value;
                    shiftLeft(array, index+1, 1);
                }
            }
            if(!found) break;
        }

        // Handle /, *
        while(array->length > 1) {
            found = 0;
            for(index=array->length-3;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index+1].type == OPERATION
                   &&(array->data[index+1].value == MULTIPLY || array->data[index+1].value == DIVIDE)) {
                    foundOnce = 1;
                    found = 1;
                    solveEquation(array, index);
                    break;
                }
            }
            if(!found) break;
        }

        // Handle + and -
        while(array->length > 1) {
            found = 0;
            for(index=array->length-3;index>=0;index--) {
                if(array->data[index].bracketDepth != bracketDepth) continue;
                if(array->data[index+1].type == OPERATION
                   &&(array->data[index+1].value == ADD || array->data[index+1].value == SUBTRACT)) {
                    foundOnce = 1;
                    found = 1;
                    solveEquation(array, index);
                    break;
                }
            }
            if(!found) break;
        }

        // Keep bracketDepth the same, it will be decremented by the for loop
        if(foundOnce) bracketDepth++;
    }

    ans = array->data[0].value;
    return array->data[0].value;
}

#endif