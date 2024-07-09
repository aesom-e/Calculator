#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "base.h"
#include "help.h"

#ifndef LEXER_H
#define LEXER_H

double ans = 0;
unsigned char blankRet = 0;

tokenArray lex(char* line) {
    blankRet = 0;

    tokenArray ret = {0};
    int len = (int)strlen(line), i, bracketIndex = 0;

    // Turn all characters to their lowercase version for parsing
    for(i=0;i<len;i++) line[i] = tolower(line[i]);

    if(strcmp(line, "exit\n") == 0) exit(0);
    if(strcmp(line, "help\n") == 0) { printHelp(); blankRet = 1; return ret; }
    if(strcmp(line, "printfull\n") == 0) { printFull = !printFull; printf("= %u\n", printFull);
                                           blankRet = 1; return ret; }

    for(i=0;i<len;i++) {
        if(isspace(line[i]) || line[i] == ',') continue;

        if(line[i] == '(' || line[i] == ')') {
            bracketIndex += (line[i] == '(')?1:-1;
            ret.data[ret.length].type = BRACKET;
        } else if(isdigit(line[i]) || line[i] == '.') {
            char number[32] = {0};
            number[0] = line[i++];
            char numberIndex = 1;

            while(isdigit(line[i]) || line[i] == '.') number[numberIndex++] = line[i++];
            i--; // Step back as the for loop will increment i

            ret.data[ret.length].type = NUMBER;
            ret.data[ret.length].value = atof(number);
        } else if(line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == '/' || line[i] == '^' ||
                  line[i] == '=' || line[i] == '!') {
            ret.data[ret.length].type = OPERATION;

            switch(line[i]) {
                case '+':
                    ret.data[ret.length].value = ADD;
                    break;
                case '-':
                    ret.data[ret.length].value = SUBTRACT;
                    break;
                case '*':
                    ret.data[ret.length].value = MULTIPLY;
                    break;
                case '/':
                    ret.data[ret.length].value = DIVIDE;
                    break;
                case '^':
                    ret.data[ret.length].value = EXPONENT;
                    break;
                case '=':
                    ret.data[ret.length].value = EQUALS;
                    break;
                case '!':
                    ret.data[ret.length].value = FACTORIAL;
                    break;
            }
        } else if(strncmp(&line[i], "pi", 2) == 0) {
            i++;
            ret.data[ret.length].type = NUMBER;
            ret.data[ret.length].value = M_PI;
        } else if(strncmp(&line[i], "euler", 5) == 0) {
            i += 4;
            ret.data[ret.length].type = NUMBER;
            ret.data[ret.length].value = M_E;
        } else if(strncmp(&line[i], "ans", 3) == 0) {
            i += 2;
            ret.data[ret.length].type = NUMBER;
            ret.data[ret.length].value = ans;
        } else if(strncmp(&line[i], "sqrt", 4) == 0) {
            i += 3;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = SQRT;
        } else if(strncmp(&line[i], "arcsin", 6) == 0 || strncmp(&line[i], "arcsine", 7) == 0) {
            if(strncmp(&line[i], "arcsine", 7) == 0) i++;
            i += 5;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = ARCSINE;
        } else if(strncmp(&line[i], "arccos", 6) == 0 || strncmp(&line[i], "arccosine", 9) == 0) {
            if(strncmp(&line[i], "arccosine", 9) == 0) i += 3;
            i += 5;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = ARCCOSINE;
        } else if(strncmp(&line[i], "arctan", 6) == 0 || strncmp(&line[i], "arctangent", 10) == 0) {
            if(strncmp(&line[i], "arctangent", 10) == 0) i += 4;
            i += 5;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = ARCTANGENT;
        } else if(strncmp(&line[i], "sin", 3) == 0 || strncmp(&line[i], "sine", 4) == 0) {
            if(strncmp(&line[i], "sine", 4) == 0) i++;
            i += 2;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = SINE;
        } else if(strncmp(&line[i], "cos", 3) == 0 || strncmp(&line[i], "cosine", 6) == 0) {
            if(strncmp(&line[i], "cosine", 6) == 0) i += 3;
            i += 2;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = COSINE;
        } else if(strncmp(&line[i], "tan", 3) == 0 || strncmp(&line[i], "tangent", 7) == 0) {
            if(strncmp(&line[i], "tangent", 7) == 0) i += 4;
            i += 2;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = TANGENT;
        } else if(strncmp(&line[i], "root", 4) == 0) {
            i += 3;
            ret.data[ret.length].type = OPERATION;
            ret.data[ret.length].value = ROOT;
        } else if(strncmp(&line[i], "log", 3) == 0 || strncmp(&line[i], "logarithm", 9) == 0) {
            if(strncmp(&line[i], "logarithm", 9) == 0) i += 6;
            i += 2;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = LOGARITHM;
        } else if(strncmp(&line[i], "ln", 2) == 0) {
            i++;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = NATURALLOG;
        } else if(strncmp(&line[i], "graph", 5) == 0) {
            i += 4;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = GRAPH;
        } else if(strncmp(&line[i], "describe", 8) == 0) {
            i += 7;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = DESCRIBE;
        } else if(strncmp(&line[i], "degtorad", 8) == 0) {
            i += 7;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = DEGTORAD;
        } else if(strncmp(&line[i], "radtodeg", 8) == 0) {
            i += 7;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = RADTODEG;
        } else if(strncmp(&line[i], "abs", 3) == 0 || strncmp(&line[i], "absolute", 8) == 0) {
            if(strncmp(&line[i], "absolute", 8) == 0) i += 5;
            i += 2;
            ret.data[ret.length].type = FUNCTION;
            ret.data[ret.length].value = ABSOLUTEFN;
        } else {
            ret.data[ret.length].type = OTHER;
            ret.data[ret.length].value = line[i];
        }
        ret.data[ret.length].bracketDepth = bracketIndex;
        ret.length++;
    }

    free(line);
    return ret;
}

#endif