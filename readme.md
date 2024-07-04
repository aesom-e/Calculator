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
= 4.000000
$ 8 * 8
= 64.000000
$ (4)(-3)
= -12.000000
$ 8^4
= 4096.000000
$ 16!
= 20922789888000.000000
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
= 0.602060
$ log 2 10   (Explanation: Log base 2 of 10)
= 3.321928
$ (log 2) 10 (Explanation: (log base 10 of 2) * 10)
= 3.010300
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
$ radToDec 1
= 57.295780
$ decToRad 1
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
= 466.000000
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
= 7625597484987.000000
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

Finally, with functions, you can ask the evaluator to describe them

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

## Command-line arguments

The program can accept raw math expressions from the command line and will return the evaluated result \
The limitation to this is that the expression can not include custom functions

```text
MathEvaluator "(7(4)!)/(sine(4!))"
-185.516800
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
