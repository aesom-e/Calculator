#include <math.h>
#include "lexer.h"
#include "tokenArrayTools.h"
#include "base.h"

#ifndef GRAPHSOLVER_H
#define GRAPHSOLVER_H

unsigned char error = 0;

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
            case SQRT:       solution = sqrt(array->data[index+1].value);     break;
            case SINE:       solution = sin(array->data[index+1].value);      break;
            case COSINE:     solution = cos(array->data[index+1].value);      break;
            case TANGENT:    solution = tan(array->data[index+1].value);      break;
            case ARCSINE:    solution = asin(array->data[index+1].value);     break;
            case ARCCOSINE:  solution = acos(array->data[index+1].value);     break;
            case ARCTANGENT: solution = atan(array->data[index+1].value);     break;
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
            case NATURALLOG:   solution = log(array->data[index+1].value);           break;
            case RADTODEG:     solution = array->data[index+1].value * (180 / M_PI);    break;
            case DEGTORAD:     solution = array->data[index+1].value * (M_PI / 180);    break;
            case ABSOLUTEFN:   solution = fabs(array->data[index+1].value);          break;
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
            case FACTORIAL: solution = tgamma(array->data[index].value + 1);                                break;
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

// Like the solve function in solver.h, but doesn't include calls to graphing headers nor printing of solutions
double graphSolve(tokenArray* array) {
    error = 0;
    noPrintFull = 1;

    // Check for function calls, if an error occurs, return
    if(!checkForFunctionCall(array)) { noPrintFull = 0; error = 1; return 0; }
    noPrintFull = 0;

    if(!validateArray(array)) {
        error = 1; return 0; }

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

    return array->data[0].value;
}

#endif