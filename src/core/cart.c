#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "gameboy.h"

struct cart_type {
    int idx;
    char *desc;
};

struct cart_type cart_types[] = {
    { 0x00, "ROM ONLY" },
    { 0x01, "MBC1" },
    { 0x02, "MBC1+RAM" },
    { 0x03, "MBC1+RAM+BATTERY" },
    { 0x05, "MBC2" },
    { 0x06, "MBC2+BATTERY" },
    { 0x08, "ROM+RAM" },
    { 0x09, "ROM+RAM+BATTERY" },
    { 0x0B, "MMM01" },
    { 0x0C, "MMM01+RAM" },
    { 0x0D, "MMM01+RAM+BATTERY" },
    { 0x0F, "MBC3+TIMER+BATTERY" },
    { 0x10, "MBC3+TIMER+RAM+BATTERY" },
    { 0x11, "MBC3" },
    { 0x12, "MBC3+RAM" },
    { 0x13, "MBC3+RAM+BATTERY" },
    { 0x15, "MBC4" },
    { 0x16, "MBC4+RAM" },
    { 0x17, "MBC4+RAM+BATTERY" },
    { 0x19, "MBC5" },
    { 0x1A, "MBC5+RAM" },
    { 0x1B, "MBC5+RAM+BATTERY" },
    { 0x1C, "MBC5+RUMBLE" },
    { 0x1D, "MBC5+RUMBLE+RAM" },
    { 0x1E, "MBC5+RUMBLE+RAM+BATTERY" },
    { 0xFC, "POCKET CAMERA" },
    { 0xFD, "BANDAI TAMA5" },
    { 0xFE, "HuC3" },
    { 0xFF, "HuC1+RAM+BATTERY" }
};

int rom_size[] = {
    0x00008000,
    0x00010000,
    0x00020000,
    0x00040000,
    0x00080000,
    0x00100000,
    0x00200000,
    0x00400000
};
int ram_size[] = { 0x0, 0x400, 0x2000, 0x8000 };

static struct cart_type *get_rom_type_def(int idx);

int read_rom(struct gameboy *gb, const char *file_path) {
    FILE *file;
    long file_size;
    
    file = fopen(file_path, "rb");
    if(file == NULL) {
        perror("Error");
        exit(1);
    }
    
    /* Get file size */
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    
    /* Check it looks vaguely legit */
    if (file_size > 0x400000 || file_size < 0x8000) {
        printf("Unsupported file size: %lu\n", file_size);
        return 0;
    }

    /* Read the entire ROM */
    fseek(file, 0, SEEK_SET);
    size_t n = fread(gb->cart_rom, file_size, 1, file);
    if (!n) {
        printf("Error: failed to read ROM file\n");
        exit(1);
    }
    fclose(file);
    
    /* Check the file size is the same as the ROM size */
    if (rom_size[gb->cart_rom[CART_SIZE]] != file_size) {
        printf("File size does not match ROM size\n");
        return 0;
    }
    
    /* Check Cart type */
    struct cart_type *ct;
    if ((ct = get_rom_type_def(gb->cart_rom[CART_TYPE])) != NULL) {
        if (gb->cart_rom[CART_TYPE] > 0x03) {
            printf("Unsupported cart type: %d\n", ct->idx);
            return 0;
        }
        printf("Cart type: %s\n", ct->desc);
    } else {
        printf("Unsupported cart type: %d\n", gb->cart_rom[CART_TYPE]);
        return 0;
    }
    
    /* FIXME: check bounds */
    printf("ROM size: %d\n", rom_size[gb->cart_rom[CART_SIZE]]);
    printf("RAM size: %d\n", ram_size[gb->cart_rom[RAM_SIZE]]);

    return 1;
}

static struct cart_type *get_rom_type_def(int idx) {
    struct cart_type *rtd = NULL;
    for (int i = 0; i < sizeof(cart_types) / sizeof(struct cart_type); i++) {
        if (idx == cart_types[i].idx)
            rtd = &cart_types[i];
    }
    return rtd;
}
