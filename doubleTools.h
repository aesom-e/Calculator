#include <string.h>

#ifndef DOUBLETOOLS_H
#define DOUBLETOOLS_H

int scientificNotation = 0;

char* compactDouble(double in) {
    char* ret = malloc(256);
    int i;

    if(scientificNotation) {
        sprintf(ret, "[%.6e", in);

        // Calculate the exponent
        int len, x;
        int devNull[2];
        char devNull2;

        sscanf(ret, "[%d.%de%c%d", &(devNull[0]), &(devNull[1]), &devNull2, &x);
        x *= (*(strrchr(ret, 'e')+1) == '-') ? -1 : 1;

        // Remove the "e+x" part of the number
        *(strrchr(ret, 'e')) = 0;
        len = strlen(ret);

        // Remove trailing 0's from the decimal point
        for(i=len-1;i>=2;i--) {
            if(ret[i] == '.') { ret[i] = 0; break; }
            if(ret[i] == '0') ret[i] = 0; else break;
        }

        // Append "*10^x" to the string
        len = strlen(ret);
        sprintf(ret+len, "*10^%d]", x);

    } else {
        sprintf(ret,"%lf", in);

        for(i=strlen(ret)-1;i>=1;i--) {
            if(ret[i] == '.') { ret[i] = 0; break; }
            if(ret[i] == '0') ret[i] = 0; else break;
        }

        // Remove "-0"
        if(strcmp(ret, "-0") == 0) { ret[0] = '0'; ret[1] = 0;}
    }

    return ret;
}

#endif