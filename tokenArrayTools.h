#include <string.h>
#include "lexer.h"
#include "customFunctions.h"

#ifndef TOKENARRAYTOOLS_H
#define TOKENARRAYTOOLS_H

void ucharMax(unsigned char* a, unsigned char b) { *a = *a>b ? *a : b; }

int validateArray(tokenArray* array) {
    inputError = 1;

    // Check that the brackets are balanced
    if(array->data[array->length-1].bracketDepth != 0) {
        strcpy(errorMessage, "Expression contains an imbalance of opening and closing brackets");
        return 0;
    }

    // Validate that it starts with a number, bracket, or function, and ends with a number, bracket, or factorial
    if(array->data[0].type != NUMBER && array->data[0].type != BRACKET && array->data[0].type != FUNCTION) {
        strcpy(errorMessage, "Expression doesn't start with a number, bracket, or function");
        return 0;
    }
    if(array->data[array->length-1].type != NUMBER && array->data[array->length-1].type != BRACKET &&
     !(array->data[array->length-1].type == OPERATION && array->data[array->length-1].value == FACTORIAL)) {
        strcpy(errorMessage, "Expression doesn't end with a number, bracket, or factorial");
        return 0;
    }

    // Check for proper token order
    int i;
    for(i=0;i<array->length-1;i++) {
        if(array->data[i].type == OPERATION) {
            if(array->data[i].value == DIVIDE
               && array->data[i+1].value == 0
               && array->data[i+1].type == NUMBER) {
                strcpy(errorMessage, "Expression includes a division by 0");
                return 0;
            }
            if(array->data[i+1].type != NUMBER && array->data[i+1].type != BRACKET && array->data[i+1].type != FUNCTION
            && array->data[i+1].type != OTHER) {
                strcpy(errorMessage, "Expression includes an invalid operation");
                return 0;
            }
        }
    }

    // No equals sins allowed (this is after function declaration is checked for)
    for(i=0;i<array->length;i++) {
        if(array->data[i].type == OPERATION
        && array->data[i].value == EQUALS) {
            strcpy(errorMessage, "Expression includes an equals sign");
            return 0;
        }
        if(array->data[i].type == FUNCTION
        && array->data[i].value == GRAPH) {
            strcpy(errorMessage, "Graph called improperly");
            return 0;
        }
    }

    inputError = 0;
    return 1;
}

unsigned char getMaxBracketDepth(const tokenArray* array) {
    unsigned char maxBracketDepth = 0;
    int i;
    for(i=0;i<array->length;i++) ucharMax(&maxBracketDepth, array->data[i].bracketDepth);
    return maxBracketDepth;
}

#endif