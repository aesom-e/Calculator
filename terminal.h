#ifdef WIN32
#include <windows.h>
#endif
#ifdef linux
#include <stdio.h>
#endif

#ifndef TERMINAL_H
#define TERMINAL_H

#ifdef WIN32
void clearTerminal() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD consoleSize, charsWritten;

    if(!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;

    consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the console with spaces
    FillConsoleOutputCharacter(hConsole, ' ', consoleSize, (COORD){0, 0}, &charsWritten);

    // Reset the text attributes
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, (COORD){0, 0}, &charsWritten);

    // Move the cursor to the top left corner
    SetConsoleCursorPosition(hConsole, (COORD){0, 0});
}
#endif
#ifdef linux
void clearTerminal() {
    printf("\033[2J\033[1;1H");
    fflush(stdout);
}
#endif

#endif