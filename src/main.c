#define ARKOS_USE_ROM_PLAYER 1

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
#include <psg/arkos.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include <psg/arkos.h>

#include "zxnext_layer2.h"


#define m_zx_border( X ) zx_border(X)
    // __asm ld a, X : out ($fe), a    __endasm 
    



#pragma output CRT_ORG_CODE = 0x6164
#pragma output REGISTER_SP = 0xC000
#pragma output CLIB_MALLOC_HEAP_SIZE = 0
#pragma output CLIB_STDIO_HEAP_SIZE = 0
#pragma output CLIB_FOPEN_MAX = -1
#pragma printf = "%c %s"

#define HAS_IMAGE
#define HAS_SPRITES
#define HAS_MUSIC


#pragma printf = "%c %s %d"
#define printAt(col, row, str) printf("\x16%c%c%s", (col), (row), (str))

// #include "zxnext_layer2.h"

// #include "zxnext_layer2.h"


#if defined HAS_MUSIC

extern uint8_t mysong[];

void wrapper() __naked
{
__asm
   INCLUDE "mysong.asm"
__endasm;
}

#endif


/* Sprites over layer 2 screen over ULA screen (default). */
#define LAYER_PRIORITIES_S_L_U 0x0

/* Layer 2 screen over sprites over ULA screen. */
#define LAYER_PRIORITIES_L_S_U 0x1

/* Sprites over ULA screen over layer 2 screen. */
#define LAYER_PRIORITIES_S_U_L 0x2

/* Layer 2 screen over ULA screen over sprites. */
#define LAYER_PRIORITIES_L_U_S 0x3

/* ULA screen over sprites over layer 2 screen. */
#define LAYER_PRIORITIES_U_S_L 0x4

/* ULA screen over layer 2 screen over sprites. */
#define LAYER_PRIORITIES_U_L_S 0x5

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

#define WAIT_FOR_SCANLINE(line)         while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) == line); \
                                        while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) != line)


typedef struct {
    uint8_t x, y;
    int8_t vx, vy;
    uint8_t spriteFlags;
    uint8_t spritePattern;
} sprite;


static void init_hardware(void)
{
    // Put Z80 in 28 MHz turbo mode.
    ZXN_NEXTREG(REG_TURBO_MODE, 0x03);

    // Disable RAM memory contention.
    ZXN_NEXTREGA(REG_PERIPHERAL_3, ZXN_READ_REG(REG_PERIPHERAL_3) | RP3_DISABLE_CONTENTION);

    layer2_set_main_screen_ram_bank(8);
    layer2_set_shadow_screen_ram_bank(11);
}

static void init_isr(void)
{
    // Set up IM2 interrupt service routine:
    // Put Z80 in IM2 mode with a 257-byte interrupt vector table located
    // at 0x6000 (before CRT_ORG_CODE) filled with 0x61 bytes. Install an
    // empty interrupt service routine at the interrupt service routine
    // entry at address 0x6161.

    intrinsic_di();
    im2_init((void *) 0x6000);
    memset((void *) 0x6000, 0x61, 257);
    z80_bpoke(0x6161, 0xFB);
    z80_bpoke(0x6162, 0xED);
    z80_bpoke(0x6163, 0x4D);
    intrinsic_ei();
}

#ifdef HAS_SPRITES
void set_sprite_layers_system(bool sprites_visible,
                              bool sprites_on_border,
                              uint8_t layer_priorities,
                              bool lores_mode)
{
    uint8_t value = (layer_priorities & LAYER_PRIORITIES_MASK) << LAYER_PRIORITIES_SHIFT;

    if (sprites_visible)
    {
        value = value | RSLS_SPRITES_VISIBLE;
    }

    if (sprites_on_border)
    {
        value = value | RSLS_SPRITES_OVER_BORDER;
    }

    if (lores_mode)
    {
        value = value | RSLS_ENABLE_LORES;
    }

    IO_NEXTREG_REG = REG_SPRITE_LAYER_SYSTEM;
    IO_NEXTREG_DAT = value;
}



void set_sprite_attrs_mike(sprite *msprite) {
    /*
    set_sprite_attributes_ext(
        sprites[i].spritePattern, 
        sprites[i].x, 
        sprites[i].y, 
        0, // palette offset
        sprites[i].spriteFlags, 
        0, // scale
        1 // visible
        );
    */

    uint16_t xe = X_EXT(msprite->x);

    // IO_SPRITE_ATTRIBUTE = X_LSB(xe);
    // IO_SPRITE_ATTRIBUTE = Y_EXT(msprite->y);
    // IO_SPRITE_ATTRIBUTE = X_MSB(xe) | msprite->spriteFlags;
    // IO_SPRITE_ATTRIBUTE = msprite->spritePattern; //pattern_slot;


    // uint16_t xe = msprite->x;

    __asm 
    ;     IO_SPRITE_ATTRIBUTE = X_LSB(xe);
    __endasm

    IO_SPRITE_ATTRIBUTE = X_LSB(xe);

    __asm 
    ;         IO_SPRITE_ATTRIBUTE = Y_EXT(msprite->y);

    __endasm
    
    IO_SPRITE_ATTRIBUTE = Y_EXT(msprite->y);
    
    uint8_t xmsb = (xe>255) ? 1 : 0;

    __asm 
    ;   IO_SPRITE_ATTRIBUTE = (0 << PALETTE_OFFSET_SHIFT) | X_MSB(xe) | msprite->spriteFlags;
   
    __endasm
    
    // IO_SPRITE_ATTRIBUTE = (0 << PALETTE_OFFSET_SHIFT) | X_MSB(xe) | msprite->spriteFlags;
    IO_SPRITE_ATTRIBUTE = xmsb + msprite->spriteFlags;
    
    __asm 
    ;      IO_SPRITE_ATTRIBUTE = msprite->spritePattern | SPRITE_VISIBLE_MASK;

    __endasm
    
    IO_SPRITE_ATTRIBUTE = msprite->spritePattern + SPRITE_VISIBLE_MASK;

/*
    if (scale_flags) {
        IO_SPRITE_ATTRIBUTE = pattern_slot | SPRITE_ENABLE_ATTRIB_4;
        IO_SPRITE_ATTRIBUTE = scale_flags;
    } else {
        IO_SPRITE_ATTRIBUTE = pattern_slot;
    }
*/
}

__asm 
; Function set_sprite_attrs_mike_ flags 0x00000200 __smallc
; void set_sprite_attrs_mike_(struct 0__anonstruct_125_10 sprite* msprite)
; parameter 'struct 0__anonstruct_125_10 sprite* msprite' at sp+2 size(2)
;                        	C_LINE	191,"src/main.c::set_sprite_attrs_mike_::0::9"
; src/main.c::set_sprite_attrs_mike_::0::9:
   ._set_sprite_attrs_mike_
   	pop	bc
   	pop	hl
   	push	hl
   	push	bc
   	ld	l,(hl)
   	ld	h,0
   	ld	bc,32
   	add	hl,bc
   	push	hl
       ;     IO_SPRITE_ATTRIBUTE =  (uint8_t) (( xe ) &  0x00FF ) ;
   	// pop	hl
   	// push	hl
   	// ld	h,0
   	ld	a,l
   	out	(_IO_SPRITE_ATTRIBUTE),a
       ;         IO_SPRITE_ATTRIBUTE =  (( msprite->y ) + 32) ;
   	call	l_gint4sp	;
   	inc	hl
   	ld	l,(hl)
   	ld	h,0
   	ld	bc,32
   	add	hl,bc
   	ld	a,l
   	out	(_IO_SPRITE_ATTRIBUTE),a
       ;   IO_SPRITE_ATTRIBUTE = (0 <<  4 ) |  (uint8_t) ((( xe ) &  0x0100 ) >>  8 )  | msprite->spriteFlags;
   	pop	hl
   	push	hl
   	ld	a,h
   	and	1
   	ld	h,a
   	ld	l,h
   	ld	h,0
   	push	hl
   	call	l_gint6sp	;
   	ld	bc,4
   	add	hl,bc
   	ld	l,(hl)
   	pop	de
   	ld	h,d
   	ld	a,l
   	or	e
   	ld	l,a
   	out	(_IO_SPRITE_ATTRIBUTE),a
       ;      IO_SPRITE_ATTRIBUTE = msprite->spritePattern |  0x80 ;
   	call	l_gint4sp	;
   	ld	bc,5
   	add	hl,bc
   	ld	l,(hl)
   	ld	h,0
   	set	7,l
   	ld	a,l
   	out	(_IO_SPRITE_ATTRIBUTE),a
   	pop	bc
   	ret
   __endasm

void set_sprite_attributes_ext(uint8_t sprite_pattern_slot,
                               uint8_t x,
                               uint8_t y,
                               uint8_t palette_offset,
                               uint8_t sprite_flags,
                               uint8_t scale_flags,
                               bool visible)
{
    uint8_t pattern_slot = sprite_pattern_slot & SPRITE_PATTERN_MASK;

    if (visible)
    {
        pattern_slot = pattern_slot | SPRITE_VISIBLE_MASK;
    }

    IO_SPRITE_ATTRIBUTE = X_LSB(X_EXT(x));
    IO_SPRITE_ATTRIBUTE = Y_EXT(y);
    IO_SPRITE_ATTRIBUTE = (palette_offset << PALETTE_OFFSET_SHIFT) | X_MSB(X_EXT(x)) | sprite_flags;

    if (scale_flags) {
        IO_SPRITE_ATTRIBUTE = pattern_slot | SPRITE_ENABLE_ATTRIB_4;
        IO_SPRITE_ATTRIBUTE = scale_flags;
    } else {
        IO_SPRITE_ATTRIBUTE = pattern_slot;
    }
}

void set_sprite_attributes(uint8_t sprite_pattern_slot,
                           uint16_t x,
                           uint8_t y,
                           uint8_t palette_offset,
                           uint8_t sprite_flags,
                           bool visible)
{
    uint8_t pattern_slot = sprite_pattern_slot & SPRITE_PATTERN_MASK;

    if (visible)
    {
        pattern_slot = pattern_slot | SPRITE_VISIBLE_MASK;
    }

    IO_SPRITE_ATTRIBUTE = X_LSB(x);
    IO_SPRITE_ATTRIBUTE = y;
    IO_SPRITE_ATTRIBUTE = (palette_offset << PALETTE_OFFSET_SHIFT) | X_MSB(x) | sprite_flags;
    IO_SPRITE_ATTRIBUTE = pattern_slot;
}

void set_sprite_attributes_ext_anchor(uint8_t sprite_pattern_slot,
                               uint8_t x,
                               uint8_t y,
                               uint8_t palette_offset,
                               uint8_t sprite_flags,
                               bool visible,
                               bool rel_is_unified)
{
    uint8_t pattern_slot = sprite_pattern_slot & SPRITE_PATTERN_MASK;

    if (visible)
    {
        pattern_slot = pattern_slot | SPRITE_VISIBLE_MASK;
    }

    IO_SPRITE_ATTRIBUTE = X_LSB(X_EXT(x));
    IO_SPRITE_ATTRIBUTE = Y_EXT(y);
    IO_SPRITE_ATTRIBUTE = (palette_offset << PALETTE_OFFSET_SHIFT) | X_MSB(X_EXT(x)) | sprite_flags;
    IO_SPRITE_ATTRIBUTE = pattern_slot | SPRITE_ENABLE_ATTRIB_4;

    // TODO: Implement magnification
    if (rel_is_unified)
        IO_SPRITE_ATTRIBUTE = SPRITE_REL_UNIFIED_MASK;
    else
        IO_SPRITE_ATTRIBUTE = 0;
}

// TODO: Implement magnification for relative composite sprites
void set_sprite_attributes_ext_relative(uint8_t sprite_pattern_slot,
                               int8_t x,
                               int8_t y,
                               uint8_t palette_offset,
                               uint8_t sprite_flags,
                               bool visible,
                               bool pattern_is_relative_to_anchor)
{
    uint8_t pattern_slot = sprite_pattern_slot & SPRITE_PATTERN_MASK;

    if (visible)
    {
        pattern_slot = pattern_slot | SPRITE_VISIBLE_MASK;
    }

    IO_SPRITE_ATTRIBUTE = x;
    IO_SPRITE_ATTRIBUTE = y;
    IO_SPRITE_ATTRIBUTE = (palette_offset << PALETTE_OFFSET_SHIFT) | X_MSB(X_EXT(x)) | sprite_flags;
    IO_SPRITE_ATTRIBUTE = pattern_slot | SPRITE_ENABLE_ATTRIB_4;

    if (pattern_is_relative_to_anchor)
        IO_SPRITE_ATTRIBUTE = 0x41;
    else
        IO_SPRITE_ATTRIBUTE = 0x40;    
}

void set_sprite_pattern(const void *sprite_pattern)
{
    intrinsic_outi((void *) sprite_pattern, __IO_SPRITE_PATTERN, 256);
}

void set_sprite_pattern_slot(uint8_t sprite_slot)
{
    IO_SPRITE_SLOT = sprite_slot & SPRITE_PATTERN_MASK;
}

void set_sprite_attrib_slot(uint8_t sprite_slot)
{
    IO_SPRITE_SLOT = sprite_slot & SPRITE_ATTRIB_MASK;
}

void set_sprite_palette(const uint16_t *colors, uint16_t length, uint8_t palette_index)
{
    uint8_t *color_bytes = (uint8_t *) colors;
    uint16_t i;

    if ((colors == NULL) || (length == 0))
    {
        return;
    }

    if (palette_index + length > 256)
    {
        length = 256 - palette_index;
    }

    IO_NEXTREG_REG = REG_PALETTE_INDEX;
    IO_NEXTREG_DAT = palette_index;

    IO_NEXTREG_REG = REG_PALETTE_VALUE_16;
    for (i = 0; i < (length << 1); i++)
    {
        IO_NEXTREG_DAT = color_bytes[i];
    }
}

void load_sprite_patterns(const char *filename,
                          const void *sprite_pattern_buf,
                          uint8_t num_sprite_patterns,
                          uint8_t start_sprite_pattern_slot)
{
    uint8_t filehandle;

    if ((filename == NULL) || (sprite_pattern_buf == NULL) ||
        (num_sprite_patterns == 0) || (start_sprite_pattern_slot > 63))
    {
        return;
    }

    if (start_sprite_pattern_slot + num_sprite_patterns > 64)
    {
        num_sprite_patterns = 64 - start_sprite_pattern_slot;
    }

    errno = 0;
    filehandle = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if (errno)
    {
        return;
    }

    set_sprite_pattern_slot(start_sprite_pattern_slot);

    while (num_sprite_patterns--)
    {
        esxdos_f_read(filehandle, (void *) sprite_pattern_buf, 256);
        if (errno)
        {
            break;
        }
        set_sprite_pattern(sprite_pattern_buf);
    }

    esxdos_f_close(filehandle);
}

void load_sprite_palette(const char *filename, const void *sprite_palette_buf)
{
    uint8_t filehandle;

    IO_NEXTREG_REG = 0x43;
    IO_NEXTREG_DAT = 0x20;  // select sprite 0 palette for read/write

    if ((filename == NULL) || (sprite_palette_buf == NULL))
    {
        return;
    }

    errno = 0;
    filehandle = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if (errno)
    {
        return;
    }

    esxdos_f_read(filehandle, (void *) sprite_palette_buf, 256);
    if (errno)
    {
        goto end;
    }
    set_sprite_palette((uint16_t *) sprite_palette_buf, 128, 0);

    esxdos_f_read(filehandle, (void *) sprite_palette_buf, 256);
    if (errno)
    {
        goto end;
    }
    set_sprite_palette((uint16_t *) sprite_palette_buf, 128, 128);

end:
    esxdos_f_close(filehandle);
}


#define NUM_SPRITES 64


uint8_t timer = 0;
sprite sprites[NUM_SPRITES];

#endif

static void init_tests(void)
{
    m_zx_border(INK_YELLOW);
    zx_cls(INK_BLACK | PAPER_WHITE);
    layer2_configure(true, false, false, 0);
}

int8_t newspeed(int8_t speed) {
    // return -speed;
    int8_t x = speed > 0 ? -1 : 1;

    uint8_t ns = 0;
    while(ns ==0 ) 
        ns = (rand() %10 );

    ns = ns * x;

    return ns;
}



int main(void)
{
    uint8_t sprBuf[256];
    bool increment = true;
    uint8_t offset_x = 0;
    uint8_t offset_y = 0;
    bool increment_x = true;
    bool increment_y = true;

    // ZXN_NEXTREG(REG_TURBO_MODE, RTM_28MHZ);
#if defined HAS_MUSIC
    ply_akg_init(mysong, 0);
#endif

#if defined HAS_IMAGE
    init_hardware();
    init_isr();
    init_tests();
    layer2_load_screen("screen1.nxi", NULL, 7, false);
#endif

    // Endless loop
#if defined HAS_MUSIC
    unsigned char songnum = 15;//rand() % 10;
#endif

#if defined HAS_SPRITES 

    __asm 
        ; load_sprite_patterns("all.spr", sprBuf, 37, 0);
    __endasm;
    load_sprite_patterns("all.spr", sprBuf, 37, 0);

    __asm 
        ; set_sprite_pattern_slot(0);
    __endasm
    set_sprite_pattern_slot(0);

    load_sprite_palette("all.nxp", sprBuf);
    set_sprite_layers_system(true, true, LAYER_PRIORITIES_S_L_U, false);

    __asm 
    ;         set_sprite_attrib_slot(0);
    __endasm
    set_sprite_attrib_slot(0);

    __asm 
        ;     for (uint8_t i = 0; i < NUM_SPRITES; i++) {
    __endasm
    



    for (uint8_t i = 0; i < NUM_SPRITES; i++) {

        __asm 
            ; sprites[i].x = i;
        __endasm;
        sprites[i].x = i ;

        __asm 
            ; sprites[i].y = i*2;
        __endasm;

        sprites[i].y = i % 174 ;

        __asm 
            ; int8_t x = -1 + rand() % 2;
        __endasm;

        int8_t x = -1 + rand() % 2;
        int8_t y = -1 + rand() % 2;
        sprites[i].vx = 0;
        sprites[i].vy = 0;
        while(sprites[i].vx ==0 ) 
            sprites[i].vx = (rand() %5 ) * ((x != 0) ? x : 1);
        while(sprites[i].vy ==0 )
            sprites[i].vy = (rand() %5 ) * ((y != 0) ? y : 1);


        sprites[i].spriteFlags = 0;
        sprites[i].spritePattern = 0;
    }
#endif

    // layer2_clear_screen(0xFF, NULL);

    while(1) {
        // intrinsic_halt();
        // if(1) {
            // for (int i = 0; i < NUM_SPRITES; i++) {
                layer2_draw_pixel(rand() % 256, rand() % 192, rand() % 256, NULL);
                // m_zx_border(INK_WHITE);
                // layer2_draw_line(rand() % 256, rand() % 192, rand() % 256, rand() % 192, rand() % 256, NULL);
                // layer2_draw_rect(rand() % 256, rand() % 192, 40, 20, rand() % 256, NULL);
                // layer2_fill_rect(rand() % 256, rand() % 192, 40, 20, rand() % 256, NULL);
                // layer2_draw_text(rand() % 24, rand() % 32, "Hello", rand() % 256, NULL);

            // }
            // layer2_blit_transparent(rand() % 256, rand() % 192, sprite, 16, 16, NULL);
        // }

        // __asm
        // ld a, INK_RED
        // out($fe), a
        // __endasm
        // m_zx_border(INK_RED);


#if defined HAS_MUSIC
__asm 
ld a, INK_BLUE
out($fe), a
__endasm
            intrinsic_di();
            ply_akg_play();
            intrinsic_ei();
#endif

        // m_zx_border(INK_CYAN);

#if defined HAS_IMAGE
__asm 
ld a, INK_CYAN
out($fe), a
__endasm

/*        if (increment_x)
        {
            offset_x++;
        }
        else
        {
            offset_x--;
        }

        if (increment_y)
        {
            offset_y = INC_Y(offset_y);
        }
        else
        {
            offset_y = DEC_Y(offset_y);
        }
*/
        layer2_set_offset_x(255-sprites[0].x);
        layer2_set_offset_y(192-sprites[0].y);
/*
        if (offset_x == 0)
        {
            increment_x = !increment_x;
        }

        if (offset_y == 0)
        {
            increment_y = !increment_y;
        }
        */

#endif

#if defined HAS_MUSIC
__asm 
ld a, INK_RED
out($fe), a
__endasm

            // in_Inkey();
            if (in_inkey())
            {
                ply_akg_stop();
                in_wait_nokey();
                // songnum++;
                if (songnum > 14) songnum = 0;
                zx_cls(INK_BLACK | PAPER_WHITE);
                printAt(3,  9, "Press any key for next song");

                char s = songnum;
                ply_akg_init(mysong, s);

                // char num[2];
                // itoa(songnum, num, 10);

                // char str[20];
                // strcpy(
                //     str,
                //     "Playing song:"
                // );
                // strcat(str, num);
                // sprintf(str, "Blah [%d]", n);
                m_zx_border(songnum);
                printf("\x16%c%c%s %d", 3, 7, "Playing song", songnum);
                // printAt(3,  7, str);
            }
            // m_zx_border(INK_BLUE);
#endif

#ifdef BORDER_TEST
            __asm
            ; has_sprites
            __endasm

    uint8_t b = 1;

    __asm 
    ; black yellow border 

    
    ld b, NUM_SPRITES

    bl1:
        ld a, b
        and 1
        cp 0
        jp z, bl_else

        // ld a, INK_YELLOW
        out($fe),a 

        jp bl_final

    bl_else:
        out ($fe),a 

    bl_final:
        djnz bl1 

    xor a

    __endasm



    __asm 

    ; c loop for black yellow border

    __endasm
    WAIT_FOR_SCANLINE(12);

    for (uint8_t i = 0; i < NUM_SPRITES; i++) {
        if (b==0) {
            zx_border(INK_BLACK);
        } else {
            zx_border(INK_YELLOW);
        }
        b = 1-b;
    }
#endif

    // WAIT_FOR_SCANLINE(192);

    // }
    // while (1) {
#if defined HAS_SPRITES

    __asm 

    ; next sprite part

    __endasm

    for (int i = 0; i < NUM_SPRITES; i++) {
                __asm
                ;                 b = i %2
                __endasm
            // uint8_t b = i % 2;

            __asm 
                ; m_zx_border(b);
                // pop hl
                // push hl
                // ld a, l
                // and 1
                // out ($fe),a
            __endasm
                // m_zx_border(b);

            if (timer %16 == 0) {
                sprites[i].spritePattern++;
                sprites[i].spritePattern = sprites[i].spritePattern & 1;
                // sprites[i].spritePattern = ++sprites[i].spritePattern  & 1;
            }
/*
            if (sprites[i].vx > 0) {
                // going right
                __asm 
                nop
                __endasm

            } else {

                // going left
                __asm 
                    nop
                __endasm
                
            }

            if (sprites[i].vy > 0) {
                // going down
                __asm 
                    nop
                __endasm

            } else {
                // going up
                __asm 
                    nop
                __endasm

            }
            */
            // sprites[i].x += sprites[i].vx;
            // sprites[i].y += sprites[i].vy;

            // if sprites[i].x 
        
            uint8_t newx = sprites[i].x + sprites[i].vx;
            if (sprites[i].vx > 0) {
                if (newx > 240) {
                    sprites[i].x = 240;
                    sprites[i].vx = newspeed(sprites[i].vx);
                    sprites[i].spriteFlags = MIRROR_X_MASK;
                } else {
                    sprites[i].x = newx;
                }
            } else if (sprites[i].vx < 0) {
                if (newx > sprites[i].x) {
                    sprites[i].x = 0;
                    sprites[i].vx = newspeed(sprites[i].vx);
                    sprites[i].spriteFlags = 0;
                } else {
                    sprites[i].x = newx;
                }
            }
            
            uint8_t newy = sprites[i].y + sprites[i].vy;
            if (sprites[i].vy > 0) {
                if (newy > 174) {
                    sprites[i].y = 174;
                    sprites[i].vy = newspeed(sprites[i].vy);
                } else {
                    sprites[i].y = newy;
                }
            } else if (sprites[i].vy < 0) {
                if (newy > 192) {
                    sprites[i].y = 0;
                    sprites[i].vy = newspeed(sprites[i].vy);
                } else {
                    sprites[i].y = newy;
                }
            }
        __asm 

        ;         for (uint8_t i = 0; i < NUM_SPRITES; i++) {
            ld a, INK_YELLOW
            out($fe), a
        __endasm

            set_sprite_attrs_mike(&sprites[i]);

        __asm 
            ld a, INK_RED
            out($fe), a
        __endasm

        }
        __asm 
        ;         set_sprite_attrib_slot(0);
        __endasm

        set_sprite_attrib_slot(0);


        __asm 
            ;         for (uint8_t i = 0; i < NUM_SPRITES; i++) {
            ld a, INK_BLACK
            out($fe), a
        __endasm

        if(0) {
        // m_zx_border(INK_RED);
        for (uint8_t i = 0; i < NUM_SPRITES; i++) {
            __asm 
                ; m_zx_border(i % 8);
            __endasm
            // m_zx_border(i % 2) + 2;

            __asm 

               ; set_sprite_attributes_ext(sprites[i].spritePattern, sprites[i].x, sprites[i].y, 0, sprites[i].spriteFlags, 0, 1);

            __endasm

            // set_sprite_attrs_mike(&sprites[i]);
                                    // SLOT,                     X,           Y,          , PAL, FLAGS, SCALE, VISIBLE);

            // set_sprite_attributes_ext(sprites[i].spritePattern, sprites[i].x, sprites[i].y, 0, sprites[i].spriteFlags, 0, 1);
        }
        }
        
        // m_zx_border(INK_YELLOW);
        __asm
        ld hl, _timer;
        inc (hl);
        __endasm;
        // timer++;  
        // intrinsic_halt();


#endif

        WAIT_FOR_SCANLINE(192);

        __asm 
        ld a, INK_GREEN
        out($fe), a
        __endasm

        // m_zx_border(INK_RED);
        // WAIT_FOR_SCANLINE(194);
        // m_zx_border(INK_BLACK);


    }

    return 0;
    
}