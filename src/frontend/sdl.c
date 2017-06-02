#include <stdio.h>
#include "SDL2/SDL.h"
#include "gameboy.h"

void sdl_render();
void sdl_audio_callback(void *userdata, Uint8 *buf, int len);

extern int logging;
extern int paused;
extern int ppu_frames;
extern int CLOCKS_PER_SDL_FRAME;

extern uint8_t apu_buffer[4][SAMPLES_PER_CALLBACK];

static unsigned int millis = 0;
static unsigned int millis_prev = 0;
static unsigned int sdl_frames = 0;
static float fps = 0;
static float pfps = 0;

int chennel_enable = 0xF;

int clocks_per_call = 0;
int clocks_per_sample = 0;
int callbacks_per_sec = 0;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *surface = NULL;
SDL_Texture *texture = NULL;
SDL_AudioSpec *hardware_spec = NULL;


void sdl_init_video(struct gameboy *gb, const char *title) {
    window = SDL_CreateWindow("GAMEBOY", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        printf( "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == NULL) {
        printf( "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    
    surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if(surface == NULL) {
        printf( "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if(texture == NULL) {
        printf( "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetWindowTitle(window, title);

    //gb_set_video_callback();
}


void sdl_init_audio(struct gameboy *gb) {

    /* Audio Setup */
    SDL_AudioSpec *desired;
    desired = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
    hardware_spec = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
    if(desired == NULL || hardware_spec == NULL) {
        printf( "malloc error in AudioSpec\n");
        exit(1);
    }

    desired->freq = SAMPLES_PER_SEC;
    desired->format = AUDIO_U8;
    desired->channels = 1;
    desired->samples = SAMPLES_PER_CALLBACK;
    desired->callback = sdl_audio_callback;
    desired->userdata = gb;
    
    if (SDL_OpenAudio(desired, hardware_spec) < 0) {
        fprintf(stderr, "Couldn't open SDL audio: %s\n", SDL_GetError());
        exit(1);
    }
    free(desired);
    desired = NULL;

    printf("req: freq %d samples %d\n",SAMPLES_PER_SEC,SAMPLES_PER_CALLBACK);
    printf("got: freq %d samples %d\n",hardware_spec->freq, hardware_spec->samples);

    callbacks_per_sec = hardware_spec->freq / hardware_spec->samples;
    clocks_per_call = DMG_MHZ / callbacks_per_sec;
    clocks_per_sample = clocks_per_call / hardware_spec->samples;
    
    printf("callbacks_per_sec %d\n",callbacks_per_sec);
    printf("clocks_per_call %d\n",clocks_per_call);
    printf("clocks_per_sample %d\n",clocks_per_sample);

    SDL_PauseAudio(0);
}

void sdl_destroy() {
    printf("Destroying SDL\n");
    free(hardware_spec);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int sdl_video_main(struct gameboy *gb) {

    //sdl_render();
    //SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

    SDL_UpdateTexture(texture, NULL, gb_pixel_buffer(gb), SCREEN_WIDTH * 4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    sdl_frames++;
    millis = SDL_GetTicks();
    if (millis > millis_prev + 1000) {
        fps = sdl_frames / ((millis - millis_prev) / 1000.0);
        pfps = ppu_frames / ((millis - millis_prev) / 1000.0);
        
        // sprintf(title, "%s - %.0f %0.f", rom_file, fps, pfps);
        // SDL_SetWindowTitle(window, title);
        
        millis_prev = millis;
        sdl_frames = 0;
        ppu_frames = 0;
        //callbacks = 0;
    }

    return 1;
}

void sdl_video_callback(struct gameboy *gb) {

    //sdl_render();
    //SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

    SDL_UpdateTexture(texture, NULL, gb_pixel_buffer(gb), SCREEN_WIDTH * 4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    sdl_events(gb);
    
    sdl_frames++;
    millis = SDL_GetTicks();
    if (millis > millis_prev + 1000) {
        fps = sdl_frames / ((millis - millis_prev) / 1000.0);
        pfps = ppu_frames / ((millis - millis_prev) / 1000.0);
        
        // sprintf(title, "%s - %.0f %0.f", rom_file, fps, pfps);
        // SDL_SetWindowTitle(window, title);
        
        millis_prev = millis;
        sdl_frames = 0;
        ppu_frames = 0;
        //callbacks = 0;
    }
}

void sdl_audio_callback(void *userdata, Uint8 *buf, int len) {
    struct gameboy *gb = (struct gameboy *) userdata;
    if (paused) {
        SDL_memset(buf, 0, len);
        return;
    }

    apu_upd(gb, clocks_per_call, clocks_per_sample);

    /* Manual mixing, as SDL_MixAudio doesn't want to work on Windows */
    uint8_t sample = 0;
    for(int i = 0; i < len; i++) {
        sample = 0;
        if (chennel_enable & 0x1) sample += apu_buffer[0][i];
        if (chennel_enable & 0x2) sample += apu_buffer[1][i];
        if (chennel_enable & 0x4) sample += apu_buffer[2][i];
        if (chennel_enable & 0x8) sample += apu_buffer[3][i];
        if (sample > 255) sample = 255;
        buf[i] = sample;
    }
    //callbacks++;
    
//    SDL_memset(buf, 0, len);
//    SDL_MixAudio(buf, apu_buffer[0], len, SDL_MIX_MAXVOLUME);
//    SDL_MixAudio(buf, apu_buffer[1], len, SDL_MIX_MAXVOLUME);
//    SDL_MixAudio(buf, apu_buffer[2], len, SDL_MIX_MAXVOLUME);
//    SDL_MixAudio(buf, apu_buffer[3], len, SDL_MIX_MAXVOLUME);
}

void sdl_events(struct gameboy *gb) {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            sdl_destroy();
            gb_free(gb);
            exit(0);
        } else if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_F1:
                    chennel_enable ^= 0x1;
                    break;
                case SDLK_F2:
                    chennel_enable ^= 0x2;
                    break;
                case SDLK_F3:
                    chennel_enable ^= 0x4;
                    break;
                case SDLK_F4:
                    chennel_enable ^= 0x8;
                    break;
                    
                case SDLK_1:
                    SDL_SetWindowSize(window, SCREEN_WIDTH * 1, SCREEN_HEIGHT * 1);
                    break;
                case SDLK_2:
                    SDL_SetWindowSize(window, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
                    break;
                case SDLK_3:
                    SDL_SetWindowSize(window, SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3);
                    break;
                case SDLK_4:
                    SDL_SetWindowSize(window, SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4);
                    break;
                case SDLK_5:
                    SDL_SetWindowSize(window, SCREEN_WIDTH * 5, SCREEN_HEIGHT * 5);
                    break;

                case SDLK_p:
                    paused = paused ? 0 : 1;
                    //print_mem(gb);
                    break;
                case SDLK_l:
                    logging = (logging ? 0 : 1);
                    break;
                case SDLK_r:
                    gb_reset(gb);
                    break;
                case SDLK_ESCAPE:
                    sdl_destroy();
                    gb_free(gb);
                    exit(0);
                    break;
                    
                case SDLK_s:
                    CLOCKS_PER_SDL_FRAME += (70224 / 2);
                    break;
                case SDLK_a:
                    CLOCKS_PER_SDL_FRAME -= (70224 / 2);
                    break;
                
                case SDLK_z:
                    gb_joypad_press(gb, BUTTON_B);
                    break;
                case SDLK_x:
                    gb_joypad_press(gb, BUTTON_A);
                    break;
                case SDLK_UP:
                    gb_joypad_press(gb, BUTTON_UP);
                    break;
                case SDLK_DOWN:
                    gb_joypad_press(gb, BUTTON_DOWN);
                    break;
                case SDLK_LEFT:
                    gb_joypad_press(gb, BUTTON_LEFT);
                    break;
                case SDLK_RIGHT:
                    gb_joypad_press(gb, BUTTON_RIGHT);
                    break;
                case SDLK_q:
                    gb_joypad_press(gb, BUTTON_SELECT);
                    break;
                case SDLK_w:
                    gb_joypad_press(gb, BUTTON_START);
                    break;
            }
        } else if(e.type == SDL_KEYUP) {
            switch(e.key.keysym.sym) {
                case SDLK_z:
                    gb_joypad_release(gb, BUTTON_B);
                    break;
                case SDLK_x:
                    gb_joypad_release(gb, BUTTON_A);
                    break;
                case SDLK_UP:
                    gb_joypad_release(gb, BUTTON_UP);
                    break;
                case SDLK_DOWN:
                    gb_joypad_release(gb, BUTTON_DOWN);
                    break;
                case SDLK_LEFT:
                    gb_joypad_release(gb, BUTTON_LEFT);
                    break;
                case SDLK_RIGHT:
                    gb_joypad_release(gb, BUTTON_RIGHT);
                    break;
                case SDLK_q:
                    gb_joypad_release(gb, BUTTON_SELECT);
                    break;
                case SDLK_w:
                    gb_joypad_release(gb, BUTTON_START);
                    break;
            }
        }
    }
}
