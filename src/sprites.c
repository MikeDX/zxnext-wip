#include "sprites.h"


void intrinsic_outi_port_0x5b(void);
void intrinsic_outi_num_256(void);

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

sprite *ms = NULL;

void set_sprite_attrs_mike() {
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

   __asm
   ; x+=32
__endasm
    // uint8_t x = ms->x + 32;

    __asm 
    ;     IO_SPRITE_ATTRIBUTE = X_LSB(xe);
    __endasm

    
    IO_SPRITE_ATTRIBUTE = ms->x + 32; //X_LSB(xe);

    __asm
    ; y+=32
    __endasm
    // uint8_t y = ms->y + 32;
    
    // uint16_t xe = X_EXT(msprite->x);

    // IO_SPRITE_ATTRIBUTE = X_LSB(xe);
    // IO_SPRITE_ATTRIBUTE = Y_EXT(msprite->y);
    // IO_SPRITE_ATTRIBUTE = X_MSB(xe) | msprite->spriteFlags;
    // IO_SPRITE_ATTRIBUTE = msprite->spritePattern; //pattern_slot;


    // uint16_t xe = msprite->x;


    __asm 
    ;         IO_SPRITE_ATTRIBUTE = Y_EXT(msprite->y);

    __endasm
    
    IO_SPRITE_ATTRIBUTE = ms->y + 32; //Y_EXT(msprite->y);
    
    uint8_t xmsb = (ms->x > 223  )  ? 1 : 0;

    __asm 
    ;   IO_SPRITE_ATTRIBUTE = (0 << PALETTE_OFFSET_SHIFT) + X_MSB(xe) + ms->spriteFlags;
   
    __endasm
    
    // IO_SPRITE_ATTRIBUTE = (0 << PALETTE_OFFSET_SHIFT) | X_MSB(xe) | msprite->spriteFlags;
    IO_SPRITE_ATTRIBUTE = xmsb + ms->spriteFlags;
    
    __asm 
    ;      IO_SPRITE_ATTRIBUTE = msprite->spritePattern | SPRITE_VISIBLE_MASK;

    __endasm
    
    IO_SPRITE_ATTRIBUTE = ms->spritePattern + SPRITE_VISIBLE_MASK;

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
__asm 
   ld	hl,2	;const
   add	hl,sp
   ld	a,(hl)
   and	127
   ld	bc,_IO_SPRITE_SLOT
   out	(c),a
__endasm
    // IO_SPRITE_SLOT = sprite_slot & SPRITE_PATTERN_MASK;
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