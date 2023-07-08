/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_main_screen_ram_bank() in zxnext_layer2.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <stdint.h>
#include <stdbool.h>
#include <intrinsic.h>
#include <stddef.h>
#include <errno.h>
#include <z80.h>
#include <stddef.h>

#include <input.h>
#include <string.h>

#include "zxnext_layer2.h"


#include "zxnext_layer2.h"
#include "layer2_defs.h"
#include "layer2_common.h"
#include "layer2_draw_pixel_fast.h"

uint8_t buf_256[256];

void layer2_set_main_screen_ram_bank(uint8_t bank)
{
    IO_NEXTREG_REG = REG_LAYER_2_RAM_PAGE;
    IO_NEXTREG_DAT = bank;
}

void layer2_set_shadow_screen_ram_bank(uint8_t bank)
{
    IO_NEXTREG_REG = REG_LAYER_2_SHADOW_RAM_PAGE;
    IO_NEXTREG_DAT = bank;
}


#define LAYER2_VISIBLE_MASK 0x02
#define LAYER2_SHADOW_SCREEN_MASK 0x08
#define LAYER2_WRITE_ENABLE_MASK 0x01
#define LAYER2_SCREEN_SECTION_MASK 0x03
#define LAYER2_SCREEN_SECTION_SHIFT 6

/*
 * (R/W) Layer 2 screen configuration port (0x123B):
 * bits 6-7 = Section (0 = top, 1 = middle, 2 = bottom) of the specified layer 2
 * screen (main or shadow) to page in for writing.
 * bits 4-5 = Reserved, must be 0.
 * bit 3 = If 0, page in main layer 2 screen as specified by LAYER2_RAM_PAGE_REGISTER.
 * If 1, page in shadow layer 2 screen as specified by LAYER2_SHADOW_RAM_PAGE_REGISTER.
 * bit 2 = Reserved, must be 0.
 * bit 1 = Layer 2 screen visible on the display.
 * bit 0 = Enable the paged-in layer 2 screen (main or shadow) for writing.
 */

// FIXME: If the LAYER2_PORT is also readable, we can split this function into
// two functions; one for enabling the layer 2 screen and one for paging it in.
// Then we should also preserve the other bits when setting the layer 2 screen
// section for writing.

void layer2_configure(bool layer2_visible,
                      bool layer2_write_enabled,
                      bool layer2_shadow_screen,
                      uint8_t layer2_screen_section)
{
    uint8_t value = (layer2_screen_section & LAYER2_SCREEN_SECTION_MASK) << LAYER2_SCREEN_SECTION_SHIFT;

    if (layer2_visible)
    {
        value = value | LAYER2_VISIBLE_MASK;
    }

    if (layer2_write_enabled)
    {
        value = value | LAYER2_WRITE_ENABLE_MASK;
    }

    if (layer2_shadow_screen)
    {
        value = value | LAYER2_SHADOW_SCREEN_MASK;
    }

    IO_LAYER_2_CONFIG = value;
}

/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_load_screen() in zxnext_layer2.h.
 ******************************************************************************/



static void get_screen_pages(layer2_screen_t *screen, uint8_t *pages);

void layer2_load_screen(const char *filename, layer2_screen_t *screen, uint8_t mmu_slot, bool has_palette)
{
    uint8_t filehandle;
    uint8_t old_page;
    uint8_t screen_pages[6];
    void *screen_address;
    uint8_t i;

    if ((filename == NULL) || (mmu_slot < 2) || (mmu_slot > 7))
    {
        return;
    }

    // Disable layer 2 screen writing at bottom 16 KB prior to ESXDOS usage.
    layer2_configure(true, false, false, 0);

    errno = 0;
    filehandle = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if (errno)
    {
        return;
    }

    if (has_palette)
    {
        esxdos_f_read(filehandle, buf_256, 256);
        if (errno)
        {
            goto end;
        }
        layer2_set_palette((uint16_t *) buf_256, 128, 0);

        esxdos_f_read(filehandle, buf_256, 256);
        if (errno)
        {
            goto end;
        }
        layer2_set_palette((uint16_t *) buf_256, 128, 128);
    }

    // Load layer 2 screen in 8 KB chunks using the given MMU slot.

    old_page = ZXN_READ_REG(REG_MMU0 + mmu_slot);
    get_screen_pages(screen, screen_pages);
    screen_address = zxn_addr_from_mmu(mmu_slot);

    for (i = 0; i < 6; i++)
    {
        ZXN_WRITE_REG(REG_MMU0 + mmu_slot, screen_pages[i]);
        esxdos_f_read(filehandle, screen_address, 8192);
        if (errno)
        {
            break;
        }
    }

    ZXN_WRITE_REG(REG_MMU0 + mmu_slot, old_page);

end:
    esxdos_f_close(filehandle);
}


// TODO: Remove if layer2_screen_t is changed and off-screen banks are consecutive.
static void get_screen_pages(layer2_screen_t *screen, uint8_t *pages)
{
    if (!IS_OFF_SCREEN(screen))
    {
        uint8_t screen_start_page;
        uint8_t i;

        screen_start_page = IS_MAIN_SCREEN(screen) ?
            layer2_get_main_screen_ram_bank() : layer2_get_shadow_screen_ram_bank();
        screen_start_page = screen_start_page << 1;

        for (i = 0; i < 6; i++)
        {
            pages[i] = screen_start_page + i;
        }
    }
    else
    {
        pages[0] = screen->top_bank << 1;
        pages[1] = pages[0] + 1;
        pages[2] = screen->middle_bank << 1;
        pages[3] = pages[2] + 1;
        pages[4] = screen->bottom_bank << 1;
        pages[5] = pages[4] + 1;
    }
}


/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_draw_pixel() in zxnext_layer2.h.
 ******************************************************************************/

void layer2_draw_pixel(uint8_t x,
                       uint8_t y,
                       uint8_t color,
                       layer2_screen_t *screen)
{
    if (y > 191)
    {
        return;
    }

    if(screen !=NULL)
        init_switch_screen(screen);

    if (y < 64)
    {
        // top
        switch_top_screen_section(screen);
    }
    else if (y < 128)
    {
        // middle
        switch_middle_screen_section(screen);
        y -= 64;
    }
    else
    {
        // bottom
        switch_bottom_screen_section(screen);
        y -= 128;
    }

    z80_bpoke(SCREEN_ADDRESS(screen) + x + (y << 8), color);

    end_switch_screen(screen);
}

/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of helper function layer2_draw_pixel_fast().
 *
 * The caller of this function is required to validate its arguments and make
 * sure it is called within init_switch_screen() and end_switch_screen().
 ******************************************************************************/

void layer2_draw_pixel_fast(uint8_t x,
                            uint8_t y,
                            uint8_t color,
                            layer2_screen_t *screen)
{
    if (y < 64)
    {
        // top
        switch_top_screen_section(screen);
    }
    else if (y < 128)
    {
        // middle
        switch_middle_screen_section(screen);
        y -= 64;
    }
    else
    {
        // bottom
        switch_bottom_screen_section(screen);
        y -= 128;
    }

    z80_bpoke(SCREEN_ADDRESS(screen) + x + (y << 8), color);
}

/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_draw_line() in zxnext_layer2.h.
 ******************************************************************************/


// http://www.edepot.com/linebresenham.html
void layer2_draw_line(uint8_t x1,
                      uint8_t y1,
                      uint8_t x2,
                      uint8_t y2,
                      uint8_t color,
                      layer2_screen_t *screen)
{
    uint16_t dx;
    uint16_t dy;
    int8_t incx;
    int8_t incy;
    int16_t balance;

    if ((y1 > 191) || (y2 > 191))
    {
        return;
    }

    if (x2 >= x1)
    {
        dx = x2 - x1;
        incx = 1;
    }
    else
    {
        dx = x1 - x2;
        incx = -1;
    }

    if (y2 >= y1)
    {
        dy = y2 - y1;
        incy = 1;
    }
    else
    {
        dy = y1 - y2;
        incy = -1;
    }

    init_switch_screen(screen);

    if (dx >= dy)
    {
        dy <<= 1;
        balance = dy - dx;
        dx <<= 1;

        while (x1 != x2)
        {
            layer2_draw_pixel_fast(x1, y1, color, screen);
            if (balance >= 0)
            {
                y1 += incy;
                balance -= dx;
            }
            balance += dy;
            x1 += incx;
        }

        layer2_draw_pixel_fast(x1, y1, color, screen);
    }
    else
    {
        dx <<= 1;
        balance = dx - dy;
        dy <<= 1;

        while (y1 != y2)
        {
            layer2_draw_pixel_fast(x1, y1, color, screen);
            if (balance >= 0)
            {
                x1 += incx;
                balance -= dy;
            }
            balance += dx;
            y1 += incy;
        }

        layer2_draw_pixel_fast(x1, y1, color, screen);
    }

    end_switch_screen(screen);
}



/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_offset_x() in zxnext_layer2.h.
 ******************************************************************************/

void layer2_set_offset_x(uint8_t offset_x)
{
    IO_NEXTREG_REG = REG_LAYER_2_OFFSET_X;
    IO_NEXTREG_DAT = offset_x;
}


/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_offset_y() in zxnext_layer2.h.
 ******************************************************************************/

void layer2_set_offset_y(uint8_t offset_y)
{
    IO_NEXTREG_REG = REG_LAYER_2_OFFSET_Y;
    IO_NEXTREG_DAT = offset_y % 192;
}


/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_set_palette() in zxnext_layer2.h.
 ******************************************************************************/

void layer2_set_palette(const uint16_t *colors, uint16_t length, uint8_t palette_index)
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


/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_main_screen_ram_bank() in zxnext_layer2.h.
 ******************************************************************************/

uint8_t layer2_get_main_screen_ram_bank(void)
{
    IO_NEXTREG_REG = REG_LAYER_2_RAM_PAGE;
    return IO_NEXTREG_DAT;
}

/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_get_shadow_screen_ram_bank() in zxnext_layer2.h.
 ******************************************************************************/

uint8_t layer2_get_shadow_screen_ram_bank(void)
{
    IO_NEXTREG_REG = REG_LAYER_2_SHADOW_RAM_PAGE;
    return IO_NEXTREG_DAT;
}


void init_switch_screen(layer2_screen_t *screen)
{
    if ((screen != NULL) && (screen->screen_type == OFF_SCREEN))
    {
        screen->tmp = z80_bpeek(__SYSVAR_BANKM);
    }
}

void switch_top_screen_section(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type == MAIN_SCREEN))
    {
        layer2_configure(true, true, false, LAYER2_SCREEN_TOP);
    }
    else if (screen->screen_type == SHADOW_SCREEN)
    {
        layer2_configure(true, true, true, LAYER2_SCREEN_TOP);
    }
    else
    {
        switch_ram_bank(screen->top_bank);
    }
}

void switch_middle_screen_section(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type == MAIN_SCREEN))
    {
        layer2_configure(true, true, false, LAYER2_SCREEN_MIDDLE);
    }
    else if (screen->screen_type == SHADOW_SCREEN)
    {
        layer2_configure(true, true, true, LAYER2_SCREEN_MIDDLE);
    }
    else
    {
        switch_ram_bank(screen->middle_bank);
    }
}

void switch_bottom_screen_section(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type == MAIN_SCREEN))
    {
        layer2_configure(true, true, false, LAYER2_SCREEN_BOTTOM);
    }
    else if (screen->screen_type == SHADOW_SCREEN)
    {
        layer2_configure(true, true, true, LAYER2_SCREEN_BOTTOM);
    }
    else
    {
        switch_ram_bank(screen->bottom_bank);
    }
}

void end_switch_screen(layer2_screen_t *screen)
{
    if ((screen == NULL) || (screen->screen_type != OFF_SCREEN))
    {
        layer2_configure(true, false, false, 0);
    }
    else
    {
        switch_ram_bank(screen->tmp);
    }
}


void switch_ram_bank(uint8_t bank)
{
    uint8_t page = bank << 1;

    intrinsic_di();
    z80_bpoke(__SYSVAR_BANKM, ((z80_bpeek(__SYSVAR_BANKM) & 0xF8) | (bank & 0x07)));
    ZXN_WRITE_MMU6(page);
    ZXN_WRITE_MMU7(page + 1);
    intrinsic_ei();
}


/*******************************************************************************
 * Stefan Bylund 2017
 *
 * Implementation of layer2_draw_text() and layer2_set_font() in zxnext_layer2.h.
 ******************************************************************************/

#define ROM_FONT_ADDRESS 0x3D00

static void layer2_draw_char(uint8_t x,
                             uint8_t y,
                             char ch,
                             uint8_t color,
                             layer2_screen_t *screen);

static uint8_t *font_address = ROM_FONT_ADDRESS;

void layer2_draw_text(uint8_t row,
                      uint8_t column,
                      const char *text,
                      uint8_t color,
                      layer2_screen_t *screen)
{
    uint8_t x = column << 3;
    uint8_t y = row << 3;
    char *str = (char *) text;

    if ((row > 23) || (column > 31) || (text == NULL))
    {
        return;
    }

    init_switch_screen(screen);

    // If the Spectrum ROM font is used, make sure the Spectrum ROM is switched in.
    if (font_address == (uint8_t *) ROM_FONT_ADDRESS)
    {
        IO_7FFD = IO_7FFD_ROM0;
    }

    if (y < 64)
    {
        switch_top_screen_section(screen);
    }
    else if (y < 128)
    {
        switch_middle_screen_section(screen);
        y -= 64;
    }
    else
    {
        switch_bottom_screen_section(screen);
        y -= 128;
    }

    while (*str != '\0')
    {
        char ch = *str;
        if ((ch < 32) || (ch > 127))
        {
            ch = '?';
        }

        layer2_draw_char(x, y, ch, color, screen);

        if (x == 248)
        {
            break;
        }

        x += 8;
        str++;
    }

    end_switch_screen(screen);
}

void layer2_set_font(const void *new_font_address)
{
    font_address = (new_font_address != NULL) ? (uint8_t *) new_font_address : (uint8_t *) ROM_FONT_ADDRESS;
}

static void layer2_draw_char(uint8_t x,
                             uint8_t y,
                             char ch,
                             uint8_t color,
                             layer2_screen_t *screen)
{
    uint8_t lines = 8;
    uint8_t *source = font_address + ((ch - 32) << 3);
    uint8_t *dest = SCREEN_ADDRESS(screen) + x + (y << 8);

    while (lines--)
    {
        uint8_t i;
        for (i = 0; i < 8; i++)
        {
            uint8_t bit = *source & (0x80 >> i);
            if (bit != 0)
            {
                z80_bpoke(dest + i, color);
            }
        }

        dest += 256;
        source++;
    }
}


static void layer2_clear_screen_section(layer2_screen_t *screen);

void layer2_clear_screen(uint8_t color, layer2_screen_t *screen)
{
    // Used in layer2_clear_screen_section().
    memset(buf_256, color, 256);

    init_switch_screen(screen);

    switch_top_screen_section(screen);
    layer2_clear_screen_section(screen);

    switch_middle_screen_section(screen);
    layer2_clear_screen_section(screen);

    switch_bottom_screen_section(screen);
    layer2_clear_screen_section(screen);

    end_switch_screen(screen);
}

static void layer2_clear_screen_section(layer2_screen_t *screen)
{
    uint8_t height = 64;
    uint8_t *dest = SCREEN_ADDRESS(screen);

    // buf_256 is set by layer2_clear_screen().

    while (height--)
    {
        // TODO: Doesn't work with sdcc_ix.
        //intrinsic_ldi(dest, buf_256, 256);
        memcpy(dest, buf_256, 256);
        dest += 256;
    }
}
