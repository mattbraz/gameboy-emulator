#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "gameboy.h"

uint8_t mem_read_regs_u8(struct gameboy *gb, uint16_t addr) {
    /* Registers */
    return gb->regs[addr - REGS_START];
}

uint8_t mem_read_vram_u8(struct gameboy *gb, uint16_t addr) {
    /* Video RAM */
    if (addr >= VRAM_START && addr <= VRAM_END) {}
    return gb->vram[addr - VRAM_START];
}


uint8_t mem_read_u8(struct gameboy *gb, uint16_t addr) {
    if (addr >= 0x0000 && addr <= 0x00FF) {
        if (gb->boot) return gb->boot_rom[addr];
    }
    
    /* Cart ROM area */
    if (addr >= 0x0000 && addr < 0x4000) {
        return gb->cart_rom[addr];
    }
    
    /* Cart ROM banking area */
    if (addr >= 0x4000 && addr <= 0x7FFF) {
        uint32_t caddr = (addr - 0x4000) + (gb->rom_bank * 0x4000);
        return gb->cart_rom[caddr];
    }

    /* Cart RAM banking area */
    if (addr >= CART_RAM_START && addr <= CART_RAM_END) {
        //uint16_t caddr = (addr - 0xA000) + ((gb->ram_bank-1) * 0x2000);
        uint16_t caddr = (addr - CART_RAM_START);
        return gb->cart_ram[caddr];
    }
    
    /* Registers */
//    if (addr >= REGS_START && addr <= REGS_END) {
//        return gb->regs[addr - REGS_START];
//    }
    
    /* Work RAM */
    if (addr >= WRAM_START && addr <= WRAM_END) {
        return gb->wram[addr - WRAM_START];
    }
    
    /* High RAM */
    if (addr >= HRAM_START && addr <= HRAM_END) {
        return gb->hram[addr - HRAM_START];
    }
    
    /* VRAM access restrictions */
//    if (addr >= 0xFE00 && addr <= 0xFE9F) {
//        if ((gb->mem[STAT] & STAT_MODE) >= 2) {
//            printf("Reading OAM %04X\n", addr);
//            return 0xFF;
//        }
//        if (gb->dma)
//            return 0xFF;
//    }
//    if (addr >= 0x8000 && addr <= 0x9FFF) {
//        if ((gb->mem[STAT] & STAT_MODE) == 3) {
//            printf("Reading VRAM %04X\n", addr);
//            return 0xFF;
//        }
//    }

    /* Video RAM */
    if (addr >= VRAM_START && addr <= VRAM_END) {
        return gb->vram[addr - VRAM_START];
    }
    
    /* OAM */
    if (addr >= OAM_START && addr <= OAM_END) {
        return gb->oam[addr - OAM_START];
    }

    /* Joypad */
    if (addr == P1) {
        if (gb->mem[addr] & (0x1 << 4)) /* Buttons */
            return (gb->mem[addr] & 0xF0) | ((gb->joypad->state >> 0) & 0x0F);
        if (gb->mem[addr] & (0x1 << 5)) /* D-Pad */
            return (gb->mem[addr] & 0xF0) | ((gb->joypad->state >> 4) & 0x0F);
        return 0x00;
    }

    
    return gb->mem[addr];
}

void mem_write_u8(struct gameboy *gb, uint16_t addr, uint8_t value) {

    if(addr == 0xFF50) {
        gb->boot = 0;
    }
    
    /* Cart ROM area */
    if (addr >= 0x0000 && addr <= 0x7FFF) {
        mem_handle_banking(gb, addr, value);
        return;
    }
    
    /* Cart RAM area */
    if (addr >= CART_RAM_START && addr <= CART_RAM_END) {
        uint16_t caddr = addr - CART_RAM_START;
        gb->cart_ram[caddr] = value;
        return;
    }

    /* Work RAM */
    if (addr >= WRAM_START && addr <= WRAM_END) {
        gb->wram[addr - WRAM_START] = value;
    }
    
    /* High RAM */
    if (addr >= HRAM_START && addr <= HRAM_END) {
        gb->hram[addr - HRAM_START] = value;
    }
    
    /* VRAM access restrictions */
//    // VRAM can only be written during H-Blank, V-Blank or OAM-Read
//    if (addr >= 0x8000 && addr <= 0x9F00) {
//        if ((gb->mem[STAT] & 0x3) == 3)
//            return;
//    }
//    // OAM can only be written during H-Blank of V-Blank
//    if (addr >= 0xFE00 && addr <= 0xFE9F) {
//        if ((gb->mem[STAT] & 0x3) >= 2)
//            return;
//    }

    /* VRAM */
    if (addr >= VRAM_START && addr <= VRAM_END) {
        gb->vram[addr - VRAM_START] = value;
    }
    
    /* OAM */
    if (addr >= OAM_START && addr <= OAM_END) {
        gb->oam[addr - OAM_START] = value;
    }

    /* FIXME: The below can be cleaned up, some bits are always high/low */
    uint8_t orig = 0;
    switch(addr) {
        case SC:
            gb->mem[addr] = (gb->mem[addr] & ~0x7E) | (value & 0x7E);
            if (value & (0x1 << 7)) {
                gb->serial = 1;
                gb->clocks_serial = 0;
            }
            break;
        case P1:
            /* Write only bits 4 and 5 */
            gb->mem[addr] = (gb->mem[addr] & ~0x30) | (value & 0x30);
            break;
        case DMA:
            gb->mem[addr] = value;
            gb->dma = 1;
            gb->clocks_dma = 0;
            break;
        case LCDC:
            orig = gb->mem[addr];
            gb->mem[addr] = value;
            if ((orig & (0x1 << 7)) && !(value & (0x1 << 7))) {
                /* Disable LCD */
                /* Reset LY and Mode and Match flag in STAT */
                /* FIXME: Do we check the match flag now? */
                gb->mem[LY] = 0;
                //gb->mem[STAT] &= ~(0x1 << 2);
                //gb->mem[STAT] &= ~0x03; // FIXME check this - state
                lcd_set_stat_mode(gb, 0);
            } else if (!(orig & (0x1 << 7)) && (value & (0x1 << 7))) {
                /* Enable LCD */
                gb->clocks_lcd = 0;
                lcd_check_lyc(gb);
            }
            break;
        case STAT:
            /* Do not write bit 7,0,1 */
            orig = gb->mem[addr];
            gb->mem[addr] = (gb->mem[addr] & ~0x7C) | (value & 0x7C);
            if (!(orig & (0x1 << 6)) && (value & (0x1 << 6)))
                lcd_check_lyc(gb);
            break;
        case LYC:
            gb->mem[addr] = value;
            lcd_check_lyc(gb);
            break;
        case DIV:
            /* Writing any vaue to this register sets it to 00h */
            gb->mem[addr] = 0x00;
            gb->clocks_div = 0;
            break;
        case IF:
            /* bits 5,6,7 always high */
            gb->mem[addr] = value | (0x7 << 5);
            break;
//        case IE:
//            // bits 5,6,7 always high
//            //gb->mem[addr] = value | (0x7 << 5);
//            gb->mem[addr] = value
//            break;
        case TAC:
            /* bits 3,4,5,6,7 always high */
            gb->mem[addr] = value | (0x1F << 3);
            break;
        // case NR10:
        // case NR11:
        // case NR12:
        // case NR13:
        case NR14:
            gb->mem[addr] = value;
            if (value & (0x01 << 7)) {
                //apu_trigger2(gb, gb->apu.channel_1, 1);
                gb->apu.channel_1.recalc = 1;
            }
            break;
        // case NR21:
        // case NR22:
        // case NR23:
        case NR24:
            gb->mem[addr] = value;
            if (value & (0x01 << 7)) {
                //apu_trigger2(gb, gb->apu.channel_2, 2);
                gb->apu.channel_2.recalc = 1;
            }
            break;
//        case NR30:
//        case NR31:
//        case NR32:
//        case NR33:
        case NR34:
            gb->mem[addr] = value;
            if (value & (0x01 << 7)) {
                //apu_trigger2(gb, gb->apu.channel_3, 3);
                gb->apu.channel_3.recalc = 1;
            }
            break;//
//        case NR41:
//        case NR42:
//        case NR43:
        case NR44:
            gb->mem[addr] = value;
            if (value & (0x01 << 7)) {
                //apu_trigger2(gb, gb->apu.channel_4, 4);
                gb->apu.channel_4.recalc = 1;
            }
            break;
//        case NR50:
//        case NR51:
//        case NR52:
        default:
            gb->mem[addr] = value;
            break;
    }
}

uint16_t mem_read_u16(struct gameboy *gb, uint16_t addr) {
    uint8_t lo = mem_read_u8(gb, addr);
    uint8_t hi = mem_read_u8(gb, addr+1);
    return (hi << 8) | lo;
}

uint16_t mem_read_vram_u16(struct gameboy *gb, uint16_t addr) {
    uint8_t lo = mem_read_vram_u8(gb, addr);
    uint8_t hi = mem_read_vram_u8(gb, addr+1);
    return (hi << 8) | lo;
}

void mem_handle_banking(struct gameboy *gb, uint16_t addr, uint8_t value) {
    /* MBC1 */
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        gb->ram_bank_enable = (value & 0xA);  // FIXME: check this
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        /* Lower 5 bits of ROM bank number */
        gb->rom_bank = (gb->rom_bank & ~0x1F) | (value & 0x1F);
    } else if (addr >= 0x4000 && addr <= 0x5FFF) {
        if (gb->bank_mode == 0x01) {
            /* RAM bank 0-3 */
            gb->ram_bank = (value & 0x3);
        } else {
            /* Upper 2 bits of ROM bank number */
            gb->rom_bank = (gb->rom_bank & 0x1F) | (value & 0x3);
        }
    } else if (addr >= 0x6000 && addr <= 0x7FFF) {
        gb->bank_mode = (value & 0x01);  // FIXME: check this
    }
    if (gb->rom_bank == 0x00 || gb->rom_bank == 0x20 ||
        gb->rom_bank == 0x40 || gb->rom_bank == 0x60)
        gb->rom_bank += 0x01;    
}
