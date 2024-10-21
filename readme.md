# Calculator / Math Evaluator

## About

This is a project I've been working on which evaluates simple mathematical expressions

## Usage

The program features a simple command line interface

```text
Note that computers can not accurately represent all numbers, results may be approximate at times
$ 
```

You may input expressions next to the $ and an evaluated answer will return

```text
$ 4
= 4
$ 8 * 8
= 64
$ (4)(-3)
= -12
$ 8^4
= 4096
$ 16!
= 20922789888000
```

### Named values

The evaluator includes three named values \
Those are: pi, euler (euler's number), and ans (equal to the previous answer)

```text
$ pi
= 3.141593
$ euler
= 2.718282
$ ans
= 2.718282
$ pi / euler
= 1.155727
$ ans
= 1.155727
```

### Built-in Functions

The evaluator supports simple expressions as well as some functions

```text
$ log 4
= 0.60206
$ log 2 10   (Explanation: Log base 2 of 10)
= 3.321928
$ (log 2) 10 (Explanation: (log base 10 of 2) * 10)
= 3.0103
$ ln 3
= 1.098612
$ sqrt 7
= 2.645751
$ 9 root 400 (Explanation: The 9th root of 400)
= 1.945888
$ sine 1
= 0.841471
$ cos 2
= -0.416147
$ tangent 3
= -0.142547
$ radToDeg 1
= 57.29578
$ degToRad 1
= 0.017453
```

### Custom Functions

The final main feature the evaluator contains is the ability to declare and run custom functions \
To declare a custom function, simply write the name of the function then in brackets the arguments \
Note that the arguments must be 1 letter each and the name may not include already defined keywords

```text
$ f(x) = sqrt(x!)
f(x) defined
$ f(4)
= 4.898979
$ g(x, y, z) = x+2y+3z
g(x, y, z) defined
$ g(8, 199, 20)
= 466
```

Functions can be redefined after their initial definition if it so requires

```text
$ f(x) = 3root(x!)
f(x) redefined
$ f(7)
= 17.145238
$ g(x y z) = x^y^z
g(x, y, z) redefined
$ g(3 3 3)
= 7625597484987
```

Functions can also include other functions

```text
$ functionOne(x) = xxx
functionOne(x) defined
$ functionTwo(x) = sqrt functionOne(x)
functionTwo(x) defined
$ functionTwo(30)
= 164.316767
```

Finally, with custom functions, you can ask the evaluator to describe them

```text
$ quadNeg(a, b, c) = ((-b) - sqrt(b^2 - 4ac)) / 2a
quadNeg(a, b, c) defined
$ quadPos(a, b, c) = ((-b) + sqrt(b^2 - 4ac)) / 2a
quadPos(a, b, c) defined
$ describe quadNeg
quadNeg(a, b, c) = ((-b) - sqrt (b^2 - 4ac)) / 2a
$ describe quadPos
quadPos(a, b, c) = ((-b) + sqrt (b^2 - 4ac)) / 2
```

### Graphing

The evaluator has graphing capabilities with custom functions \
The graph keyword allows the user to graph their functions \
Note that the graph may be inaccurate due to the limitations of the IEEE 754 floating point numbers

Here is how to graph a custom function \
At the moment, graphing is only available on Windows

```text
$ f(x) = cosine x
f(x) defined
$ graph f
Graphing f(x)
```

This spawns a window that contains a cosine wave from (-9.6, -5.4) to (9.6, 5.4)

Graphing can also be done as x = function(y) \
To do this, the parameter of the function must be named 'y' \

```text
$ g(y) = y^3
g(y) defined
$ graph g
Graphing g(y)
```

Navigating around the graph is very simple \
To pan around the graph, click and drag \
Use your mouse's scroll wheel to zoom into and out of sections of the graph

The options menu found in the top right corner of the window contains the following options
```text
Set Ranges                    - Pops up a window which allows
                                you to set the range of the viewport into the graph
Reset Ranges                  - Resets the range of the viewport to
                                (-9.6, -5.4), (9.6, 5.4)
Enable/Disable Axes           - Toggles showing axis lines in red at x=0 and y=0
Enable/Disable Showing Coords - Toggles showing the coordinates of the
                                mouse cursor in the window's title
Enable/Disable Limits         - Toggles enforcing limits on panning and zooming
```

On top of those options, the graphing window contains the following keybinds
```text
Set Ranges            - Ctrl R
Reset Range           - Ctrl Shift R
Toggle Axes           - Ctrl X
Toggle Showing Coords - Ctrl C
Toggle Limits         - Ctrl L
Zoom In               - Ctrl +
Zoom Out              - Ctrl -
Zoom In  (Large zoom) - Ctrl Shift +
Zoom Out (Large zoom) - Ctrl Shift -
Pan                   - WASD & Arrow keys
Large zoom            - Ctrl Scroll wheel
Redraw Graph          - Space
```

## Command-line arguments

The program can accept raw math expressions from the command line and will return the evaluated result \
The limitation to this is that the expression can not include custom functions

```text
MathEvaluator "(7(4)!)/(sine(4!))"
-185.5168
MathEvaluator "sine cosine pi" 
-0.841471
```

### Command-line flags

**-load <​file>** will load any custom functions defined in the specified file \
For example, we can create a customFunctions.math file and populate it as such:

```text
quadNeg(a, b, c) = ((-b) - sqrt(b^2 - 4ac)) / 2a
quadPos(a, b, c) = ((-b) + sqrt(b^2 - 4ac)) / 2
compoundInterest(p, r, t) = (p(1 + (r / 100))^t) - p
```

We can then call the function with -load customFunctions.math and we will se this:

```text
quadNeg(a, b, c) defined
quadPos(a, b, c) defined
compoundInterest(p, r, t) defined
Note that computers can not accurately represent all numbers, results may be approximate at times
$ 
```

**-full** makes the program print out all the steps towards solving the expression

```text
$ compoundInterest(800032 3.18 37)
= ((800032(1 + (3.180000 / 100))^37) - 800032)
= ((800032(1 + (0.031800))^37) - 800032)
= ((800032(1.031800)^37) - 800032)
= ((800032 (3.184450)) - 800032)
= ((2547661.530434) - 800032)
= 1747629.530434
```

**-sn** makes the program print numbers in scientific notation

```text
$ 100
= [1*10^3]
$ cosine -pi
= [-1*10^0]
$ 0.004
= [4*10^-3]
```

Note that we can also change settings in the evaluator by using
```text
$ printFull = 1
printFull set
$ printFull
= 1
$ cos sine 90
= cosine 0.893997
= 0.626301
$ printFull = 0
printFull set
$ printFull
= 0
$ cos sine 90
= 0.626301
$ sciNotation
= 0
$ sciNotation = 1
sciNotation set
$ cos sine 90
= [6.26013*10^-1]
```
