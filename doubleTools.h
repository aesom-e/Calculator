#include <string.h>

#ifndef DOUBLETOOLS_H
#define DOUBLETOOLS_H

int scientificNotation = 0;

char* compactDouble(double in) {
    char* raw = malloc(512);
    int i;

    if(isinf(in)) {
        if(signbit(in)) {
            if(scientificNotation) sprintf(raw, "[-inf]");
            else                   sprintf(raw, "-inf");
        } else {
            if(scientificNotation) sprintf(raw, "[inf]");
            else                   sprintf(raw, "inf");
        }
    } else if(isnan(in)) {
        if(scientificNotation) sprintf(raw, "[NaN]");
        else                   sprintf(raw, "NaN");
    } else if(scientificNotation) {
        sprintf(raw, "[%.6e", in);

        // Calculate the exponent
        int len, x;
        int devNull[2];
        char devNull2;

        sscanf(raw, "[%d.%de%c%d", &(devNull[0]), &(devNull[1]), &devNull2, &x);
        x *= (*(strrchr(raw, 'e')+1) == '-') ? -1 : 1;

        // Remove the "e+x" part of the number
        *(strrchr(raw, 'e')) = 0;
        len = strlen(raw);

        // Remove trailing 0's from the decimal point
        for(i=len-1;i>=2;i--) {
            if(raw[i] == '.') { raw[i] = 0; break; }
            if(raw[i] == '0') raw[i] = 0; else break;
        }

        // Append "*10^x" to the string
        len = strlen(raw);
        sprintf(raw+len, "*10^%d]", x);

    } else {
        sprintf(raw,"%lf", in);

        for(i=strlen(raw)-1;i>=1;i--) {
            if(raw[i] == '.') { raw[i] = 0; break; }
            if(raw[i] == '0') raw[i] = 0; else break;
        }

        // Remove "-0"
        if(strcmp(raw, "-0") == 0) { raw[0] = '0'; raw[1] = 0;}
    }

    // Conserve memory
    char* ret = calloc(strlen(raw)+1, 1);
    strcpy(ret, raw);
    free(raw);

    return ret;
}

#endif