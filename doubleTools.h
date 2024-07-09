#include <string.h>

#ifndef DOUBLETOOLS_H
#define DOUBLETOOLS_H

char* compactDouble(double in) {
    char* ret = malloc(32), i;

    sprintf(ret, "%lf", in);

    for(i=strlen(ret)-1;i>=1;i--) {
        if(ret[i] == '.') { ret[i] = 0; break; }
        if(ret[i] == '0') ret[i] = 0; else break;
    }

    // Remove "-0"
    if(strcmp(ret, "-0") == 0) { ret[0] = '0'; ret[1] = 0;}

    return ret;
}

#endif