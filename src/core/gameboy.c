#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "gameboy.h"
#include "cpu.h"

#include <pthread.h>

extern uint8_t boot_image[];

int boot = 1;
int logging = 0;
int paused = 0;

int CLOCKS_PER_SDL_FRAME = 70224;

/* Public interface */

void gb_run(struct gameboy *gb) {

    pthread_t self = pthread_self();
    unsigned long tid = (unsigned long) self;

    paused = 0;
    int i = 0;
    while(!paused) {
        gb_main_new(gb);
        //sdl_main(gb);
        if((i++ % 100) == 0)
            //printf("thread id: %d", tid);
            printf("%lu libgb.gb_run - clocks %d\n",tid, gb->clocks_total);
    }
}

void gb_run_clocks(struct gameboy *gb, unsigned int clocks) {}

void gb_run_ops(struct gameboy *gb, unsigned int ops) {
    while (ops--) {
        gb_once(gb);
    }
    //sdl_main(gb);   // FIXME: this is not right
}
void gb_run_frames(struct gameboy *gb, unsigned int frames) {
    while (frames--) {
        gb_main_new(gb);
        //sdl_main(gb);
    }
}

void gb_stop(struct gameboy *gb) {
    paused = 1;
}
void gb_close(struct gameboy *gb) {
    sdl_finish(gb);
}

/* ------------------------------ */

void gb_main_new(struct gameboy *gb) {
    if (paused) return;
    gb->clocks_total = gb->clocks_total % CLOCKS_PER_SDL_FRAME;
    while (gb->clocks_total < CLOCKS_PER_SDL_FRAME) {
        gb_once(gb);
    }
}

void gb_once(struct gameboy *gb) {
    struct op op;

    if (paused) return;

    gb_process_interrupts(gb);

    /* be careful about moving this. Prehistorik man relies on the op after IE being executed. */
    if (gb->ime_delay) {
        gb->ime_delay = 0;
        gb->ime = 1;
    }
    
    if (gb->clock_stopped) {
        gb_add_clocks(gb, 4);
    } else {
        cpu_get_op(gb, &op);
        cpu_exec_op(gb, &op);
        if (logging) cpu_log_op(gb, &op);
        if (logging) cpu_log_regs(gb);
    }
}

void gb_add_clocks(struct gameboy *gb, int clocks) {
    gb->clocks_total += clocks;

    for (; clocks >= 4; clocks -= 4) {
        gb_process_timers(gb, 4);
        gb_process_dma(gb, 4);
        //gb_process_serial(gb, 4);
        gb_process_joypad(gb);
        lcd_upd(gb, 4);
        //apu_upd(gb, 4);
    }
}

void gb_process_dma(struct gameboy *gb, int clocks) {
    /* DMA transfers happen 'all at once' */    
    if (gb->dma == 0)
        return;
    uint16_t dst_addr = OAM;
    uint16_t src_addr = mem_read_u8(gb, DMA) << 8;
    for (uint16_t i = 0; i < 0x100; i++) {
        mem_write_u8(gb, dst_addr++, mem_read_u8(gb, src_addr++));
    }
    gb->dma = 0;
}

/* This is not working currently */
void gb_process_serial(struct gameboy *gb, int clocks) {
/*    if (gb->serial == 0)
        return;
    
    gb->clocks_serial += clocks;
    if (gb->clocks_serial >= (512 * 8)) {
        gb->clocks_serial = gb->clocks_serial % (512 * 8);
        gb_raise_irq(gb, IRQ_SERIAL);
        
        mem_write_u8(gb, SB, 0xFF);
        mem_write_u8(gb, SC, mem_read_u8(gb, SC) & ~80);
    }
*/
}

void gb_process_timers(struct gameboy *gb, int clocks) {
    gb->clocks_div += clocks;
    gb->clocks_tima += clocks;
    
    if (gb->clocks_div >= 256) {
        gb->clocks_div = gb->clocks_div % 256;
        gb->mem[DIV]++;
        //mem_write_regs_u8(gb, DIV, mem_read_regs_u8(gb, DIV) + 1);
        //mem_write_u8(gb, DIV, mem_read_u8(gb, DIV) + 1);
    }
    
    /* Return if the timer is stopped */
    if (!(mem_read_u8(gb, TAC) & (0x1 << 2)))
        return;

    int period = 0;
    switch(mem_read_u8(gb, TAC) & 0x3) {
        case 0x0:
            period = 1024; break;
        case 0x1:
            period = 16; break;
        case 0x2:
            period = 64; break;
        case 0x3:
            period = 256; break;
    }
        
    uint8_t inc = (uint8_t) (gb->clocks_tima / period);
    gb->clocks_tima = gb->clocks_tima % period;
    
    uint16_t result = mem_read_u8(gb, TIMA) + inc;
    if (result >> 8) {
        /*  Overflow */
        /* Assumption: We are not adding enough clocks to overflow TIMA twice. */
        //gb->mem[TIMA] = gb->mem[TMA] + (result % foo_clocks);
        mem_write_u8(gb, TIMA, mem_read_u8(gb, TMA) + (result % period));
        // INTERRUPT
        //gb->mem[IF] |= IRQ_TIMER;
        gb_raise_irq(gb, IRQ_TIMER);
    } else {
        //gb->mem[TIMA] += inc;
        mem_write_u8(gb, TIMA, mem_read_u8(gb, TIMA) + inc);
    }
}

void gb_raise_irq(struct gameboy *gb, uint8_t irq_flag) {
    mem_write_u8(gb, IF, mem_read_u8(gb, IF) | irq_flag);
}
void gb_disable_irq(struct gameboy *gb, uint8_t irq_flag) {
    mem_write_u8(gb, IF, mem_read_u8(gb, IF) & ~irq_flag);
}

void gb_process_interrupts(struct gameboy *gb) {
    uint8_t enab = mem_read_u8(gb, IE);
    uint8_t flag = mem_read_u8(gb, IF);
    uint8_t foo = (enab & flag) & 0x0F;
    if (gb->clock_stopped && foo) {
        /* If a given interrupt request and enable flag are both high
           then HALT mode is cancelled and the interrupt is NOT processed. */
        /* FIXME: Does the IF flag get reset? */
        gb->clock_stopped = 0;
    }
    if (!gb->ime || !foo)
        return;
        
    /* We have interrupts that will be processed. */
    gb->ime_delay = 0;
    gb->ime = 0;
    cpu_stack_push(gb, gb->cpu->PC);
    if (foo & IRQ_VBLANK) {
        gb->cpu->PC = IRQ_VBLANK_JUMP;
        gb_disable_irq(gb, IRQ_VBLANK);
    } else if (foo & IRQ_STAT) {
        gb->cpu->PC = IRQ_STAT_JUMP;
        gb_disable_irq(gb, IRQ_STAT);
    } else if (foo & IRQ_TIMER) {
        gb->cpu->PC = IRQ_TIMER_JUMP;
        gb_disable_irq(gb, IRQ_TIMER);
    } else if (foo & IRQ_SERIAL) {
        gb->cpu->PC = IRQ_SERIAL_JUMP;
        gb_disable_irq(gb, IRQ_SERIAL);
    } else if (foo & IRQ_JOYP) {
        gb->cpu->PC = IRQ_JOYP_JUMP;
        gb_disable_irq(gb, IRQ_JOYP);
    }

    /* Be careful where you put this. It can generate new interrupts. */
    gb_add_clocks(gb, 20);
}

void gb_process_joypad(struct gameboy *gb) {
    if (gb->joypad->state != 0xFF) {
        //uint8_t flag = mem_read_u8(gb, IF);
        //mem_write_u8(gb, IF, flag | (0x1 << 4));
        //gb_raise_irq(gb, IRQ_JOYP);
    }
}

void gb_joypad_press(struct gameboy *gb, uint8_t button) {
    gb->joypad->state &= ~button;
    gb_raise_irq(gb, IRQ_JOYP);
}

void gb_joypad_release(struct gameboy *gb, uint8_t button) {
    gb->joypad->state |= button;
}

struct gameboy *gb_create() {
    struct gameboy *gb;
    gb = malloc(sizeof(struct gameboy));
    if(gb == NULL) {
        printf("Failed to allocate memory for gb\n");
    }
    gb->cpu = malloc(sizeof(struct cpu));
    if(gb->cpu == NULL) {
        printf("Failed to allocate memory for cpu\n");
    }
    gb->gpu = malloc(sizeof(struct gpu));
    if(gb->gpu == NULL) {
        printf("Failed to allocate memory for gpu\n");
    }
    gb->apu = malloc(sizeof(struct apu));
    if(gb->apu == NULL) {
        printf("Failed to allocate memory for apu\n");
    }
    gb->apu->channel_1 = malloc(sizeof(struct channel));
    gb->apu->channel_2 = malloc(sizeof(struct channel));
    gb->apu->channel_3 = malloc(sizeof(struct channel));
    gb->apu->channel_4 = malloc(sizeof(struct channel));
    
    gb->mem = malloc(0x10000);
    if(gb->mem == NULL) {
        printf("Failed to allocate memory for mem\n");
    }
//    gb->cart = malloc(0x200000);
//    if(gb->cart == NULL) {
//        printf("Failed to allocate memory for cart rom\n");
//    }
//    gb->cart_ram = malloc(0x8000);
//    if(gb->cart_ram == NULL) {
//        printf("Failed to allocate memory for cart ram\n");
//    }
    gb->joypad = malloc(sizeof(struct joypad));
    if(gb->joypad == NULL) {
        printf("Failed to allocate memory for joypad\n");
    }
    
    return gb;
}

void gb_free(struct gameboy *gb) {
    free(gb->cpu);
    free(gb->gpu);
    free(gb->apu->channel_1);
    free(gb->apu->channel_2);
    free(gb->apu->channel_3);
    free(gb->apu->channel_4);
    free(gb->apu);
    free(gb->mem);
    //free(gb->cart);
    //free(gb->cart_ram);
    free(gb->joypad);
    free(gb);
}


void gb_reset(struct gameboy *gb) {
    int i;
    
    boot = 1;
    
    for(i = 0x0; i < 0x100; i++)
        gb->boot_rom[i] = boot_image[i];
    for(i = 0x0000; i <= 0xFFFF; i++)
        gb->mem[i] = 0xAA;

//    // BG maps set to 0
//    for(i = 0x9800; i <= 0x9FFF; i++)
//        gb->mem[i] = 0x00;
//    // CHR data set to 0
//    for(i = 0x8000; i < 0x9800; i++)
//        gb->mem[i] = 0x00;

    // -----------------------------------
    
    // Boot ROM page toggle
    gb->mem[0xFF50] = 0x00;

    // REGS
//    for (i = REGS_START; i <= REGS_END; i++) {
//        gb->mem[i] = 0x00;
//        //gb->regs[i - REGS_START] = 0x00;
//    }
    // VRAM
    for (i = VRAM_START; i <= VRAM_END; i++) {
        gb->vram[i - VRAM_START] = 0x00;
    }
    // WRAM
    for (i = WRAM_START; i <= WRAM_END; i++) {
        gb->wram[i - WRAM_START] = 0xFF;
    }
    // HRAM
    for (i = HRAM_START; i <= HRAM_END; i++) {
        gb->hram[i - HRAM_START] = 0xFF;
    }
    
//    // Mario Land 2 - Requires cart RAM not to be all 0s
//    for (int b = 0; b < 4; b++)
//        for (i = 0x0000; i < 0x2000; i++)
//            gb->cart_ram[b][i] = 0xFF;

    // Mario Land 2 - Requires cart RAM not to be all 0s
    for(i = 0x0000; i < 0x8000; i++)
        gb->cart_ram[i] = 0xFF;
    
    // Map the cart
//    uint16_t saddr = 0x0000;
//    uint16_t daddr = 0x0000;
//    for(i = 0; i < 0x8000; i++) {  // FIXME
//        //gb->mem[daddr++] = gb->cart[saddr++];
//        gb->mem[daddr++] = gb->cart_rom[saddr++];
//    }
    
    gb->joypad->state = 0xFF;
    
    /* GPU */
    gb->gpu->lx = 0;
    gb->gpu->bg_data = 0;
    gb->gpu->win_data = 0;
    gb->gpu->obj_data = 0;
    gb->gpu->num_obj = 0;
    gb->gpu->bg_idx = 0;
    gb->gpu->bg_color = 0;
    gb->gpu->obj_idx = 0;
    gb->gpu->obj_color = 0;
    gb->gpu->obj_priority = 0;

    /* APU */
    gb->apu->channel_1->length_count = 0;
    gb->apu->channel_1->volume_count = 0;
    gb->apu->channel_1->square_count = 0;
    gb->apu->channel_1->volume = 0;
    gb->apu->channel_1->length = 0;
    gb->apu->channel_1->length_enable = 0;
    gb->apu->channel_1->frequency = 0;
    gb->apu->channel_1->disabled = 0;
    gb->apu->channel_1->volume_mode = 0;
    gb->apu->channel_1->volume_steps = 0;
    gb->apu->channel_1->volume_step_count = 0;
    gb->apu->channel_1->duty = 0;
    gb->apu->channel_1->dp = 0;
    gb->apu->channel_1->recalc = 0;
    gb->apu->channel_1->addr = NR10;

    gb->apu->channel_2->length_count = 0;
    gb->apu->channel_2->volume_count = 0;
    gb->apu->channel_2->square_count = 0;
    gb->apu->channel_2->volume = 0;
    gb->apu->channel_2->length = 0;
    gb->apu->channel_2->length_enable = 0;
    gb->apu->channel_2->frequency = 0;
    gb->apu->channel_2->disabled = 0;
    gb->apu->channel_2->volume_mode = 0;
    gb->apu->channel_2->volume_steps = 0;
    gb->apu->channel_2->volume_step_count = 0;
    gb->apu->channel_2->duty = 0;
    gb->apu->channel_2->dp = 0;
    gb->apu->channel_2->recalc = 0;
    gb->apu->channel_2->addr = NR20;
    
    gb->apu->channel_3->addr = NR30;
    gb->apu->channel_4->addr = NR40;

//    gb->apu->channel_1->sweep_count = SWEEP_TIMER;
//    gb->apu->channel_1->length_count = LENGTH_PERIOD;
//    gb->apu->channel_1->volume_count = VOLUME_PERIOD;
    
    gb->ime = 0;
    gb->ime_delay = 0;
    gb->clock_stopped = 0;
    gb->oscillator_stopped = 0;
    
    gb->clocks_total = 0;
    gb->clocks_lcd = 0;
    gb->clocks_apu = 0;
    gb->clocks_div = 0;
    gb->clocks_tima = 0;
    gb->clocks_dma = 0;
    gb->clocks_serial = 0;
    
    gb->rom_bank = 1;
    gb->ram_bank = 1;
    gb->bank_mode = 0;
    gb->ram_bank_enable = 0;
    
    gb->dma = 0;
    gb->serial = 0;
    
    gb->cpu->PC = 0;
    gb->cpu->SP = STACK_START;
    gb->cpu->AF = 0x01B0;
    gb->cpu->BC = 0x0013;
    gb->cpu->DE = 0x00D8;
    gb->cpu->HL = 0x014D;
    
    gb->mem[LCDC] = 0x91;
    gb->mem[STAT] = 0x85;
    gb->mem[LY] = 0x00;
    gb->mem[LYC] = 0x00;
    gb->mem[SCX] = 0x00;
    gb->mem[SCY] = 0x00;
    gb->mem[WX] = 0x00;
    gb->mem[WY] = 0x00;
    gb->mem[BGP] = 0xFC;
    gb->mem[OBP0] = 0xFF;
    gb->mem[OBP1] = 0xFF;

    gb->mem[NR10] = 0x80;
    gb->mem[NR11] = 0xBF;
    gb->mem[NR12] = 0xF3;
    gb->mem[NR14] = 0xBF;
    gb->mem[NR21] = 0x3F;
    gb->mem[NR22] = 0x00;
    gb->mem[NR24] = 0xBF;
    gb->mem[NR30] = 0x7F;
    gb->mem[NR31] = 0xFF;
    gb->mem[NR32] = 0x9F;
    gb->mem[NR33] = 0xBF;
    gb->mem[NR41] = 0xFF;
    gb->mem[NR42] = 0x00;
    gb->mem[NR43] = 0x00;
    gb->mem[NR30] = 0xBF;
    gb->mem[NR50] = 0x77;
    gb->mem[NR51] = 0xF3;
    
    gb->mem[P1] = 0xC7;
    gb->mem[SB] = 0x00;
    gb->mem[SC] = 0x7E;
    gb->mem[IF] = 0xE1;
    gb->mem[IE] = 0x00;
    
    gb->mem[TIMA] = 0x00;
    gb->mem[TMA] = 0x00;
    gb->mem[TAC] = 0xF8;
}


void cpu_log_op(struct gameboy *gb, struct op *op) {
    printf("%04X: ", gb->cpu->PC);
    if (op->opcode >= 0xCB00) {
        printf("CB ");
        printf("%02X ", op->opcode & 0xFF);
    } else {
        printf("%02X ", op->opcode);
        if (op->op_def->bytes == 2)
            printf("%02X ", op->immediate.byte);
        else if (op->op_def->bytes == 3)
            printf("%02X %02X", op->immediate.lo, op->immediate.hi);
    }
    
    printf("\t%-10s", op->op_def->mnemonic);
}

void cpu_log_regs(struct gameboy *gb) {
    printf("\tAF: %04X BC: %04X DE: %04X HL: %04X SP: %04X ", gb->cpu->AF, gb->cpu->BC, gb->cpu->DE, gb->cpu->HL, gb->cpu->SP);
    printf("ZNHC: %d%d%d%d ", gb->cpu->flags.Z, gb->cpu->flags.N, gb->cpu->flags.H, gb->cpu->flags.C);
    //printf("(HL): %02X, IF: %02X, LY: %02X LYC: %02X STAT: %02X ", mem_read_u8(gb, gb->cpu->HL), gb->mem[IF], gb->mem[LY], gb->mem[LYC], gb->mem[STAT]);
    printf("LY: %02X LYC: %02X STAT: %02X c_lcd %03d rom %X ram %X IF %02X IE %02X\n", gb->mem[LY], gb->mem[LYC], gb->mem[STAT], gb->clocks_lcd, gb->rom_bank, gb->ram_bank, gb->mem[IF],gb->mem[IE]);
}

void print_mem(struct gameboy *gb) {
    printf("HRAM:\n");
    for(uint32_t g = HRAM_START; g <= HRAM_END; g++) {
        printf("%02X ",gb->mem[g]);
        if (g % 0x10 == 0xF)
            printf("\n");
    }
    printf("\n");
    
    // Background printed as 32 x 32 map
    printf("BACKGROUND_MAP0:\n");
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            printf("%02X ",gb->mem[BACKGROUND_MAP0 + (y * 32) + x]);
        }
        printf("\n");
    }
    
    printf("AF: 0x%04X BC: 0x%04x DE: 0x%04x HL: 0x%04x (HL): 0x%02x", gb->cpu->AF, gb->cpu->BC, gb->cpu->DE, gb->cpu->HL, gb->mem[gb->cpu->HL]);
    printf("\tZNHC: %d%d%d%d", gb->cpu->flags.Z, gb->cpu->flags.N, gb->cpu->flags.H, gb->cpu->flags.C);
    
    printf("\njoypad: %02x -> %02x\n", gb->joypad->state, mem_read_u8(gb, P1));
    
    printf("STAT: %02X\n", mem_read_u8(gb, STAT));
    printf("LCDC: %02X\n", mem_read_u8(gb, LCDC));
    printf("LY:   %02X\n", mem_read_u8(gb, LY));
    printf("LYC:  %02X\n", mem_read_u8(gb, LYC));
    printf("SCX:  %02X\n", mem_read_u8(gb, SCX));
    printf("SCY:  %02X\n", mem_read_u8(gb, SCY));
    printf("BGP:  %02X\n", mem_read_u8(gb, BGP));
    printf("IF:   %02X\n", mem_read_u8(gb, IF));
    printf("TIMA: %02X\n", mem_read_u8(gb, TIMA));
    printf("TMA:  %02X\n", mem_read_u8(gb, TMA));
    printf("TAC:  %02X\n", mem_read_u8(gb, TAC));
    printf("DIV:  %02X\n", mem_read_u8(gb, DIV));
    
    printf("ime  %d\n", gb->ime);
    printf("lcd clocks  %d\n", gb->clocks_lcd);
    printf("div clocks %d\n", gb->clocks_div);
    printf("tima clocks %d\n", gb->clocks_tima);
    printf("rom/ram bank %d %d\n", gb->rom_bank, gb->ram_bank);
    
}

