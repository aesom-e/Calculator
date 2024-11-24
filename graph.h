#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include "base.h"
#include "graphSolver.h"
#include "doubleTools.h"
#include "constants.h"
#include "colour.h"
#include "windowIcon.h"

#ifndef GRAPHSDL_H
#define GRAPHSDL_H

typedef struct {
    SDL_Window* graphWindow;
    SDL_Renderer* graphRenderer;
    function graphFunctions[GRAPH_MAX_FUNCTIONS];
    int numFunctions;
    double xRange[2], dragXRange[2];
    double yRange[2], dragYRange[2];
    double cursorPos[2];
    unsigned char showAxes, showCoords, clickDragging;
    int height, width, dragStart[2];
} graphData;

int numWindows = 0;
SDL_mutex* numWindowsMutex = NULL;

void screenToGraph(graphData* context, int x, int y, double* graphX, double* graphY) {
    *graphX = context->xRange[0] + (context->xRange[1] - context->xRange[0]) * x / context->width;
    *graphY = context->yRange[0] + (context->yRange[1] - context->yRange[0]) * (context->height - y) / context->height;
}

void updateWindowTitle(graphData* context) {
    char graphTitle[2048], *rangeNames[4], *cursorPosNames[2], functionNames[1792] = {0};

    rangeNames[0] = compactDouble(context->xRange[0]); rangeNames[1] = compactDouble(context->yRange[0]);
    rangeNames[2] = compactDouble(context->xRange[1]); rangeNames[3] = compactDouble(context->yRange[1]);

    int i;
    for(i=0;i<context->numFunctions;i++) {
        char functionName[256];
        snprintf(functionName, 255, "%s(%c)", context->graphFunctions[i].name,
                 context->graphFunctions[i].arguments[0]);
        strncat(functionNames, functionName, 1792 - strlen(functionNames) - 1);
        if(i < context->numFunctions-1)
            strncat(functionNames, "; ", 1792 - strlen(functionNames) - 1);
    }

    if(context->showCoords) {
        cursorPosNames[0] = compactDouble(context->cursorPos[0]); cursorPosNames[1] = compactDouble(context->cursorPos[1]);
        snprintf(graphTitle, 2047, "Graph of %s from (%s, %s) to (%s, %s) at (%s, %s)",
                 functionNames, rangeNames[0], rangeNames[1],
                rangeNames[2], rangeNames[3],
                cursorPosNames[0], cursorPosNames[1]);
    } else {
        snprintf(graphTitle, 2047, "Graph of %s from (%s, %s) to (%s, %s)",
                functionNames, rangeNames[0], rangeNames[1],
                rangeNames[2], rangeNames[3]);
    }

    SDL_SetWindowTitle(context->graphWindow, graphTitle);

    free(rangeNames[0]); free(rangeNames[1]);
    free(rangeNames[2]); free(rangeNames[3]);
}

void drawWindowIcon(graphData* context) {
    SDL_Surface* iconSurface = SDL_CreateRGBSurfaceFrom(
                               (void*)windowIcon,
                               64, 64, 32, 256,
                               0x000000ff,
                               0x0000ff00,
                               0x00ff0000,
                               0xff000000);

    if(!iconSurface) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return;
    }

    SDL_SetWindowIcon(context->graphWindow, iconSurface);
    SDL_FreeSurface(iconSurface);
}

void resetRanges(graphData* context) {
    context->xRange[0] = -9.6; context->xRange[1] = 9.6;
    context->yRange[0] = -5.4; context->yRange[1] = 5.4;

    updateWindowTitle(context);
}

void drawLine(graphData* context, int x1, int y1, int x2, int y2, unsigned int colourValue) {
    colour colourStruct = colour_GetColour(colourValue);
    SDL_SetRenderDrawColor(context->graphRenderer, colourStruct.r, colourStruct.g, colourStruct.b, 200);
    int i;
    for(i=-(GRAPH_LINE_THICKNESS/2);i<=(GRAPH_LINE_THICKNESS/2);i++) {
        SDL_RenderDrawLine(context->graphRenderer, x1 + i, y1, x2 + i, y2);
        SDL_RenderDrawLine(context->graphRenderer, x1, y1 + i, x2, y2 + i);
    }

    SDL_SetRenderDrawColor(context->graphRenderer, colourStruct.r, colourStruct.g, colourStruct.b, 255);
    SDL_RenderDrawLine(context->graphRenderer, x1, y1, x2, y2);
}

void drawAxes(graphData* context) {
    if(!context->showAxes) return;

    int xAxis = (int)((0 - context->xRange[0]) / (context->xRange[1] - context->xRange[0]) * context->width);
    int yAxis = context->height - (int)((0 - context->yRange[0]) /
                (context->yRange[1] - context->yRange[0]) * context->height);

    drawLine(context, 0, yAxis, context->width, yAxis, GRAPH_AXIS_COLOUR);
    drawLine(context, xAxis, 0, xAxis, context->height, GRAPH_AXIS_COLOUR);
}

void pan(graphData* context, double xAmount, double yAmount, const double* startXRange, const double* startYRange) {
    double xRangeSize = startXRange[1] - startXRange[0];
    double yRangeSize = startYRange[1] - startYRange[0];

    double xRangeTemp[2], yRangeTemp[2];
    xRangeTemp[0] = startXRange[0] - xAmount * xRangeSize;
    xRangeTemp[1] = startXRange[1] - xAmount * xRangeSize;
    yRangeTemp[0] = startYRange[0] + yAmount * yRangeSize;
    yRangeTemp[1] = startYRange[1] + yAmount * yRangeSize;

    context->xRange[0] = xRangeTemp[0]; context->xRange[1] = xRangeTemp[1];
    context->yRange[0] = yRangeTemp[0]; context->yRange[1] = yRangeTemp[1];

    updateWindowTitle(context);
}

void zoom(graphData* context, int zoomIn, double xValue, double yValue, double xRatio, double yRatio, int large) {
    double zoomFactor;
    if(zoomIn) zoomFactor = large ? ZOOM_FACTOR_LARGE : ZOOM_FACTOR;
    else zoomFactor = large ? 1/ZOOM_FACTOR_LARGE : 1/ZOOM_FACTOR;

    // Calculate the new size of the ranges after zoom
    double newXRangeHalf = ((context->xRange[1] - context->xRange[0]) / 2) * zoomFactor;
    double newYRangeHalf = ((context->yRange[1] - context->yRange[0]) / 2) * zoomFactor;

    // Calculate the amount to shift the center by
    double shiftX = (xRatio - 0.5) * (newXRangeHalf * 2);
    double shiftY = (yRatio - 0.5) * (newYRangeHalf * 2);

    // Set the new centers of the ranges to keep the cursor's position fixed
    double newXRangeCenter = xValue - shiftX;
    double newYRangeCenter = yValue - shiftY;

    // Update the ranges
    context->xRange[0] = newXRangeCenter - newXRangeHalf;
    context->xRange[1] = newXRangeCenter + newXRangeHalf;
    context->yRange[0] = newYRangeCenter - newYRangeHalf;
    context->yRange[1] = newYRangeCenter + newYRangeHalf;

    updateWindowTitle(context);
}

void drawGraph(graphData* context) {
    int f;
    for(f=0;f<context->numFunctions;f++) {
        // Get the colour that the line should be
        int lineColour;
        switch(f) {
            case 0:  lineColour = GRAPH_LINE_1; break;
            case 1:  lineColour = GRAPH_LINE_2; break;
            case 2:  lineColour = GRAPH_LINE_3; break;
            case 3:  lineColour = GRAPH_LINE_4; break;
            case 4:  lineColour = GRAPH_LINE_5; break;
            default: lineColour = GRAPH_LINE_1; break;
        }

        int i, validPoint = 0;
        int lastValidX = -1, lastValidY = -1;
        // Check if the argument to the function is 'y', then graph as y = f(y), else x = f(<any>)
        if(context->graphFunctions[f].arguments[0] == 'y') {
            for(i=0;i<context->height;i++) {
                double y = context->yRange[0] + (context->yRange[1] - context->yRange[0]) * i / context->height;

                tokenArray math = {0};
                setupFunction(&math, context->graphFunctions[f], &y);

                double x = graphSolve(&math);
                if(error) { printf("Error getting graph values\n"); quit(-1); }

                if(!isnan(x) && !isinf(x)) {
                    // Normalize x to the screen coordinates
                    int screenX = (int)((x - context->xRange[0]) / (context->xRange[1] - context->xRange[0]) * context->width);
                    int screenY = context->height - i;

                    if(validPoint) drawLine(context, lastValidX, lastValidY,
                                            screenX, screenY, lineColour);

                    lastValidX = screenX;
                    lastValidY = screenY;
                    validPoint =  1;
                } else validPoint = 0;
            }
        } else {
            for(i=0;i<context->width;i++) {
                double x = context->xRange[0] + (context->xRange[1] - context->xRange[0]) * i / context->width;

                tokenArray math = {0};
                setupFunction(&math, context->graphFunctions[f], &x);

                double y = graphSolve(&math);
                if(error) { printf("Error getting graph values\n"); quit(-1); }

                if(!isnan(y) && !isinf(y)) {
                    // Normalize y to the screen coordinates
                    int screenX = i;
                    int screenY = context->height - (int)((y - context->yRange[0]) /
                                                          (context->yRange[1] - context->yRange[0]) * context->height);

                    if(validPoint) drawLine(context, lastValidX, lastValidY,
                                            screenX, screenY, lineColour);

                    lastValidX = screenX;
                    lastValidY = screenY;
                    validPoint =  1;
                } else validPoint = 0;
            }
        }
    }
}

void processEvent(graphData* context, SDL_Event event) {
    switch(event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if(event.button.button == SDL_BUTTON_LEFT) {
                context->clickDragging = 1;
                context->dragStart[0] = event.button.x;
                context->dragStart[1] = event.button.y;

                context->dragXRange[0] = context->xRange[0];
                context->dragXRange[1] = context->xRange[1];
                context->dragYRange[0] = context->yRange[0];
                context->dragYRange[1] = context->yRange[1];
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if(event.button.button == SDL_BUTTON_LEFT) context->clickDragging = 0;
            break;
        }

        case SDL_MOUSEMOTION: {
            if(context->clickDragging) {
                double deltaX = (double)(event.motion.x - context->dragStart[0]) / (double)context->width;
                double deltaY = (double)(event.motion.y - context->dragStart[1]) / (double)context->height;

                pan(context, deltaX, deltaY, context->dragXRange, context->dragYRange);
                break;
            }
            // Update the title if not click-dragging
            screenToGraph(context, event.button.x, event.button.y, &(context->cursorPos[0]), &(context->cursorPos[1]));
            updateWindowTitle(context);
            break;
        }

        case SDL_MOUSEWHEEL: {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Convert mouse position to graph coordinates
            double xValue, yValue;
            screenToGraph(context, mouseX, mouseY, &xValue, &yValue);

            // Calculate the ratios
            double xRatio = (xValue - context->xRange[0]) / (context->xRange[1] - context->xRange[0]);
            double yRatio = (yValue - context->yRange[0]) / (context->yRange[1] - context->yRange[0]);

            // Zoom
            zoom(context, event.wheel.y < 0, xValue, yValue,
                 xRatio, yRatio, (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL]));
            break;
        }

        case SDL_KEYDOWN: {
            int ctrlPressed = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL];
            int shiftPressed = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT];

            switch(event.key.keysym.sym) {
                case 'x': {
                    if(!shiftPressed && ctrlPressed) context->showAxes = !context->showAxes;
                    break;
                }
                case 'r': {
                    if(shiftPressed && ctrlPressed) resetRanges(context);
                    break;
                }
                case 'c': {
                    if(!shiftPressed && ctrlPressed) context->showCoords = !context->showCoords;
                    break;
                }
                case SDLK_KP_PLUS:
                case SDLK_EQUALS: {
                    if(ctrlPressed)  zoom(context, 0, (context->xRange[0] + context->xRange[1])/2,
                                          (context->yRange[0] + context->yRange[1])/2, 0.5, 0.5, shiftPressed);
                    break;
                }
                case SDLK_KP_MINUS:
                case SDLK_MINUS: {
                    if(ctrlPressed)  zoom(context, 1, (context->xRange[0] + context->xRange[1])/2,
                                          (context->yRange[0] + context->yRange[1])/2, 0.5, 0.5, shiftPressed);
                    break;
                }
                case 'w':
                case SDLK_UP: {
                    if(shiftPressed) break;
                    pan(context, 0, (ctrlPressed ? PAN_STEP_LARGE : PAN_STEP),
                        context->xRange, context->yRange);
                    break;
                }
                case 'a':
                case SDLK_LEFT: {
                    if(shiftPressed) break;
                    pan(context, (ctrlPressed ? PAN_STEP_LARGE : PAN_STEP), 0,
                        context->xRange, context->yRange);
                    break;
                }
                case 's':
                case SDLK_DOWN: {
                    if(shiftPressed) break;
                    pan(context, 0, -(ctrlPressed ? PAN_STEP_LARGE : PAN_STEP),
                        context->xRange, context->yRange);
                    break;
                }
                case 'd':
                case SDLK_RIGHT: {
                    if(shiftPressed) break;
                    pan(context, -(ctrlPressed ? PAN_STEP_LARGE : PAN_STEP), 0,
                        context->xRange, context->yRange);
                    break;
                }
            }
            break;
        }
    }
}

int graphFunc(void* in) {
    graphData* context = (graphData*)in;
    context->showCoords = 1;
    context->showAxes = 0;

    // Initialize the numWindowsMutex if necessary
    if(!numWindowsMutex) {
        numWindowsMutex = SDL_CreateMutex();
        if(!numWindowsMutex) {
            printf("Failed to create mutex: %s\n", SDL_GetError());
            return 1;
        }
    }

    // Increment the number of windows
    SDL_LockMutex(numWindowsMutex);
    numWindows++;
    SDL_UnlockMutex(numWindowsMutex);

    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    context->graphWindow = SDL_CreateWindow("Graph",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            GRAPH_WIDTH, GRAPH_HEIGHT,
                                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(!context->graphWindow) {
        printf("Window failed to show: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    context->graphRenderer = SDL_CreateRenderer(context->graphWindow, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!context->graphRenderer) {
        printf("Renderer failed to create: %s\n", SDL_GetError());
        SDL_DestroyWindow(context->graphWindow);
        SDL_Quit();
        return 1;
    }

    // Draw the window icon
    drawWindowIcon(context);

    // Reset the variables for this graph
    resetRanges(context);

    // Main render loop
    int running = 1;
    SDL_Event event;
    while(running) {
        while(SDL_PollEvent(&event)) {
            // Check if the current window was closed
            if(event.type == SDL_WINDOWEVENT
            && event.window.event == SDL_WINDOWEVENT_CLOSE
            && event.window.windowID == SDL_GetWindowID(context->graphWindow))
                running = 0;
            else processEvent(context, event);
        }

        SDL_SetRenderDrawColor(context->graphRenderer, 255, 255, 255, 255);
        SDL_RenderClear(context->graphRenderer);

        // Get the height and width of the window
        SDL_GetWindowSize(context->graphWindow, &(context->width), &(context->height));

        // Draw the window
        drawAxes(context);
        updateWindowTitle(context);
        SDL_SetRenderDrawBlendMode(context->graphRenderer, SDL_BLENDMODE_INVALID);
        drawGraph(context);

        SDL_RenderPresent(context->graphRenderer);
    }

    // Clean up
    SDL_DestroyRenderer(context->graphRenderer);
    SDL_DestroyWindow(context->graphWindow);
    free(context);

    // Quit SDL if no windows remain
    SDL_LockMutex(numWindowsMutex);
    numWindows--;
    if(!numWindows) SDL_Quit();
    SDL_UnlockMutex(numWindowsMutex);

    return 0;
}

void graph(function* functionsIn, int numFunctions) {
    graphData* data = calloc(1, sizeof(graphData));

    int i;
    for(i=0;i<numFunctions&&i<GRAPH_MAX_FUNCTIONS;i++) data->graphFunctions[i] = functionsIn[i];
    data->numFunctions = numFunctions > GRAPH_MAX_FUNCTIONS ? GRAPH_MAX_FUNCTIONS : numFunctions;

    SDL_Thread* thread = SDL_CreateThread(graphFunc, "GraphThread", data);

    if(!thread) {
        free(data);
        printf("Failed to create thread: %s\n", SDL_GetError());
    } else {
        SDL_DetachThread(thread);
    }
}

#endif