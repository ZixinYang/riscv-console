#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "video_api.h"
#include "thread_api.h"
#include <stdio.h>
// #include "api.h"
#define SMALL_SPRITE_CTRL_OFFSET 16
#define CONTROLLER      (*((volatile uint32_t *)0x40000018))

int checkAlive(int cur_x, int cur_y, int budget);
int checkGetPellet(int cur_x, int cur_y, int center_x, int center_y, int budget);
void initSpriteControllers(void);
void drawPellet(void);
int genRandom(int high);
void getCoordinates(int idx, int* x, int* y);

volatile uint32_t *MODE_CTRL_REG = (volatile uint32_t *)(0x500FF414);
volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile uint8_t *SMALL_SPRITE_CONTROLS[128];
volatile uint8_t *SMALL_SPRITE_DATAS[128];

int main() {
    int last_global = 42;
    setDisplayMode(DISPLAY_MODE_GRAPHICS);
    // Set color to sprite palette
    volatile uint32_t SPRITE_PALETTE[3];
    SPRITE_PALETTE[0] = 0x8000A65F;
    SPRITE_PALETTE[1] = 0x80FFFFFF;
    SPRITE_PALETTE[2] = 0x80000000;
    initSpritePalette(0, (uint8_t*)SPRITE_PALETTE, 10);
    int pellet_x = 100;
    int pellet_y = 100;
    int center_x = pellet_x + 4;
    int center_y = pellet_y + 4;
    int step_size = 3;
    
    drawPellet();
    setSmallSpriteControl(0, 0, 8, 8, pellet_x, pellet_y, 0);

    int control_idx = 1;
    int cur_x = 0;
    int cur_y = 0;
    int budget = 5;
    int alive = 1;
    uint32_t current_status = 0;
    uint32_t last_status = 0;

    while (alive == 1) {
        global = getTicks();
        if(global != last_global){
            controller_status = CONTROLLER;
            if (controller_status == 0x0){
                current_status = last_status;
            }
            else {
                current_status = controller_status;
            }
            if(current_status & 0x1){
                if(cur_x >= step_size){
                    cur_x -= step_size;
                }
                else{
                    break;
                }
            }
            if(current_status & 0x2){
                if( cur_y >= step_size){
                    cur_y -= step_size;
                }
                else{
                    break;
                }
            }
            if(current_status & 0x4){
                if( cur_y <= 287 - step_size){
                    cur_y += step_size;
                }
                else{
                    break;
                }
            }
            if(current_status & 0x8){
                if(cur_x <= 511 - step_size){
                    cur_x += step_size;
                }
                else{
                    break;
                }
            }
            if (checkGetPellet(cur_x, cur_y, center_x, center_y, budget)){
                budget += 3;
                pellet_x = genRandom(512);
                pellet_y = genRandom(288);
                setSmallSpriteControl(0, 0, 8, 8, pellet_x, pellet_y, 0);
                center_x = pellet_x + 4;
                center_y = pellet_y + 4;
            }

            alive = checkAlive(cur_x, cur_y, budget);
            setSmallSpriteControl(control_idx, 0, 6, 6, cur_x, cur_y, 0);
            control_idx++;
            if (control_idx == budget){
                control_idx = 1;
            }
            last_global = global;
            last_status = current_status;
        }
    }
    setDisplayMode(DISPLAY_MODE_TEXT);
    linePrintf(0, "GAME OVER!!!");
    while(1);
    return 0;
}

int genRandom(int high)
{
    static unsigned long int next = 1;
    next = ((next * 214013L + 2531011L) >> 16) & 0x7fff;
    return next % high;
}

int checkAlive(int cur_x, int cur_y, int budget){
    int alive = 1;
    int x, y;
    if (cur_x != 0){
        for (int i = 1; i < budget; i++){
            getCoordinates(i, &x, &y);
            if (x == cur_x & y == cur_y){
                alive = 0;
                break;
            }
        }
    }
    return alive;
}

void getCoordinates(int idx, int* x, int* y){
    *x = ((*SMALL_SPRITE_CONTROLS[idx] >> 2) & 0x3FF) - SMALL_SPRITE_CTRL_OFFSET;
    *y = ((*SMALL_SPRITE_CONTROLS[idx] >> 12) & 0x1FF) - SMALL_SPRITE_CTRL_OFFSET;
}

int checkGetPellet(int cur_x, int cur_y, int center_x, int center_y, int budget){
    return (cur_x < center_x + 10) & (cur_y < center_y + 10) & (cur_x > center_x - 10) & (cur_y > center_y - 10) & (budget <= 129);
}

void drawPellet(){
    uint8_t sprite_img[16 * 16];
    for(int x = 0; x < 16; x++){
        for(int y = 0; y < 16; y++){   
            sprite_img[(y<<4) + x] = ((x >= 3) & (x <= 5) & (y >= 0) & (y <= 8)) | ((x >= 2) & (x <= 6) & (y >= 3) & (y <= 5)) ? 1 : 2;
        }
    }
    setSmallSpriteDataImage(0, sprite_img);
}