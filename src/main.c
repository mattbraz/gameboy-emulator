#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include "gameboy.h"

struct gameboy gb;

extern int paused;


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

    /* Only command line arg is a path to a rom file */
    if (argc < 2) {
        printf("usage: %s rom_file\n", argv[0]);
        exit(1);
    }
    file_path = argv[1];

    /* Set up the core */
    //struct gameboy gb;
    gb_alloc_mem(&gb);
    gb_init(&gb, 0, 0);
    gb_reset(&gb);

    if  (!load_rom(&gb, file_path)) {
        printf("Failed to run emulator\n");
        exit(1);
    }

    /* Initiate the front end */
    sdl_init_video(&gb, basename(file_path));
    sdl_init_audio(&gb);

    /* method #1 */
    // gb_set_vbl_callback(gb, sdl_video_callback);
    // gb_run(gb);

    /* method #2 */
    while(1) {
        if (!paused)
            gb_run_frames(&gb, 1);
        sdl_video_main(&gb);
        sdl_events(&gb);
    }

    return 0;
}

