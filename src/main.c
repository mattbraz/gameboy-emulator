#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <getopt.h>
#include <libgen.h>
#include "gameboy.h"

struct gameboy gameboy;

extern int logging;

/*
 * TO DO
 * -----
 * Clear the sound buffer when resetting
 * ROM bank write range checking (when switching rom banks)
 * FIXME: can we have interrupts doing DMA?
 * interrupt_time.gb fails in dmg mode in bgb too. works in cgb mode
 * MEMORY
 *  Why does 2d rom array break mario 2? - level 2 , left path
 * GRAPHICS
 *  V-ram read during modes broken
 *  prehistoric-man - sprite priority broken
 *  q-tsu - round blobs disappear/re-appear
 *  opus - scrolling still tears
 * SOUND
 *  super-mario - jump and hit block sound too long
 *  prehistoric-man - sound is very broken
 * OTHER
 *  alleyway - controls during bonus round. also serial port issues
*/

int main(int argc, char **argv) {
    char *file_path = NULL;
    extern char *optarg;
    extern int optind, opterr, optopt;
    int c;
    
    opterr = 0;
    while ((c = getopt(argc, argv, "f:l")) != -1)
        switch (c) {
            case 'l':
                logging = 1;
                break;
            case 'f':
                file_path = optarg;
                break;
            case '?':
                if (optopt == 'f')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                exit(1);
            default:
                abort();
        }
    if (file_path == NULL) {
        printf("Please pass a ROM file with -f\n");
        exit(1);
    }
    printf("logging = %d, file_path = %s\n", logging, file_path);

    
    struct gameboy *gb;
    gb = gb_create();
    gb_reset(gb);

    if  (!read_rom(gb, file_path)) {
        printf("Failed to run emulator\n");
        exit(1);
    }
    // if (gb->cart_rom[CART_TYPE] > 0x03) {
    //     printf("Failed to run emulator - unsupported memory banking\n");
    //     exit(1);
    // }

    sdl_init(gb, basename(file_path));
    gb_reset(gb);

    gb_run(gb);

    // while(1) {
    //     gb_main_new(gb);
    //     sdl_main(gb);
    // }
        
}
