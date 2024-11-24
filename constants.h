#include "colour.h"

#define MAX_INPUT_LENGTH 4096
#define MAX_FUNCTIONS 64
#define MAX_VARIABLES 256
#define ZOOM_FACTOR 1.1
#define ZOOM_FACTOR_LARGE 1.5
#define PAN_STEP 0.025
#define PAN_STEP_LARGE 0.25
#define GRAPH_HEIGHT 1080
#define GRAPH_WIDTH 1920
#define GRAPH_LINE_THICKNESS 2
#define GRAPH_AXIS_COLOUR colour_GetRGB(0, 0, 0)
#define GRAPH_MAX_FUNCTIONS 5

// Graph line colours
#define GRAPH_LINE_1 colour_GetRGB(199, 68, 64)
#define GRAPH_LINE_2 colour_GetRGB(45, 112, 179)
#define GRAPH_LINE_3 colour_GetRGB(56, 140, 70)
#define GRAPH_LINE_4 colour_GetRGB(250, 126, 25)
#define GRAPH_LINE_5 colour_GetRGB(96, 66, 166)