#include <stdio.h>
#include <stdint.h>
#include "gameboy.h"

/*
 * 0 H-Blank
 * 1 V-Blank
 * 2 Reading OAM
 * 3 Reading OAM and VRAM
 */

#define CLOCKS_PER_LINE     456

#define CLOCKS_MODE_2		92
#define CLOCKS_MODE_3		160 // cum. 252
#define CLOCKS_MODE_0		204

uint8_t screen[SCREEN_WIDTH][SCREEN_HEIGHT];
//uint8_t buffer[SCREEN_WIDTH][SCREEN_HEIGHT];

#define WHITE       0xFFE0F8D0
#define LIGHT_GRAY  0xFF88C070
#define DARK_GRAY   0xFF346856
#define BLACK       0xFF081820
#define TRANSPARENT 0x00000000

int ppu_frames = 0;

/* Some observations about line behaviour:
 *  line 153 puts 0 in LY
 *  we then get an entire line of mode 1 (didnt time clocks)
 *  no LYC recheck when 153/0 goes to 0 (it seems)
 *  then a normal line of LY=0
 *  Do we still get "virtual" interrupts even though the mode doesnt change in STAT
 */

void lcd_set_stat_mode(struct gameboy *gb, uint8_t mode) {
    //gb->mem[STAT] &= ~0x3;
    uint8_t stat = mem_read_u8(gb, STAT) & ~0x3;
    mode &= 0x3;
    //gb->mem[STAT] = (gb->mem[STAT] & ~mode) | mode;
    gb->mem[STAT] = (stat & ~mode) | mode;
    // func below prevents the setting of bits 0,1
    //mem_write_u8(gb, STAT, (stat & ~mode) | mode);
}

void lcd_copy_buf(struct gameboy *gb) {
    uint32_t *pixels = (uint32_t *) gb->gpu->pixel_buffer;
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            int shade = screen[x][y];
            switch(shade) {
                case 0:
                    pixels[(y*SCREEN_WIDTH)+x] = WHITE;
                    //pixels[x][y] = WHITE;
                    break;
                case 1:
                    pixels[(y*SCREEN_WIDTH)+x] = LIGHT_GRAY;
                    //pixels[x][y] = LIGHT_GRAY;
                    break;
                case 2:
                    pixels[(y*SCREEN_WIDTH)+x] = DARK_GRAY;
                    //pixels[x][y] = DARK_GRAY;
                    break;
                case 3:
                    pixels[(y*SCREEN_WIDTH)+x] = BLACK;
                    //pixels[x][y] = BLACK;
                    break;
            }
        }
    }
}

void lcd_dmg_pixel(struct gameboy *gb, int lx) {
    if (mem_read_u8(gb, LCDC) & LCDC_BG_ENABLE) lcd_bg(gb, lx);
    if (mem_read_u8(gb, LCDC) & LCDC_WIN_ENABLE) lcd_win(gb, lx);
    if (mem_read_u8(gb, LCDC) & LCDC_OBJ_ENABLE) lcd_obj(gb, lx);

    uint8_t color;
    if (gb->gpu->obj_idx == 0) {
        color = gb->gpu->bg_color;
    } else if (gb->gpu->bg_idx == 0) {
        color = gb->gpu->obj_color;
    } else if (!gb->gpu->obj_priority) {
        color = gb->gpu->obj_color;
    } else {
        color = gb->gpu->bg_color;
    }

    int ly = mem_read_u8(gb, LY);
    screen[lx][ly] = color;
    
//    if (ly == 0 && lx == 0) {
//        printf("x0y0 idx %d color %d bgdata %X\n", gb->gpu->bg_idx, color, gb->gpu->bg_data);
//    }
}

void lcd_upd(struct gameboy *gb, int clocks) {
    //if ((gb->mem[LCDC] & (0x1 << 7)) == 0)
    if ((mem_read_u8(gb, LCDC) & (0x1 << 7)) == 0)
        return;
    gb->clocks_lcd += clocks;
    //uint8_t stat = mem_read_u8(gb, STAT);
    int ly = mem_read_u8(gb, LY);
    //switch (gb->mem[STAT] & STAT_MODE) {
    switch (mem_read_u8(gb, STAT) & STAT_MODE) {
        case 2:
            gb->gpu->lx = 0;
            if (gb->clocks_lcd >= CLOCKS_MODE_2) {
                lcd_start_line(gb);
                lcd_set_stat_mode(gb, 3);
            }
            break;
        case 3:
            while((gb->clocks_lcd - CLOCKS_MODE_2) >= gb->gpu->lx) {
                lcd_dmg_pixel(gb, gb->gpu->lx++);
                if (gb->gpu->lx >= 160) {
                    break;
                }
            }

            if (gb->clocks_lcd >= (CLOCKS_MODE_2 + CLOCKS_MODE_3)) {
                lcd_set_stat_mode(gb, 0);
                /* H-Blank STAT Interrupt */
                if (mem_read_u8(gb, STAT) & STAT_MODE_0_IRQ)
                    gb_raise_irq(gb, IRQ_STAT);
            }
            break;
        case 0: /* H-Blank */
        case 1:
            if (gb->clocks_lcd >= CLOCKS_PER_LINE) {
                gb->clocks_lcd = gb->clocks_lcd - CLOCKS_PER_LINE;

                ly++;

                if(ly == 154) {
                    ly = 0;
                    ppu_frames++;
                    lcd_copy_buf(gb);
                }
                
                if(ly == 144) {
                    lcd_set_stat_mode(gb, 1);
                    /* V-Blank Interrupt */
                    gb_raise_irq(gb, IRQ_VBLANK);
                    /* V-Blank or Read-OAM STAT Interrupt */
                    if (mem_read_u8(gb, STAT) & STAT_MODE_1_IRQ)
                        gb_raise_irq(gb, IRQ_STAT);
                    else if (mem_read_u8(gb, STAT) & STAT_MODE_2_IRQ)
                        gb_raise_irq(gb, IRQ_STAT);
                    
                } else if (ly < 144) {
                    lcd_set_stat_mode(gb, 2);
                    /* Read-OAM STAT Interrupt */
                    if (mem_read_u8(gb, STAT) & STAT_MODE_2_IRQ)
                        gb_raise_irq(gb, IRQ_STAT);
                } else {
                    /* Stay in V-Blank */
                }
                
                mem_write_u8(gb, LY, ly);
                lcd_check_lyc(gb);
            }
            break;
    }
}

void lcd_check_lyc(struct gameboy *gb) {
    if ((gb->mem[LCDC] & (0x1 << 7)) == 0)
        return;
    if((gb->mem[LYC] && gb->mem[LY] == gb->mem[LYC])
       || (!gb->mem[LYC] && gb->mem[LY] == 153)) {
        
        if (gb->mem[STAT] & (0x1 << 2)) {
            /* Do NOT raise IRQ if the LY=LYC flag is already set */
            /* FIXME: perhaps this is only when IME=0 ? */
        } else {
            /* Set Match flag */
            gb->mem[STAT] |= (0x1 << 2);
            /* Interrupt */
            if (gb->mem[STAT] & (0x1 << 6))
                gb->mem[IF] |= (0x1 << 1);
        }
    } else {
        /* Disable Match flag */
        gb->mem[STAT] &= ~(0x1 << 2);
    }
}

uint16_t lcd_read_tile_data(struct gameboy *gb, int map, int x, int y) {
    int ysub = y % 8;
    map = map ? 1 : 0;
    
    uint16_t charaddr = (0x9800 | (map << 10) | ((y & 0xF8) << 2) | ((x & 0xF8) >> 3));
    uint8_t tidx = mem_read_vram_u8(gb, charaddr);

    uint16_t tileaddr;
    if (mem_read_u8(gb, LCDC) & LCDC_TILE_DATA)
        tileaddr = (tidx << 4) | ((ysub & 0x7) << 1);
    else
        tileaddr = (0x1000 + (((int8_t) tidx) << 4)) | ((ysub & 0x7) << 1);
    return mem_read_vram_u16(gb, 0x8000 + tileaddr);
}

void lcd_bg(struct gameboy *gb, int lx) {
    int ly = mem_read_u8(gb, LY);
    int scy = mem_read_u8(gb, SCY);
    int scx = mem_read_u8(gb, SCX);
    int sx = (scx + lx) & 255;
    int sy = (scy + ly) & 255;
    int tx = (scx + lx) % 8;
    
    if (tx == 0 || lx == 0)
        gb->gpu->bg_data = lcd_read_tile_data(gb, mem_read_u8(gb, LCDC) & LCDC_BG_MAP, sx, sy);
    
    int lo = (gb->gpu->bg_data << 0) & (0x80 >> tx) ? 1 : 0;
    int hi = (gb->gpu->bg_data >> 8) & (0x80 >> tx) ? 2 : 0;
    gb->gpu->bg_idx = hi + lo;
    gb->gpu->bg_color = (mem_read_u8(gb, BGP) & (0x3 << (gb->gpu->bg_idx*2))) >> (gb->gpu->bg_idx*2);
}

void lcd_win(struct gameboy *gb, int lx) {
    int ly = mem_read_u8(gb, LY);
    int wy = mem_read_u8(gb, WY);
    int wx = mem_read_u8(gb, WX);
    int sx = (lx - wx + 7); // FIXME modulo
    int sy = (ly - wy);
    
    if (sx < 0) return;
    if (sy < 0) return;
    
    /* FIXME: return if not in window */
    
    int tx = sx % 8;
    if (tx == 0 || lx == 0)
        gb->gpu->win_data = lcd_read_tile_data(gb, mem_read_u8(gb, LCDC) & LCDC_WIN_MAP, sx, sy);
    
    int lo = (gb->gpu->win_data << 0) & (0x80 >> tx) ? 1 : 0;
    int hi = (gb->gpu->win_data >> 8) & (0x80 >> tx) ? 2 : 0;
    gb->gpu->bg_idx = hi + lo;
    gb->gpu->bg_color = (mem_read_u8(gb, BGP) & (0x3 << (gb->gpu->bg_idx*2))) >> (gb->gpu->bg_idx*2);
}

void lcd_obj(struct gameboy *gb, int lx) {
    int ly = mem_read_u8(gb, LY);
    int height = mem_read_u8(gb, LCDC) & LCDC_OBJ_SIZE ? 16 : 8;
    gb->gpu->obj_idx = 0;
    gb->gpu->obj_color = 0;
    for (int i = 0; i < gb->gpu->num_obj; i++) {
        struct obj *o = gb->gpu->obj[i];
        int tx = lx - (o->x - 8);
        int ty = ly - (o->y - 16);
        if (o->flipy) ty = (height - 1) - ty;
        if (o->flipx) tx = 7 - tx;

        if (tx < 0 || tx >= 8) continue;

        uint16_t tileaddr = 0x8000 + (o->tidx << 4) + (ty << 1);
        gb->gpu->obj_data = mem_read_vram_u16(gb, tileaddr);
        int lo = (gb->gpu->obj_data << 0) & (0x80 >> tx) ? 1 : 0;
        int hi = (gb->gpu->obj_data >> 8) & (0x80 >> tx) ? 2 : 0;
        if (hi + lo == 0) continue;
        gb->gpu->obj_idx = hi + lo;
        
        uint8_t palette = o->dmg_pal ? mem_read_u8(gb, OBP1) : mem_read_u8(gb, OBP0);
        gb->gpu->obj_color = (palette & (0x3 << (gb->gpu->obj_idx*2))) >> (gb->gpu->obj_idx*2);
        gb->gpu->obj_priority = o->priority;
    }
}

void lcd_start_line(struct gameboy *gb) {
    int ly = mem_read_u8(gb, LY);
    int height = mem_read_u8(gb, LCDC) & LCDC_OBJ_SIZE ? 16 : 8;
    
    struct obj *oam = (struct obj *) &gb->oam;

    gb->gpu->num_obj = 0;
    for (int i = 0; i < 40; i++) {
        struct obj *ob = &(oam[i]);
        
        unsigned int fooy = ly - (ob->y - 16);
        if (fooy >= height) continue;
        
        if (ob->y == 0 || ob->y >= 160)
            continue;
        
        // Is this line inside the obj
//        if (ly < (ob->y - 16) || ly >= ((ob->y - 16) + height))
//            continue;

        gb->gpu->obj[gb->gpu->num_obj++] = ob;
        if (gb->gpu->num_obj == 10)
            break;
    }
    
    for (int l = 0; l < gb->gpu->num_obj; l++) {
        for (int h = l+1; h < gb->gpu->num_obj; h++) {
            if (gb->gpu->obj[l]->x < gb->gpu->obj[h]->x) {
                struct obj *tmp = gb->gpu->obj[h];
                gb->gpu->obj[h] = gb->gpu->obj[l];
                gb->gpu->obj[l] = tmp;
            }
        }
    }
}
