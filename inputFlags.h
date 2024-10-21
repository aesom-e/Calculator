#include <string.h>

#ifndef INPUTFLAGS_H
#define INPUTFLAGS_H

typedef struct {
    unsigned full: 1;
    unsigned scientificNotation: 1;
    char* fileToLoad;
} inputFlags;

inputFlags getInputFlags(int argc, char** argv) {
    inputFlags ret = {0};

    int i;
    for(i=0;i<argc;i++) {
        if(strcmp(argv[i], "-full") == 0) ret.full = 1;

        if(strcmp(argv[i], "-sn") == 0) ret.scientificNotation = 1;

        if(strcmp(argv[i], "-load") == 0 && i+1 < argc) ret.fileToLoad = argv[i+1];
    }

    return ret;
}

#endif