#ifdef linux
#include <stdio.h>
#include "customFunctions.h"

#ifndef GRAPHLINUX_H
#define GRAPHLINUX_H

double* graphData;
const unsigned short graphWidth = 1920;

void graph(function functionIn, double* precomputedAnswers) {
    printf("Graphing is only available on Windows at the moment\n");
}

#endif
#endif