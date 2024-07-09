#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "base.h"
#include "graphSolver.h"
#include "doubleTools.h"
#include "Windows/resource.h"
#include "constants.h"

#ifndef GRAPHWIN_H
#define GRAPHWIN_H

function graphFunction;
HWND graphHwnd;
HMENU optionsHmenu;

double xRange[2] = {-9.6, 9.6}, dragXRange[2];
double yRange[2] = {-5.4, 5.4}, dragYRange[2];
double cursorPos[2] = {0, 0};
unsigned char showAxes = 0, showCoords = 1, clickDragging = 0, limitsEnabled = 1;
POINT dragStartPoint;

void drawAxes(HDC hdc, RECT rect) {
    int width  = rect.right - rect.left,
        height = rect.bottom - rect.top;

    if(showAxes) {
        int x0 = (int)((0 - xRange[0]) / (xRange[1] - xRange[0]) * width);
        int y0 = height - (int)((0 - yRange[0]) / (yRange[1] - yRange[0]) * height);

        HPEN hRedPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN hPenOld = (HPEN)SelectObject(hdc, hRedPen);

        MoveToEx(hdc, 0, y0, NULL);
        LineTo(hdc, width, y0);

        MoveToEx(hdc, x0, 0, NULL);
        LineTo(hdc, x0, height);

        SelectObject(hdc, hPenOld);
        DeleteObject(hRedPen);
    }
}

void updateWindowTitle() {
    char className[256], *rangeNames[4], *cursorPosNames[2];

    rangeNames[0] = compactDouble(xRange[0]); rangeNames[1] = compactDouble(yRange[0]);
    rangeNames[2] = compactDouble(xRange[1]); rangeNames[3] = compactDouble(yRange[1]);

    if(showCoords) {
        cursorPosNames[0] = compactDouble(cursorPos[0]); cursorPosNames[1] = compactDouble(cursorPos[1]);
        sprintf(className, "Graph of %s(%c) from (%s, %s) to (%s, %s) at (%s, %s)",
                graphFunction.name, graphFunction.arguments[0], rangeNames[0], rangeNames[1], rangeNames[2], rangeNames[3],
                cursorPosNames[0], cursorPosNames[1]);
    } else {
        sprintf(className, "Graph of %s(%c) from (%s, %s) to (%s, %s)",
                graphFunction.name, graphFunction.arguments[0], rangeNames[0], rangeNames[1], rangeNames[2], rangeNames[3]);
    }

    SetWindowText(graphHwnd, className);

    free(rangeNames[0]); free(rangeNames[1]);
    free(rangeNames[2]); free(rangeNames[3]);
}

void screenToGraph(int x, int y, double* graphX, double* graphY) {
    RECT rect;
    GetClientRect(graphHwnd, &rect);

    int width  = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    *graphX = xRange[0] + (xRange[1] - xRange[0]) * x / width;
    *graphY = yRange[0] + (yRange[1] - yRange[0]) * (height - y) / height;
}

void resetRanges() {
    xRange[0] = -9.6; xRange[1] = 9.6;
    yRange[0] = -5.4; yRange[1] = 5.4;

    updateWindowTitle();
    InvalidateRect(graphHwnd, NULL, TRUE); // Redraw the main window
}

void updateOptionsMenu() {
    ModifyMenu(optionsHmenu, ID_OPTIONS_TOGGLEAXES, MF_BYCOMMAND | MF_STRING, ID_OPTIONS_TOGGLEAXES,
               showAxes ? "Disable &Axes" : "Enable &Axes");


    ModifyMenu(optionsHmenu, ID_OPTIONS_TOGGLECOORDS, MF_BYCOMMAND | MF_STRING, ID_OPTIONS_TOGGLECOORDS,
               showCoords ? "Disable Showing &Coords" : "Enable Showing &Coords");


    ModifyMenu(optionsHmenu, ID_OPTIONS_TOGGLELIMITS, MF_BYCOMMAND | MF_STRING, ID_OPTIONS_TOGGLELIMITS,
               limitsEnabled ? "Disable &Limits" : "Enable &Limits");
}

void zoom(int zoomIn, double xValue, double yValue, double xRatio, double yRatio, int large) {
    double zoomFactor;
    if(zoomIn) zoomFactor = large ? ZOOM_FACTOR_LARGE : ZOOM_FACTOR;
    else zoomFactor = large ? 1/ZOOM_FACTOR_LARGE : 1/ZOOM_FACTOR;

    // Calculate the new size of the ranges after zoom
    double newXRangeHalf = ((xRange[1] - xRange[0]) / 2) * zoomFactor;
    double newYRangeHalf = ((yRange[1] - yRange[0]) / 2) * zoomFactor;

    // Calculate the amount to shift the center by
    double shiftX = (xRatio - 0.5) * (newXRangeHalf * 2);
    double shiftY = (yRatio - 0.5) * (newYRangeHalf * 2);

    // Set the new centers of the ranges to keep the cursor's position fixed
    double newXRangeCenter = xValue - shiftX;
    double newYRangeCenter = yValue - shiftY;

    // Check if the new ranges are within the limits
    if(limitsEnabled) {
        if(fabs(newXRangeCenter + newXRangeHalf) >= LIMIT_MAX
        || fabs(newYRangeCenter + newYRangeHalf) >= LIMIT_MAX) return;
        if(fabs(newXRangeCenter - newXRangeHalf) <= LIMIT_MIN
        || fabs(newYRangeCenter - newYRangeHalf) <= LIMIT_MIN) return;
    }

    // Update the ranges
    xRange[0] = newXRangeCenter - newXRangeHalf;
    xRange[1] = newXRangeCenter + newXRangeHalf;
    yRange[0] = newYRangeCenter - newYRangeHalf;
    yRange[1] = newYRangeCenter + newYRangeHalf;

    // Update the window title and redraw the window
    updateWindowTitle();
    InvalidateRect(graphHwnd, NULL, TRUE); // Redraw the window
}

void pan(double xAmount, double yAmount, const double* startXRange, const double* startYRange) {
    double xRangeSize = startXRange[1] - startXRange[0];
    double yRangeSize = startYRange[1] - startYRange[0];

    double xRangeTemp[2], yRangeTemp[2];
    xRangeTemp[0] = startXRange[0] - xAmount * xRangeSize;
    xRangeTemp[1] = startXRange[1] - xAmount * xRangeSize;
    yRangeTemp[0] = startYRange[0] + yAmount * yRangeSize;
    yRangeTemp[1] = startYRange[1] + yAmount * yRangeSize;

    if(limitsEnabled) {
        if(fabs(xRangeTemp[0]) >= LIMIT_MAX || fabs(xRangeTemp[1]) >= LIMIT_MAX
        || fabs(yRangeTemp[0]) >= LIMIT_MAX || fabs(yRangeTemp[1]) >= LIMIT_MAX) return;

        if(fabs(xRangeTemp[0]) <= LIMIT_MIN || fabs(xRangeTemp[1]) <= LIMIT_MIN
        || fabs(yRangeTemp[0]) <= LIMIT_MIN || fabs(yRangeTemp[1]) <= LIMIT_MIN) return;
    }

    xRange[0] = xRangeTemp[0]; xRange[1] = xRangeTemp[1];
    yRange[0] = yRangeTemp[0]; yRange[1] = yRangeTemp[1];

    updateWindowTitle();
    InvalidateRect(graphHwnd, NULL, TRUE); // Redraw the window
}

INT_PTR CALLBACK SetRangesDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            char buffer[64];

            sprintf(buffer, "%s", compactDouble(xRange[0]));
            SetDlgItemText(hDlg, IDC_XMIN, buffer);
            sprintf(buffer, "%s", compactDouble(xRange[1]));
            SetDlgItemText(hDlg, IDC_XMAX, buffer);
            sprintf(buffer, "%s", compactDouble(yRange[0]));
            SetDlgItemText(hDlg, IDC_YMIN, buffer);
            sprintf(buffer, "%s", compactDouble(yRange[1]));
            SetDlgItemText(hDlg, IDC_YMAX, buffer);
        }
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if(LOWORD(wParam) == IDOK) {
                char buffer[64];

                GetDlgItemText(hDlg, IDC_XMIN, buffer, sizeof(buffer));
                xRange[0] = atof(buffer);
                GetDlgItemText(hDlg, IDC_XMAX, buffer, sizeof(buffer));
                xRange[1] = atof(buffer);
                GetDlgItemText(hDlg, IDC_YMIN, buffer, sizeof(buffer));
                yRange[0] = atof(buffer);
                GetDlgItemText(hDlg, IDC_YMAX, buffer, sizeof(buffer));
                yRange[1] = atof(buffer);

                if(xRange[0] >= xRange[1] || yRange[0] >= yRange[1]) resetRanges();

                updateWindowTitle();

                EndDialog(hDlg, LOWORD(wParam));
                InvalidateRect(GetParent(hDlg), NULL, TRUE); // Redraw the main window
                return (INT_PTR)TRUE;
            } else if(LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_OPTIONS_SETRANGES:
                    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETRANGES), hwnd, SetRangesDlgProc);
                    break;
                case ID_OPTIONS_RESETRANGES:
                    resetRanges();
                    break;
                case ID_OPTIONS_TOGGLEAXES:
                    showAxes = !showAxes;
                    InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
                    break;
                case ID_OPTIONS_TOGGLECOORDS:
                    showCoords = !showCoords;
                    break;
                case ID_OPTIONS_TOGGLELIMITS:
                    limitsEnabled = !limitsEnabled;
                    break;
            }
            updateWindowTitle();
            updateOptionsMenu();
            break;
        case WM_LBUTTONDOWN: {
            clickDragging = 1;
            dragStartPoint.x = GET_X_LPARAM(lParam);
            dragStartPoint.y = GET_Y_LPARAM(lParam);

            dragXRange[0] = xRange[0];
            dragXRange[1] = xRange[1];
            dragYRange[0] = yRange[0];
            dragYRange[1] = yRange[1];

            SetCapture(hwnd);
            break;
        }
        case WM_LBUTTONUP: {
            clickDragging = 0;
            ReleaseCapture();
            break;
        }
        case WM_RBUTTONDOWN: {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            double graphX, graphY;
            screenToGraph(xPos, yPos, &graphX, &graphY);

            cursorPos[0] = graphX;
            cursorPos[1] = graphY;

            updateWindowTitle();

            break;
        }
        case WM_MOUSEMOVE: {
            // Handle click-dragging
            if(clickDragging) {
                POINT currentPoint;
                currentPoint.x = GET_X_LPARAM(lParam);
                currentPoint.y = GET_Y_LPARAM(lParam);

                RECT rect;
                GetWindowRect(hwnd, &rect);

                double deltaX = (double)(currentPoint.x - dragStartPoint.x) / (rect.right - rect.left);
                double deltaY = (double)(currentPoint.y - dragStartPoint.y) / (rect.bottom - rect.top);

                pan(deltaX, deltaY, dragXRange, dragYRange);
                break;
            }

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            screenToGraph(xPos, yPos, &cursorPos[0], &cursorPos[1]);

            updateWindowTitle();

            break;
        }
        case WM_MOUSEWHEEL: {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(graphHwnd, &pt);

            double xValue, yValue;
            screenToGraph(pt.x, pt.y, &xValue, &yValue);

            // Calculate the ratios
            double xRatio = (xValue - xRange[0]) / (xRange[1] - xRange[0]);
            double yRatio = (yValue - yRange[0]) / (yRange[1] - yRange[0]);

            zoom((GET_WHEEL_DELTA_WPARAM(wParam) <= 0), xValue, yValue, xRatio, yRatio, (GetKeyState(VK_CONTROL) & 0x8000) != 0);

            break;
        }
        case WM_KEYDOWN: {
            int ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            int shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

            switch(wParam) {
                case 'X':
                    if(shiftPressed) break;
                    if(ctrlPressed) {
                        showAxes = !showAxes;
                        updateOptionsMenu();
                        InvalidateRect(hwnd, NULL, TRUE); // Redraw the window
                    }
                    break;
                case 'R':
                    if(ctrlPressed) {
                        if(shiftPressed) {
                            resetRanges();
                            break;
                        }
                        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETRANGES), hwnd, SetRangesDlgProc);
                    }
                    break;
                case 'C':
                    if(shiftPressed) break;
                    if(ctrlPressed) {
                        showCoords = !showCoords;
                        updateOptionsMenu();
                    }
                    break;
                case 'L':
                    if(shiftPressed) break;
                    if(ctrlPressed) {
                        limitsEnabled = !limitsEnabled;
                        updateOptionsMenu();
                    }
                    break;
                case VK_ADD:
                case VK_OEM_PLUS:
                    if(ctrlPressed)  zoom(0, (xRange[0] + xRange[1])/2, (yRange[0] + yRange[1])/2,
                                          0.5, 0.5, shiftPressed);
                    break;
                case VK_SUBTRACT:
                case VK_OEM_MINUS:
                    if(ctrlPressed)  zoom(1, (xRange[0] + xRange[1])/2, (yRange[0] + yRange[1])/2,
                                          0.5, 0.5, shiftPressed);
                    break;
                case 'W':
                case VK_UP:
                    if(ctrlPressed || shiftPressed) break;
                    pan(0, PAN_STEP, xRange, yRange);
                    break;
                case 'A':
                case VK_LEFT:
                    if(ctrlPressed || shiftPressed) break;
                    pan(PAN_STEP, 0, xRange, yRange);
                    break;
                case 'S':
                case VK_DOWN:
                    if(ctrlPressed || shiftPressed) break;
                    pan(0, -PAN_STEP, xRange, yRange);
                    break;
                case 'D':
                case VK_RIGHT:
                    if(ctrlPressed || shiftPressed) break;
                    pan(-PAN_STEP, 0, xRange, yRange);
                    break;
                case VK_SPACE:
                    InvalidateRect(graphHwnd, NULL, TRUE);
                    updateWindowTitle();
                    break;
            }

            updateWindowTitle();
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(6));

            HPEN hPenOld;
            HPEN hLinePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            hPenOld = (HPEN)SelectObject(hdc, hLinePen);

            drawAxes(hdc, rect);

            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            MoveToEx(hdc, 0, height / 2, NULL); // Center of window

            int i;
            for(i=0;i<width;i++) {

                double x = xRange[0] + (xRange[1] - xRange[0]) * i / width;

                tokenArray math = {0};
                setupFunction(&math, graphFunction, &x);

                double y = graphSolve(&math);
                if(error) { printf("Error getting graph values\n"); exit(-1); }

                // Normalize y to the screen coordinates
                int screenX = i;
                int screenY = height - (int)((y - yRange[0]) / (yRange[1] - yRange[0]) * height);

                LineTo(hdc, screenX, screenY);
            }

            SelectObject(hdc, hPenOld);
            DeleteObject(hLinePen);

            EndPaint(hwnd, &ps);

            updateWindowTitle();
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void graph(function functionIn) {
    graphFunction = functionIn;

    char className[256], *rangeNames[4];

    rangeNames[0] = compactDouble(xRange[0]); rangeNames[1] = compactDouble(yRange[0]);
    rangeNames[2] = compactDouble(xRange[1]); rangeNames[3] = compactDouble(yRange[1]);

    sprintf(className, "Graph of %s(%c)",
            functionIn.name, functionIn.arguments[0]);

    free(rangeNames[0]); free(rangeNames[1]);
    free(rangeNames[2]); free(rangeNames[3]);

    // Setting up the window
    HINSTANCE hinstance = GetModuleHandle(NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hinstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
            0,
            className, className,
            WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT,
            1920, 1080,
            NULL, NULL,
            hinstance,
            NULL
    );

    if(!hwnd) {
        printf("Error: Window could not be created (%ld)\n", GetLastError());
        return;
    }

    graphHwnd = hwnd;

    HMENU hMenu = LoadMenu(hinstance, MAKEINTRESOURCE(IDR_MENU));
    if(hMenu) SetMenu(hwnd, hMenu);

    optionsHmenu = hMenu;
    updateOptionsMenu();

    resetRanges();
    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {0};
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#endif
#endif