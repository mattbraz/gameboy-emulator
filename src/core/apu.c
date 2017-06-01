#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "gameboy.h"

//#define ENVELOPE_TIMER_PERIOD
#define VOLUME_PERIOD   65536
#define LENGTH_PERIOD   16384
#define SWEEP_TIMER     32768

uint8_t base[] = {8, 16, 32, 48, 64, 80, 96, 112};
uint8_t wave_volume_shift[] = {4, 0, 1, 2};
int duty_waveform[4][8] = {
    {0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,1},
    {0,1,1,1,1,1,1,0}
};

/* Sound buffers */
uint8_t apu_buffer[4][SAMPLES_PER_CALLBACK] = {{0}};
/* Buffer position */
static int bp = 0;

int sweep_count = SWEEP_TIMER;
int volume_count = VOLUME_PERIOD;
int length_count = LENGTH_PERIOD;

//void clock_length(struct gameboy *gb, struct channel5 *ch, struct _regs *_regs);
//void clock_envelope(struct gameboy *gb, struct channel5 *ch, struct _regs *_regs);
//void clock_sweep(struct gameboy *gb, struct channel5 *ch, struct _regs *_regs);
//uint8_t square(struct gameboy *gb, struct channel5 *ch, struct _regs *_regs);
//uint8_t noise(struct gameboy *gb, struct channel5 *ch, struct _regs *_regs);

static void clock_envelope(struct gameboy *gb, struct channel *ch);
static void clock_length(struct gameboy *gb, struct channel *ch);
static void clock_sweep(struct gameboy *gb, struct channel *ch);
static uint8_t noise(struct gameboy *gb, struct channel *ch);
static uint8_t square(struct gameboy *gb, struct channel *ch);
static uint8_t wave(struct gameboy *gb, struct channel *ch);

/* This is a god-awful mess and shouldn't exist. */
void apu_trigger2(struct gameboy *gb, struct channel *ch, int n) {
    uint16_t nrx0_addr, nrx1_addr, nrx2_addr, nrx3_addr, nrx4_addr;
    
    if (n == 1) {
        nrx0_addr = NR10;
        nrx1_addr = NR11;
        nrx2_addr = NR12;
        nrx3_addr = NR13;
        nrx4_addr = NR14;
    } else if(n == 2) {
        nrx0_addr = NR20;
        nrx1_addr = NR21;
        nrx2_addr = NR22;
        nrx3_addr = NR23;
        nrx4_addr = NR24;
    } else if(n == 3) {
        nrx0_addr = NR30;
        nrx1_addr = NR31;
        nrx2_addr = NR32;
        nrx3_addr = NR33;
        nrx4_addr = NR34;
        
        ch->dp = 0;
    } else if(n == 4) {
        nrx0_addr = NR40;
        nrx1_addr = NR41;
        nrx2_addr = NR42;
        nrx3_addr = NR43;
        nrx4_addr = NR44;
        
        ch->lfsr = 0x7FFF;
    } else {
        exit(1);
    }
    
    if (n == 3) {
        ch->length = mem_read_u8(gb, nrx1_addr);
        ch->length = 256 - ch->length;
        ch->square_count = (2048 - ch->frequency) * 2;
    } else {
        ch->length = mem_read_u8(gb, nrx1_addr) & 0x3F;
        ch->length = 64 - ch->length;
        ch->duty = (mem_read_u8(gb, nrx1_addr) & 0xC0) >> 6;
        ch->square_count = (2048 - ch->frequency) * 4;
    }

    ch->volume = (mem_read_u8(gb, nrx2_addr) & 0xF0) >> 4;
    ch->volume_mode = (mem_read_u8(gb, nrx2_addr) & 0x8) >> 3;
    ch->volume_steps = mem_read_u8(gb, nrx2_addr) & 0x7;
    ch->shadow_vol_steps = ch->volume_steps;
    ch->shadow_length = ch->length;
    ch->length_enable = (mem_read_u8(gb, nrx4_addr) & 0x40) >> 6;
    
    uint8_t lo = mem_read_u8(gb, nrx3_addr);
    uint8_t hi = mem_read_u8(gb, nrx4_addr);
    ch->frequency = ((hi & 0x07) << 8) | lo;
    
    ch->sweep_period = (mem_read_u8(gb, nrx0_addr) & 0x70) >> 4;
    ch->sweep_negate = (mem_read_u8(gb, nrx0_addr) & 0x8) >> 3;
    ch->sweep_shift = mem_read_u8(gb, nrx0_addr) & 0x7;
    ch->sweep_count = SWEEP_TIMER;
    
    ch->disabled = 0;
    
    ch->length_count = LENGTH_PERIOD;
    ch->volume_count = VOLUME_PERIOD;
    ch->volume_step_count = ch->volume_steps * 65536;
}


static void clock_envelope(struct gameboy *gb, struct channel *ch) {
    struct _regs *r = (struct _regs *) &gb->mem[ch->addr];
    if (ch->shadow_vol_steps && --ch->shadow_vol_steps == 0) {
        ch->volume += r->envelope.mode ? 1 : -1;
        if (ch->volume > 15) ch->volume = 15;
        if (ch->volume < 0) ch->volume = 0;
        ch->shadow_vol_steps = r->envelope.period;
    }
}

static void clock_length(struct gameboy *gb, struct channel *ch) {
    if (ch->length_enable) {
        if (ch->length && --ch->length == 0) {
            ch->disabled = 1;
        }
    }
}

static void clock_sweep(struct gameboy *gb, struct channel *ch) {
    if (ch->sweep_period && --ch->sweep_period == 0) {
        int delta = ch->frequency >> ch->sweep_shift;
        ch->frequency += ch->sweep_negate ? -delta : delta;
        if(ch->frequency > 2047)
            ch->disabled = 1;
        ch->sweep_period = ch->sweep_period;
        
        mem_write_u8(gb, ch->addr + 3, (ch->frequency >> 0) & 0xFF);
        mem_write_u8(gb, ch->addr + 4, (ch->frequency >> 8) & 0x07);
    }
}

static uint8_t square(struct gameboy *gb, struct channel *ch) {
    if (ch->square_count && --ch->square_count == 0) {
        ch->dp++;
        ch->dp %= 8;
        uint8_t lo = mem_read_u8(gb, ch->addr + 3);
        uint8_t hi = mem_read_u8(gb, ch->addr + 4);
        /* Only 3 bits of hi */
        int frequency = ((hi & 0x7) << 8) | lo;
        ch->square_count = (2048 - frequency) * 4;
        //ch->square_count = (2048 - ch->frequency) * 4;
    }
    if (ch->disabled) ch->volume = 0;
    return duty_waveform[ch->duty][ch->dp] ? ch->volume : 0;
}

static uint8_t wave(struct gameboy *gb, struct channel *ch) {
    if (ch->square_count && --ch->square_count == 0) {
        ch->disabled = !(mem_read_u8(gb, ch->addr) & 0x80);
        uint8_t lo = mem_read_u8(gb, ch->addr + 3);
        uint8_t hi = mem_read_u8(gb, ch->addr + 4);
        /* Only 3 bits of hi */
        int frequency = ((hi & 0x7) << 8) | lo;
        ch->square_count = (2048 - frequency) * 2;
        
        int x = ch->dp >> 1;
        int sample = gb->mem[WAVE_TABLE + x];
        sample &= (ch->dp % 2 ? 0x0F : 0xF0);
        sample >>= (ch->dp % 2 ? 0 : 4);
        
        int volume_shift_code = (mem_read_u8(gb, ch->addr + 2) >> 5) & 0x3;

        ch->wave_buf = sample >> wave_volume_shift[volume_shift_code];
        ch->dp = (ch->dp + 1) % 32;
    }
    
    if (ch->disabled)
        return 0;
   
    return ch->wave_buf;
}

static uint8_t noise(struct gameboy *gb, struct channel *ch) {
    struct _regs *r = (struct _regs *) &gb->mem[ch->addr];
    if (ch->square_count && --ch->square_count == 0) {
        uint8_t bit = (ch->lfsr & 0x1) ^ ((ch->lfsr & 0x2) >> 1);
        ch->lfsr >>= 0x1;
        ch->lfsr ^= (bit << 14);
        
        if(r->noise.width_mode)
            ch->lfsr ^= (bit << 6);
        ch->square_count = base[r->noise.divisor_code] << r->noise.clock_shift;
    }
    if (ch->disabled) ch->volume = 0;
    return (ch->lfsr & 0x1) ? 0 : ch->volume;
}

/* Called by the SDL sound callback */
void apu_upd(struct gameboy *gb, int clocks, int clocks_per_sample) {
    uint8_t sample1 = 0;
    uint8_t sample2 = 0;
    uint8_t sample3 = 0;
    uint8_t sample4 = 0;
    
    struct channel *c1 = gb->apu->channel_1;
    struct channel *c2 = gb->apu->channel_2;
    struct channel *c3 = gb->apu->channel_3;
    struct channel *c4 = gb->apu->channel_4;
    //struct channel *ch = c1;
    
    if (c1->recalc) {
        apu_trigger2(gb, c1, 1);
        c1->recalc = 0;
    }
    if (c2->recalc) {
        apu_trigger2(gb, c2, 2);
        c2->recalc = 0;
    }
    if (c3->recalc) {
        apu_trigger2(gb, c3, 3);
        c3->recalc = 0;
    }
    if (c4->recalc) {
        apu_trigger2(gb, c4, 4);
        c4->recalc = 0;
    }
    
    //int bp = 0;
    //int clocks = 0;
    //while (clocks < CLOCKS_PER_CALL) {
    while (clocks--) {
        sample1 = square(gb, c1);
        sample2 = square(gb, c2);
        sample3 = wave(gb, c3);
        sample4 = noise(gb, c4);
        
        if (length_count && --length_count == 0) {
            clock_length(gb, c1);
            clock_length(gb, c2);
            clock_length(gb, c3);
            clock_length(gb, c4);
            length_count = LENGTH_PERIOD;
        }
        if (sweep_count && --sweep_count == 0) {
            clock_sweep(gb, c1);
            sweep_count = SWEEP_TIMER;
        }
        if (volume_count && --volume_count == 0) {
            clock_envelope(gb, c1);
            clock_envelope(gb, c2);
            clock_envelope(gb, c4);
            volume_count = VOLUME_PERIOD;
        }
        
        gb->clocks_apu += 1;
        if (gb->clocks_apu % clocks_per_sample == 0) {
            gb->clocks_apu = 0;

            /* Both L and R must be enabled. */
            sample1 = mem_read_u8(gb, NR51) & 0x11 ? sample1 : 0;
            sample2 = mem_read_u8(gb, NR51) & 0x22 ? sample2 : 0;
            sample3 = mem_read_u8(gb, NR51) & 0x44 ? sample3 : 0;
            sample4 = mem_read_u8(gb, NR51) & 0x88 ? sample4 : 0;
            
            /* Average of L and R master volume */
            int vol = (mem_read_u8(gb, NR50) & 0x70) >> 4;
            vol += mem_read_u8(gb, NR50) & 0x7;
            vol >>= 1;
            vol ++;
            
            sample1 = (sample1 * vol) >> 2;
            sample2 = (sample2 * vol) >> 2;
            sample3 = (sample3 * vol) >> 2;
            sample4 = (sample4 * vol) >> 2;
            
            apu_buffer[0][bp] = sample1;
            apu_buffer[1][bp] = sample2;
            apu_buffer[2][bp] = sample3;
            apu_buffer[3][bp] = sample4;

            bp += 1;
            bp %= SAMPLES_PER_CALLBACK;
        }
    }
}
