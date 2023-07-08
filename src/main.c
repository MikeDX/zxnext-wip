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
#include "sprites.h"

extern sprite *ms;
// #define m_zx_border(X) \
//     __asm \
//     ; blah \
//     __endasm \
    // Blah

//        ld a, X \
//        out ($fe), a \
//    __endasm

#define m_zx_border( X ) zx_border(X)
    // __asm ld a, X : out ($fe), a    __endasm 
    



#pragma output CRT_ORG_CODE = 0x6164
#pragma output REGISTER_SP = 0xC000
#pragma output CLIB_MALLOC_HEAP_SIZE = 0
#pragma output CLIB_STDIO_HEAP_SIZE = 0
#pragma output CLIB_FOPEN_MAX = -1
#define HAS_IMAGE
#define HAS_SPRITES
#define HAS_MUSIC


#pragma printf = "%c %s %d"
#define printAt(col, row, str) printf("\x16%c%c%s", (col), (row), (str))

#if defined HAS_MUSIC

extern uint8_t mysong[];

void wrapper() __naked
{
__asm
   INCLUDE "mysong.asm"
__endasm;
}

#endif

#define WAIT_FOR_SCANLINE(line)         while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) == line); \
                                        while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) != line)

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



#define NUM_SPRITES 128


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


#if defined HAS_MUSIC
    unsigned char songnum = 15;//rand() % 10;
#endif

uint8_t tmp8;

#define do_x do_x_asm

void do_x_asm(void) __naked {
__asm
    ; border white
    ld a,  0x07
    out($fe),a

    ; tmp8 = x + vx

    ld hl, (_ms)
    ld a, (hl) ; x
    inc hl
    inc hl
    ld l, (hl) ; vx
    add a, l
    ld (_tmp8), a

    ; if vx > 0
    ld a, l
    and a
   	jp m,dox_i_8	; jump if minus (not more than zero)

    ; if tmp8 > 240
   	ld	hl,(_tmp8)
   	ld	a,240
   	sub	l
   	jr	nc,dox_i_9	; jump if carry not set
    
    ; x = 240
   	ld	hl,(_ms)
   	ld	(hl),240 ; (_ms)+0 = x = 240
    
    ; vx = newspeed(vx)
    ; hl already contains _ms address
   	inc	hl
   	inc	hl
    ld a, (hl)    
    cpl ; invert bits
    inc a ; inc
    ld (hl), a ; vx is now the inverse of vx (-vx)

    ; spriteFlags =  0x08
   	ld	hl,(_ms)
   	ld	bc,4
   	add	hl,bc
   	ld	(hl),8
    ret ; // done
   	// jp	dox_i_11	;EOS
   .dox_i_9
    
    ; else ( tmp8 <= 240 )
    ; x = tmp8;
   	ld  hl, (_ms) ; with the pointer at _ms
   	ld	a,(_tmp8) ; store tmp8 into a
   	ld	(hl),a ; (_ms)+0 = x = tmp8 (ms->x)
   	ret
   .dox_i_8
    
    ; else (vx is negative)
    ; if tmp8 > x 
   	ld	hl,(_ms)
    ld hl, (hl)
    ld  a, l
   	ld	hl,(_tmp8)
    sbc l
   	jr	nc,dox_i_12	;
    
    ; x = 0
   	ld	hl,(_ms)
   	ld	(hl),0

    ; vx = newspeed(vx)
    ; hl already contains _ms address
   	inc	hl
   	inc	hl
    ld a, (hl)    
    dec a
    cpl ; invert bits
    // inc a ; inc
    ld (hl), a ; vx is now the inverse of vx (-vx)


    ; spriteFlags = 0
   	ld	hl,(_ms)
   	ld	bc,4
   	add	hl,bc
   	ld	(hl),0
    ret
   	// jp	i_13	;EOS
   .dox_i_12
    ; x = tmp8
   	ld	hl,(_ms)
   	ld	a,(_tmp8)
   	ld	(hl),a
   .dox_i_11
   	ret
   
   
;   	C_LINE	213,"src/main.c::do_x::0::14"
__endasm
}

void do_x_c(void) {
    __asm
    ld a, INK_WHITE
    out($fe),a
    
    ; tmp8 = x + vx
    __endasm


    tmp8 = ms->x + ms->vx;

    __asm
    ; if vx > 0
    __endasm

    if (ms->vx > 0) { // right

    __asm
    ; if tmp8 > 240
    __endasm
        if (tmp8 > 240) {
            __asm
            ; x = 240
            __endasm

            ms->x = 240;

            __asm
            ; vx = newspeed(vx)
            __endasm

            ms->vx = newspeed(ms->vx);
            __asm
            ; spriteFlags = MIRROR_X_MASK
            __endasm

            ms->spriteFlags = MIRROR_X_MASK;

        } else {
            __asm
            ; else
            ; x = tmp8;
            __endasm

            ms->x = tmp8;
        }
    } else { // left
            __asm
            ; else
            ; if tmp8 > x
            __endasm

        if (tmp8 > ms->x) { // overflow
            __asm
            ; x = 0
            __endasm
            ms->x = 0;
            __asm
            ; vx = newspeed(vx)
            __endasm
            ms->vx = newspeed(ms->vx);
            __asm
            ; spriteFlags = 0
            __endasm

            ms->spriteFlags = 0;
        } else {
            __asm
            ; x = tmp8
            __endasm

            ms->x = tmp8;
        }
    }
}

#define do_y do_y_asm

void do_y_asm(void) __naked {
__asm
    ; border cyan
    ld a,  INK_CYAN
    out($fe),a

    ; tmp8 = y + vx

    ld hl, (_ms)
    inc hl
    ld a, (hl) ; y
    inc hl
    inc hl
    ld l, (hl) ; vx
    add a, l
    ld (_tmp8), a

    ; if vx > 0
    ld a, l
    and a
   	jp m,doy_i_8	; jump if minus (not more than zero)

    ; if tmp8 > 174
   	ld	hl,(_tmp8)
   	ld	a,174
   	sub	l
   	jr	nc,doy_i_9	; jump if carry not set
    
    ; x = 174
   	ld	hl,(_ms)
    inc hl
   	ld	(hl),174 ; (_ms)+0 = y = 174
    
    ; vx = newspeed(vx)
    ; hl already contains _ms address
   	inc	hl
    inc hl
    ld a, (hl)    
    cpl ; invert bits
    inc a ; inc
    ld (hl), a ; vx is now the inverse of vx (-vx)

    ret ; // done
   	// jp	doy_i_11	;EOS
   .doy_i_9
    
    ; else ( tmp8 <= 192 )
    ; y = tmp8;
   	ld  hl, (_ms) ; with the pointer at _ms
    inc hl
   	ld	a,(_tmp8) ; store tmp8 into a
   	ld	(hl),a ; (_ms)+0 = x = tmp8 (ms->x)
   	ret
   .doy_i_8
    
    ; else (vx is negative)
    ; if tmp8 > y
    ld a, 192
   	ld	hl,(_tmp8)
    sbc l
   	jr	nc,doy_i_12	;
    
    ; y = 0
   	ld	hl,(_ms)
    inc hl
   	ld	(hl),0

    ; vx = newspeed(vx)
    ; hl already contains _ms address + 1
   	inc	hl
    inc hl
    ld a, (hl)    
    dec a
    cpl ; invert bits
    // inc a ; inc
    ld (hl), a ; vx is now the inverse of vx (-vx)

    ret
   	// jp	i_13	;EOS
   .doy_i_12
    ; x = tmp8
   	ld	hl,(_ms)
    inc hl
   	ld	a,(_tmp8)
   	ld	(hl),a
   .doy_i_11
   	ret
   
   
;   	C_LINE	213,"src/main.c::do_x::0::14"
__endasm
}
void do_y_c(void) {
    __asm
    ld a, INK_GREEN
    out($fe),a
    __endasm
    
    tmp8 = ms->y + ms->vy;
    
    if (ms->vy > 0) {
        __asm
        ; Going down
        __endasm

        if (tmp8 > 174) {
            ms->y = 174;
            ms->vy = newspeed(ms->vy);
        } else {
            ms->y = tmp8;
        }
    } else  {
        __asm
        ; Going up
        __endasm

        if (tmp8 > 192) {
            ms->y = 0;
            ms->vy = newspeed(ms->vy);
        } else {
            ms->y = tmp8;
        }
    }
}

uint8_t i = 0;


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
        ;    // for (uint8_t i = 0; i < NUM_SPRITES; i++) {
    __endasm
    
    for (i = 0; i < NUM_SPRITES; i++) {

        ms = &sprites[i];
        __asm 
            ; sprites[i].x = i;
        __endasm;
        ms->x = i ;

        __asm 
            ; ms->y = i*2;
        __endasm;

        ms->y = i % 174 ;

        __asm 
            ; int8_t x = -1 + rand() % 2;
        __endasm;

        int8_t x = -1 + rand() % 2;
        int8_t y = -1 + rand() % 2;
        ms->vx = 0;
        ms->vy = 0;
        while(ms->vx ==0 ) 
            ms->vx = (rand() %5 ) * ((x != 0) ? x : 1);
        while(ms->vy ==0 )
            ms->vy = (rand() %5 ) * ((y != 0) ? y : 1);

        ms->spriteFlags = 0;
        ms->spritePattern = 0;
    }
#endif

    // layer2_clear_screen(0xFF, NULL);

    while(1) {
        __asm 
            ld a, INK_WHITE
            out ( $fe ), a
        __endasm
        // intrinsic_halt();
        // if(1) {
            // for (int i = 0; i < 2; i++) {
                // layer2_draw_pixel_fast(rand() % 256, rand() % 192, rand() % 256, NULL);
                // layer2_draw_pixel(rand() % 256, rand() % 192, rand() % 256, NULL);
                // m_zx_border(INK_WHITE);
                // layer2_draw_line(rand() % 256, rand() % 192, rand() % 256, rand() % 192, rand() % 256, NULL);
                // layer2_draw_rect(rand() % 256, rand() % 192, 40, 20, rand() % 256, NULL);
                // layer2_fill_rect(rand() % 256, rand() % 192, 40, 20, rand() % 256, NULL);
                //  layer2_draw_text(rand() % 24, rand() % 32, "Hello", rand() % 256, NULL);
            // }
            // layer2_blit_transparent(rand() % 256, rand() % 192, sprite, 16, 16, NULL);
        // }

#if defined HAS_MUSIC
        __asm 
        ld a, INK_BLUE
        out($fe), a
        __endasm
        intrinsic_di();
        ply_akg_play();
        intrinsic_ei();
#endif

#if defined HAS_IMAGE
        __asm 
        ld a, INK_CYAN
        out($fe), a
        __endasm

        layer2_set_offset_x(255-sprites[0].x);
        layer2_set_offset_y(192-sprites[0].y);

#endif

#if defined HAS_MUSIC
        __asm 
        ld a, INK_RED
        out($fe), a
        __endasm

        // in_Inkey();
        if (in_inkey()) {
            ply_akg_stop();
            in_wait_nokey();
            // songnum++;
            if (songnum > 14) songnum = 0;
            zx_cls(INK_BLACK | PAPER_WHITE);
            printAt(3,  9, "Press any key for next song");

            char s = songnum;
            ply_akg_init(mysong, s);

            printf("\x16%c%c%s %d", 3, 7, "Playing song", songnum);
        }
#endif

#if defined HAS_SPRITES

        __asm 

        ; next sprite part

        __endasm

        set_sprite_attrib_slot(0);

        // __asm 
        //     ld a, INK_YELLOW
        //     out($fe), a
        // __endasm

        for (i = 0; i < NUM_SPRITES; i++) {
            __asm
                ; // b = i %2
                ld a, INK_YELLOW
                out($fe), a
            __endasm
    
            ms = &sprites[i];

            __asm 
                ; Timer check to animate
            __endasm
            if (timer == 0) {
                ms->spritePattern ++;
                ms->spritePattern &=1;
            }

            do_x();
            do_y();

            __asm
            ; Update attributes
                ld a, INK_RED
                out($fe), a
            __endasm

            // set_sprite_attrs_mike();
        }
        __asm 
        ;         out sprite vars
        __endasm

        
        // m_zx_border(INK_YELLOW);
        __asm 
            ld a, INK_BLACK
            out($fe), a
        __endasm

        __asm
        ld hl, _timer;
        inc (hl);
        ld hl, (_timer);
        ld a,15
        and l
        ld l,a
        ld (_timer), hl
        __endasm;

#endif

    __asm 
    ld a, INK_BLUE
    out($fe), a
    
        ; WAIT_FOR_SCANLINE_192
    .start_wait_for_scanline_192:
        ld	hl,$1F	; const for 31
    .start_wait_for_scanline_192_loop
        ld	bc,0x243b
        out	(c),l ; write 31 to 0x243b
        inc	b 
        in	a,(c) ; read scanline from 0x253b
        cp	192 ; is it 192?
        jp	nz,	start_wait_for_scanline_192_loop ; repeat if not
    __endasm

        // Do the sprite update after the screen update

        // if(0) {
        __asm
            ld a, INK_YELLOW
            out($fe), a

            ; update next regs for sprite update
        __endasm
        
        set_sprite_pattern_slot(0);

        ms = sprites;

        __asm
        ; i = 0
        and a
        ld (_i),a
        __endasm
        // i = 0;
        __asm 
        ; while i < NUM_SPRITES
        __endasm

        __asm
        .startupdate


            ld a, (_i)
            sub 128
            jp nc, endupdate

            // ld a, INK_RED
            // out($fe), a

            call _set_sprite_attrs_mike

            ; border blaack
            // ld a, INK_BLACK
            // out($fe), a

            ; inc _i
            ld a, (_i)
            inc a
            ld (_i), a

            ; inc ms (10 is sizeof sprite)
            ld hl, (_ms)
            ld bc, 10
            add hl, bc
            ld (_ms), hl

            jr startupdate

        .endupdate
        __endasm
        // while (i < NUM_SPRITES) {

        //     // ms = &sprites[i];
        //     set_sprite_attrs_mike();
        //     __asm
        //     ld a, INK_BLACK
        //     out ($fe), a
        //     ; inc i
        //     __endasm
        //     i++;
        //     __asm
        //     ; inc ms
        //     __endasm
        //     ms++;
        //     // ms+=sizeof(sprite);
        //     __asm
        //     ; while loop end
        //     __endasm
        // }
        // }

        __asm 
            ; end loop
            ; BORDER GREEN
            ld a, INK_GREEN
            out($fe), a
        __endasm

    }

    return 0;
    
}