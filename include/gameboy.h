#ifndef _DMG_H_
#define _DMG_H_

/* For the internal button press struct, not the gb memory */
#define BUTTON_A        0x1 << 0
#define BUTTON_B        0x1 << 1
#define BUTTON_SELECT   0x1 << 2
#define BUTTON_START    0x1 << 3
#define BUTTON_RIGHT    0x1 << 4
#define BUTTON_LEFT     0x1 << 5
#define BUTTON_UP       0x1 << 6
#define BUTTON_DOWN     0x1 << 7

#define DMG_MHZ             4194304

/* Windows apparently will not do > 100 callbacks per second.. */
#define SAMPLES_PER_SEC         65536
#define SAMPLES_PER_CALLBACK    1024

#define SCREEN_WIDTH    160
#define SCREEN_HEIGHT   144

#define CART_ROM_START  0x0000
#define CART_ROM_END    0x7FFF
#define VRAM_START      0x8000
#define VRAM_END        0x9FFF
#define CART_RAM_START  0xA000
#define CART_RAM_END    0xBFFF
#define WRAM_START      0xC000
#define WRAM_END        0xDFFF
#define ECHO_START      0xE000
#define ECHO_END        0xFDFF
#define OAM_START       0xFE00
#define OAM_END         0xFE9F
#define UNUSED_START    0xFEA0
#define UNUSED_END      0xFEFF
#define REGS_START      0xFF00
#define REGS_END        0xFF7F
#define HRAM_START      0xFF80
#define HRAM_END        0xFFFE

#define ROM_BANK        0x4000
#define RAM_BANK        0xA000

#define STACK_START     0xFFFE
#define CHARACTER_DATA  0x8000
#define BACKGROUND_MAP0 0x9800
#define BACKGROUND_MAP1 0x9C00
#define OAM             0xFE00

#define CART_TITLE      0x0134
#define CART_TYPE       0x0147
#define CART_SIZE       0x0148
#define RAM_SIZE        0x0149

/* Registers */
#define P1      0xFF00
#define SB      0xFF01
#define SC      0xFF02
#define DIV     0xFF04
#define TIMA    0xFF05
#define TMA     0xFF06
#define TAC     0xFF07

#define IE      0xFFFF
#define IF      0xFF0F

#define NR10    0xFF10
#define NR11    0xFF11
#define NR12    0xFF12
#define NR13    0xFF13
#define NR14    0xFF14
#define NR20    0xFF15
#define NR21    0xFF16
#define NR22    0xFF17
#define NR23    0xFF18
#define NR24    0xFF19
#define NR30    0xFF1A
#define NR31    0xFF1B
#define NR32    0xFF1C
#define NR33    0xFF1D
#define NR34    0xFF1E
#define NR40    0xFF1F
#define NR41    0xFF20
#define NR42    0xFF21
#define NR43    0xFF22
#define NR44    0xFF23
#define NR50    0xFF24
#define NR51    0xFF25
#define NR52    0xFF26

#define WAVE_TABLE  0xFF30

#define LCDC    0xFF40
#define STAT    0xFF41
#define SCY     0xFF42
#define SCX     0xFF43
#define LY      0xFF44
#define LYC     0xFF45
#define DMA     0xFF46
#define BGP     0xFF47
#define OBP0    0xFF48
#define OBP1    0xFF49
#define WY      0xFF4A
#define WX      0xFF4B

/* Interrupts */
#define IRQ_VBLANK  (0x1 << 0)
#define IRQ_STAT    (0x1 << 1)
#define IRQ_TIMER   (0x1 << 2)
#define IRQ_SERIAL  (0x1 << 3)
#define IRQ_JOYP    (0x1 << 4)

#define IRQ_VBLANK_JUMP  0x0040
#define IRQ_STAT_JUMP    0x0048
#define IRQ_TIMER_JUMP   0x0050
#define IRQ_SERIAL_JUMP  0x0058
#define IRQ_JOYP_JUMP    0x0060

/* Graphics */
#define LCDC_LCD_ENABLE (0x1 << 7)
#define LCDC_WIN_MAP    (0x1 << 6)
#define LCDC_WIN_ENABLE (0x1 << 5)
#define LCDC_TILE_DATA  (0x1 << 4)
#define LCDC_BG_MAP     (0x1 << 3)
#define LCDC_OBJ_SIZE   (0x1 << 2)
#define LCDC_OBJ_ENABLE (0x1 << 1)
#define LCDC_BG_ENABLE  (0x1 << 0)

#define STAT_MODE       (0x3 << 0)
#define STAT_LYC_MATCH  (0x1 << 2)
#define STAT_MODE_0_IRQ (0x1 << 3)
#define STAT_MODE_1_IRQ (0x1 << 4)
#define STAT_MODE_2_IRQ (0x1 << 5)
#define STAT_LYC_IRQ    (0x1 << 6)

// FIXME: pick a magic number....
#define LCD_CLOCKS_START    424 // 456 - 32
#define DMA_CLOCKS_TOTAL    671

struct gameboy;

typedef void (*vbl_func_ptr)(struct gameboy *);
typedef void (*mhz_func_ptr)(struct gameboy *);

struct op {
    uint16_t        opcode;
    struct op_def   *op_def;
    union {
        uint8_t     byte;
        uint16_t    word;
        struct {
            uint8_t lo;
            uint8_t hi;
        };
    } immediate;
};


typedef void (*op_func_ptr)(struct gameboy *, struct op *);

struct op_def {
    char        *mnemonic;
    uint8_t     bytes;
    op_func_ptr op_func_ptr;
};

struct flags {
    uint8_t blank   :4;
    uint8_t C       :1;
    uint8_t H       :1;
    uint8_t N       :1;
    uint8_t Z       :1;
};

struct cpu {
    /* Accumulator and Flag Register */
    union {
        uint16_t AF;
        struct {
            union {
                uint8_t F;
                struct flags flags;
            };
            uint8_t A;
        };
    };
    /* Auxiliary Registers */
    union {
        uint16_t BC;
        struct {
            uint8_t C;
            uint8_t B;
        };
    };
    union {
        uint16_t DE;
        struct {
            uint8_t E;
            uint8_t D;
        };
    };
    union {
        uint16_t HL;
        struct {
            uint8_t L;
            uint8_t H;
        };
    };
    /* Program Counter */
    uint16_t PC;
    /* Stack Pointer */
    uint16_t SP;

};

/* Convenience structure for the PPU */
struct obj {
    uint8_t y;
    uint8_t x;
    uint8_t tidx;
    struct {
        uint8_t cgb_pal :3;
        uint8_t tbank   :1;
        uint8_t dmg_pal :1;
        uint8_t flipx   :1;
        uint8_t flipy   :1;
        uint8_t priority:1;
    };
};

struct gpu {
    /* Current x pixel */
    uint8_t lx;
    /* Tile plane data */
    uint16_t bg_data;
    uint16_t win_data;
    uint16_t obj_data;
    /* Max 10 sprites on current line */
    int num_obj;
    struct obj *obj[10];
    /* Pixel attributes */
    uint8_t bg_idx;
    uint8_t bg_color;
    uint8_t obj_idx;
    uint8_t obj_color;
    uint8_t obj_priority;
    /* Pixel data */
    uint32_t pixel_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];
};

struct channel {
    int length_count;
    int volume_count;
    int square_count;
    int sweep_count;
    int volume;
    int length;
    int length_enable;
    int frequency;
    int disabled;
    int volume_mode;
    int volume_steps;
    int volume_step_count;
    int duty;
    int dp;

    int shadow_vol_steps;
    int shadow_length;
    
    int sweep_period;
    int sweep_negate;
    int sweep_shift;
    
    int recalc;
    
    int wave_buf;
    uint16_t lfsr;

    uint16_t addr;
};

struct apu {
    struct channel *channel_1;
    struct channel *channel_2;
    struct channel *channel_3;
    struct channel *channel_4;

    // struct channel5 square1;
    // struct channel5 square2;
    // struct channel5 wave;
    // struct channel5 noise;
};

/* EXPERIMENTAL APU STRUCTURES */
struct sweep {
    uint8_t blank   :1;
    uint8_t period  :3;
    uint8_t negate  :1;
    uint8_t shift   :3;
};
struct duty_length {
    uint8_t length  :6;
    uint8_t duty    :2;
};
struct envelope {
    uint8_t period  :3;
    uint8_t mode    :1;
    uint8_t start   :4;
};
//struct wave_volume { backwards
//    uint8_t blank1      :1;
//    uint8_t volume_code :2;
//    uint8_t blank2      :5;
//};
struct frequency {
    uint16_t freq       :11;
    uint16_t blank       :3;
    uint16_t len_enab    :1;
    uint16_t trigger     :1;
};
struct noise {
//    uint8_t trigger         :1;
//    uint8_t length_enab     :1;
//    uint8_t blank           :3;
    uint8_t divisor_code    :3;
    uint8_t width_mode      :1;
    uint8_t clock_shift     :4;
};

struct _regs {
    union {
        struct sweep sweep;
        uint8_t f1;
    };
    union {
        struct duty_length duty_length;
        //uint8_t length;
        uint8_t f2;
    };
    union {
        struct envelope envelope;
        //struct wave_volume wave_volume;
        uint8_t f3;
    };
    union {
        uint8_t frequency_lsb;
        struct noise noise;
    };
    union {
        uint8_t f5;
        struct {
            uint16_t freq_msb    :3;
            uint16_t blank       :3;
            uint16_t len_enab    :1;
            uint16_t trigger     :1;
        } piss;
//        struct frequency frequency;
        //struct noise noise;
//        struct {
//            uint16_t trigger       :1;
//            uint16_t length_enable :1;
//            uint16_t blank        :14;
//        } other ;
//        struct {
//            uint8_t b1;
//            uint8_t b2;
//        };
    };
};


struct gameboy {
    struct cpu *cpu;
    struct gpu *gpu;
    struct apu *apu;
    uint8_t *mem;
    uint8_t boot_rom[0x100];
    
    //    0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
    //    4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
    uint8_t cart_rom[0x200000];
    //uint8_t cart_rom_bank[64][0x4000];
    
    /* 8 KB External RAM */
    //uint8_t cart_ram[4][0x2000];
    uint8_t cart_ram[0x8000];
    
    /* 8 KB Video RAM */
    uint8_t vram[0x2000];
    /* 8 KB Work RAM */
    uint8_t wram[0x2000];
    /* 128 bytes High RAM */
    uint8_t hram[0x80];
    /* Sprite Attribute Table (OAM) */
    uint8_t oam[0xA0];
    /* I/O Ports */
    uint8_t regs[0x80];

    /* Interrupt master enable flag */
    uint8_t ime;
    uint8_t ime_delay;
    
    /* ROM and RAM banking */
    uint8_t rom_bank;
    uint8_t ram_bank;
    uint8_t bank_mode;
    uint8_t ram_bank_enable;

    uint8_t dma;
    uint8_t serial;
    uint8_t oscillator_stopped;
    uint8_t clock_stopped;

    struct joypad *joypad;
    
    /* Clock counters */
    uint32_t clocks_lcd;
    uint32_t clocks_apu;
    uint32_t clocks_div;
    uint32_t clocks_tima;
    uint32_t clocks_dma;
    uint32_t clocks_serial;
    int clocks_total;

    /* */
    vbl_func_ptr vbl_callback;
    
};

struct joypad {
    uint8_t state;
};

void sdl_init(struct gameboy *gb, const char *rom_file);
void sdl_finish(struct gameboy *gb);
int sdl_video_main(struct gameboy *gb);
void sdl_events(struct gameboy *gb);
uint8_t *sdl_pixel_buffer();
void sdl_init_audio(struct gameboy *gb);
void sdl_init_video(struct gameboy *gb, const char *title);

void sdl_video_callback(struct gameboy *gb);

uint32_t *gb_pixel_buffer(struct gameboy *gb);

void gb_set_vbl_callback(struct gameboy *gb, vbl_func_ptr vbl_callback);

void gb_run(struct gameboy *gb);
void gb_run_clocks(struct gameboy *gb, unsigned int clocks);
void gb_run_ops(struct gameboy *gb, unsigned int ops);
void gb_run_frames(struct gameboy *gb, unsigned int frames);
void gb_stop(struct gameboy *gb);
void gb_close(struct gameboy *gb);

int read_rom(struct gameboy *gb, const char *file_path);

struct gameboy *gb_create();
void gb_free(struct gameboy *gb);
void gb_reset(struct gameboy *gb);
void gb_main_new(struct gameboy *gb);
void gb_once(struct gameboy *gb);
void gb_add_clocks(struct gameboy *gb, int clocks);
void gb_raise_irq(struct gameboy *gb, uint8_t irq_flag);
void gb_disable_irq(struct gameboy *gb, uint8_t irq_flag);
void gb_process_joypad(struct gameboy *gb);
void gb_process_dma(struct gameboy *gb,  int clocks);
void gb_process_serial(struct gameboy *gb, int clocks);
void gb_process_timers(struct gameboy *gb, int clocks);
void gb_process_interrupts(struct gameboy *gb);
void gb_joypad_press(struct gameboy *gb, uint8_t button);
void gb_joypad_release(struct gameboy *gb, uint8_t button);
void gb_get_title(struct gameboy *gb, char title[16]);

uint8_t mem_read_u8(struct gameboy *gb, uint16_t addr);
uint8_t mem_read_regs_u8(struct gameboy *gb, uint16_t addr);
uint8_t mem_read_vram_u8(struct gameboy *gb, uint16_t addr);
uint16_t mem_read_u16(struct gameboy *gb, uint16_t addr);
uint16_t mem_read_vram_u16(struct gameboy *gb, uint16_t addr);
void mem_write_u8(struct gameboy *gb, uint16_t addr, uint8_t value);
void mem_write_regs_u8(struct gameboy *gb, uint16_t addr, uint8_t value);
void mem_write_vram_u8(struct gameboy *gb, uint16_t addr, uint8_t value);
void mem_handle_banking(struct gameboy *gb, uint16_t addr, uint8_t value);

void lcd_set_stat_mode(struct gameboy *gb, uint8_t mode);
void lcd_copy_buf(struct gameboy *gb);
void lcd_line(struct gameboy *gb);
void lcd_pixel(struct gameboy *gb);
void lcd_dmg_pixel(struct gameboy *gb, int lx);
uint16_t lcd_read_tile_data(struct gameboy *gb, int map, int x, int y);
void lcd_start_line(struct gameboy *gb);
void lcd_bg(struct gameboy *gb, int lx);
void lcd_win(struct gameboy *gb, int lx);
void lcd_obj(struct gameboy *gb, int lx);
void lcd_upd(struct gameboy *gb, int clocks);
void lcd_check_lyc(struct gameboy *gb);

void apu_upd(struct gameboy *gb, int clocks, int clocks_per_sample);
void apu_trigger2(struct gameboy *gb, struct channel *ch, int n);
uint16_t apu_read_frequency(struct gameboy *gb, int channel);

void cpu_log_op(struct gameboy *gb, struct op *op);
void cpu_log_regs(struct gameboy *gb);
void print_mem(struct gameboy *gb);

#endif