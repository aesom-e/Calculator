#include <stdio.h>

#ifndef HELP_H
#define HELP_H

void printHelp() {
    printf(""
           "Settings:\n"
           "printFull          | Prints the current setting for printing full solutions\n"
           "printFull = x      | Sets printFull to x {x = 0,1}\n"
           "sciNotation        | Prints the current setting for printing solutions in scientific notation\n"
           "sciNotation = x    | Sets sciNotation to x {x = 0,1}\n\n"

           "Commands:\n"
           "help               | Brings up this menu\n"
           "clear              | Clears the terminal screen\n"
           "cls                | Same as \"clear\"\n\n"

           "Constants:\n"
           "pi                 | ~= 3.141593\n"
           "euler              | ~= 2.718282\n"
           "ans                | The previous result\n\n"

           "Operations:\n"
           "x + y              | Adds x and y together\n"
           "x - y              | Subtracts y from x\n"
           "x / y              | Divides x by y\n"
           "x * y              | Multiplies x by y\n"
           "x y                | Multiplies x by y\n"
           "x(y)               | Multiplies x by y\n"
           "x^y                | Returns x to the power of y\n"
           "x!                 | Returns x factorial\n\n"

           "Functions:\n"
           "sqrt x             | Returns the square root of x\n"
           "x root y           | Returns the xth root of y\n"
           "sine x             | Returns the sine of x in radians\n"
           "cosine x           | Returns the cosine of x in radians\n"
           "tangent x          | Returns the tangent of x in radians\n"
           "log x              | Returns the base 10 logarithm of x\n"
           "log x y            | Returns the base x logarithm of y\n"
           "(log x) y          | Returns the (base 10 logarithm of x) multiplied by y\n"
           "ln x               | Returns the natural logarithm (base e) of x\n"
           "radToDeg x         | Converts x from radians to degrees\n"
           "degToRad x         | Converts x from degrees to radians\n"
           "abs x              | Returns the absolute value of x\n"
           "round x y          | Rounds the number x to y decimal points\n"
           "sgn x              | Returns 0 if x = 0, 1 if x > 0, and -1 if x < 0\n\n"

           "Custom Functions\n"
           "Declaration:\n"
           " f(x) = y          | Sets f(x) to y (f can be any string of characters < 32 characters)\n"
           " double(x) = 2x    | Sets double(x) to 2 * x\n"
           " f(x, y) = xy      | Sets f(x, y) to x * y\n"
           "describe f         | Returns what makes up x (in this case: f(x, y) = xy)\n"
           "graph double       | Graphs the double function where y = double(x)\n"
           "                     Functions must have only one argument to be graphed\n\n"

           "Graphing\n"
           "Actions:\n"
           " Scroll Wheel      | Zooms into/out of a portion of the graph\n"
           " Click Dragging    | Pans around the graph\n"
           "Keybinds:\n"
           " Ctrl X            | Toggle axes\n"
           " Ctrl Shift R      | Resets the range of the graph to (-960, -540), (960, 540)\n"
           " Ctrl C            | Toggles showing the coordinates of the cursor\n"
           " Ctrl +            | Zooms in on the graph\n"
           " Ctrl -            | Zooms out on the graph\n"
           " WASD/Arrow keys   | Pans around the graph\n"
           " Space             | Redraw the graph\n");
}

#endif