#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <SDL_main.h>
#ifdef linux
#include <readline/readline.h>
#include <readline/history.h>
#endif
#include "lexer.h"
#include "solver.h"
#include "tokenArrayTools.h"
#include "inputFlags.h"
#include "doubleTools.h"

char* takeInput() {
    char* ret;
    #ifdef _WIN32
        printf("$%c", -1); // -1 to print a non-breaking space
        ret = malloc(MAX_INPUT_LENGTH);
        fgets(ret, MAX_INPUT_LENGTH-1, stdin);
    #endif
    #ifdef linux
        ret = readline("$ ");
        if(ret && *ret) add_history(ret);
    #endif
    return ret;
}

void sigInterrupt() {
    quit(0);
}

int main(int argc, char** argv) {
    // Register sigInterrupt and sigBreak with a custom handler
    signal(SIGINT, sigInterrupt);
    #ifdef WIN32
        signal(SIGBREAK, sigInterrupt);
    #endif

    inputFlags flags = getInputFlags(argc, argv);

    if(flags.full) printFull = 1;
    if(flags.scientificNotation) scientificNotation = 1;

    if(flags.fileToLoad) {
        FILE* startFile = fopen(flags.fileToLoad, "r");
        if(!startFile) {
            printf("Unable to load file %s\n", flags.fileToLoad);
        } else {
            char* math = malloc(MAX_INPUT_LENGTH);

            // This is required for solve to work
            createFunctionArray();

            while(fgets(math, MAX_INPUT_LENGTH-1, startFile) != NULL) {
                tokenArray line = lex(math);
                if(blankRet) continue;
                math = malloc(MAX_INPUT_LENGTH); // lex frees math
                solve(&line);
            }

            fclose(startFile);
            ans = 0;
        }
    }

    // Run a single expression from the command line
    if(!flags.full && !flags.fileToLoad && argc > 1) {
        char* math = malloc(MAX_INPUT_LENGTH);
        math[0] = 0;

        int i;
        for(i=1;i<argc;i++) { strcat(math, argv[i]); strcat(math, " "); }

        tokenArray line = lex(math);
        if(blankRet) quit(0);

        // This is required for solve to work
        createFunctionArray();

        solve(&line);
        if(noPrint) return 1;
        if(inputError) { printf("Error: %s\n", errorMessage); return 2; }
        else { char* ansStr = compactDouble(ans); printf("%s\n", ansStr); free(ansStr); }

        // lex frees math, so we don't need to free it here
        destroyFunctionArray();

        return 0;
    }

    printf("Note that computers can not accurately represent all numbers, results may be approximate at times\n");
    createFunctionArray();
    while(1) {
        tokenArray line = lex(takeInput());
        if(blankRet) continue;
        solve(&line);
        if(noPrint) continue;
        if(inputError) printf("Error: %s\n", errorMessage);
        else { char* ansStr = compactDouble(ans); printf("= %s\n", ansStr); free(ansStr); }
    }
}