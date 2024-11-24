#ifndef COLOUR_H
#define COLOUR_H

typedef struct {
    unsigned char r, g, b;
} colour;

// Pack an RGB colour into a single 24-bit value
#define colour_GetRGB(red, green, blue) ((red << 16) + (green << 8) + blue)
// Decode a 24-bit value into a colour struct
#define colour_GetColour(colourValue) ((colour){(colourValue >> 16), (colourValue >> 8 & 0xFF), (colourValue & 0xFF)})

#endif