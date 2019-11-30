#ifndef ARCH_I386_VGA_H
#define ARCH_I386_VGA_H

#include <stdint.h>

typedef enum VGAColour {
    VGABlack,
    VGABlue,
    VGAGreen,
    VGACyan,
    VGARed,
    VGAMagenta,
    VGABrown,
    VGALightGrey,
    VGADarkGrey,
    VGALightBlue,
    VGALightGreen,
    VGALightCyan,
    VGALightRed,
    VGALightMagenta,
    VGAYellow,
    VGAWhite
} VGAColour;

static inline uint8_t vgaCharacterColour(VGAColour fg, VGAColour bg) {
    return bg << 4 | fg;
}

static inline uint16_t vgaCharacter(unsigned char c, uint8_t colour) {
    return (uint16_t) colour << 8 | (uint16_t) c;
}

#endif
