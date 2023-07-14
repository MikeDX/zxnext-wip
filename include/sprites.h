
#include <arch/zxn.h>
#include <input.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include "zxnext_layer2.h"

/* Max sprites per scanline limit reached. */
#define MAX_SPRITES_PER_SCANLINE_MASK 0x02

/* A collision between two or more sprites. */
#define SPRITE_COLLISION_MASK 0x01

/* Mirror the sprite x-wise. */
#define MIRROR_X_MASK 0x08

/* Mirror the sprite y-wise. */
#define MIRROR_Y_MASK 0x04

/* Rotate the sprite 90 degrees. */
#define ROTATE_MASK 0x02

#define SCALE_1x_XMASK 0x00
#define SCALE_2x_XMASK 0x08
#define SCALE_4x_XMASK 0x10
#define SCALE_8x_XMASK 0x18
#define SCALE_1x_YMASK 0x00
#define SCALE_2x_YMASK 0x02
#define SCALE_4x_YMASK 0x04
#define SCALE_8x_YMASK 0x06

#define SPRITE_PATTERN_MASK     0x3F
#define SPRITE_ATTRIB_MASK      0x7F
#define X_LSB_MASK           0x00FF
#define X_MSB_MASK           0x0100
#define X_MSB_SHIFT          8
#define PALETTE_OFFSET_SHIFT 4
#define SPRITE_VISIBLE_MASK  0x80
#define SPRITE_ENABLE_ATTRIB_4 0x40
#define SPRITE_REL_UNIFIED_MASK     0x20
#define X_EXT(x) (((uint16_t) (x)) + 32)
#define X_LSB(x_ext) (uint8_t) ((x_ext) & X_LSB_MASK)
#define X_MSB(x_ext) (uint8_t) (((x_ext) & X_MSB_MASK) >> X_MSB_SHIFT)
#define Y_EXT(y) ((y) + 32)
#define LAYER_PRIORITIES_MASK  0x07
#define LAYER_PRIORITIES_SHIFT 2

typedef struct {
    uint8_t x, y;
    int8_t vx, vy;
    uint8_t spriteFlags;
    uint8_t spritePattern;
    int8_t f1;
    int8_t f2;
    uint8_t f3;
    uint8_t f4;    
} sprite;


void load_sprite_patterns(const char *filename,
                          const void *sprite_pattern_buf,
                          uint8_t num_sprite_patterns,
                          uint8_t start_sprite_pattern_slot);


void set_sprite_pattern_slot(uint8_t sprite_slot);

void load_sprite_palette(const char *filename, const void *sprite_palette_buf);

void set_sprite_layers_system(bool sprites_visible,
                              bool sprites_on_border,
                              uint8_t layer_priorities,
                              bool lores_mode);


void set_sprite_attrib_slot(uint8_t sprite_slot);


void set_sprite_attrs_mike();

