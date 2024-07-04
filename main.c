#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "solver.h"
#include "tokenArrayTools.h"
#include "inputFlags.h"

char* takeInput() {
    char* ret = malloc(MAXLENGTH);
    #ifdef _WIN32
        printf("$%c", -1); // -1 to print a non-breaking space
    #endif
    #ifdef linux
        printf("$ "); // Linux allows trailing spaces before an input, Windows does not
    #endif
    fgets(ret, MAXLENGTH-1, stdin);
    return ret;
}

int main(int argc, char** argv) {
    inputFlags flags = getInputFlags(argc, argv);

    if(flags.full) printFull = 1;
    if(flags.fileToLoad) {
        FILE* startFile = fopen(flags.fileToLoad, "r");
        if(!startFile) {
            printf("Unable to load file %s\n", flags.fileToLoad);
        } else {
            char* math = malloc(MAXLENGTH);

            createFunctionArray(); // This is required for solve to work
            while(fgets(math, MAXLENGTH-1, startFile) != NULL) {
                tokenArray line = lex(math);
                math = malloc(MAXLENGTH); // lex frees math
                solve(&line);
            }

            fclose(startFile);
            ans = 0;
        }
    }

    // Run a single expression from the command line
    if(!flags.full && !flags.fileToLoad && argc > 1) {
        char* math = malloc(MAXLENGTH);
        math[0] = 0;

        int i;
        for(i=1;i<argc;i++) { strcat(math, argv[i]); strcat(math, " "); }

        tokenArray line = lex(math);
        createFunctionArray(); // This is required for solve to work
        solve(&line);
        if(noPrint) return 1;
        if(inputError) { printf("Error: %s\n", errorMessage); return 2; }
        else printf("%lf\n", ans);

        // lex frees math, so we don't need to free it here
        return 0;
    }

    printf("Note that computers can not accurately represent all numbers, results may be approximate at times\n");
    createFunctionArray();
    while(1) {
        tokenArray line = lex(takeInput());
        solve(&line);
        if(noPrint) continue;
        if(inputError) printf("Error: %s\n", errorMessage);
        else printf("= %lf\n", ans);
    }
}