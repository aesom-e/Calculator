#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "customFunctions.h"

#ifndef GRAPHWIN_H
#define GRAPHWIN_H

double* graphData;
const unsigned short graphWidth = 1920;

double minGraphPoint() {
    double ret = DBL_MAX;
    int i;
    for(i=0;i<graphWidth;i++) if(ret > graphData[i]) ret = graphData[i];
    return ret;
}

double maxGraphPoint() {
    double ret = -DBL_MAX;
    int i;
    for(i=0;i<graphWidth;i++) if(ret < graphData[i]) ret = graphData[i];
    return ret;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(6));

            HPEN hPenOld;
            HPEN hLinePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            hPenOld = (HPEN)SelectObject(hdc, hLinePen);

            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            double yScale = 10, yAdjustment;

            if(minGraphPoint() >= 0) yAdjustment = -960;

            double X_MIN = -540, X_MAX = 540,
                   Y_MIN = yScale * (-960 - yAdjustment), Y_MAX = yScale * (960 - yAdjustment);

            MoveToEx(hdc, 0, height / 2, NULL); // Center of window

            int i;
            for(i=0;i<width;i++) {
                double x = X_MIN + (X_MAX - X_MIN) * i / width;
                double y = graphData[i];

                // Normalize y to the screen coordinates
                int screenX = i;
                int screenY = height - (int)((y - Y_MIN) / (Y_MAX - Y_MIN) * height);

                LineTo(hdc, screenX, screenY);
            }

            SelectObject(hdc, hPenOld);
            DeleteObject(hLinePen);

            EndPaint(hwnd, &ps);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void graph(function functionIn, double* precomputedAnswers) {
    graphData = precomputedAnswers;

    // Create & show the window
    char className[32];
    sprintf(className, "Graph of %s(%c)", functionIn.name, functionIn.arguments[0]);

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
            graphWidth, 1080,
            NULL, NULL,
            hinstance,
            NULL
    );

    if(hwnd == NULL) {
        printf("Error: Window could not be created (%ld)\n", GetLastError());
        return;
    }

    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {0};
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#endif
#endif