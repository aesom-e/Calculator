#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "solver.h"
#include "tokenArrayTools.h"

char* takeInput() {
    char* ret = malloc(MAXLENGTH);
    printf("$%c", -1);
    fgets(ret, MAXLENGTH-1, stdin);
    return ret;
}

int main(int argc, char** argv) {

    // Run a single expression from the command line
    if(argc >= 2) {
        if(strcmp(argv[1], "-load") != 0) {
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
    }

    // Load a file for things like function definitions
    if(argc == 3) {
        FILE* startFile = fopen(argv[2], "r");
        if(startFile == NULL) {
            printf("Unable to load file %s", argv[2]);
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