#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#define SMALL_SPRITE_CTRL_OFFSET 16

int checkAlive(int cur_x, int cur_y, int budget);
int checkGetPellet(int cur_x, int cur_y, int center_x, int center_y, int budget);
void initSpriteControllers(void);
void initSpriteData(void);
void drawPellet(void);

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile char *VIDEO_MEMORY = (volatile char *)(0x500FE800);
volatile uint32_t *SMALL_SPRITE_CONTROLS[128];
volatile uint8_t *SMALL_SPRITE_DATAS[128];

int main() {
    int last_global = 42;
    initSpriteControllers();
    initSpriteData();
    setGraphicsMode();
    // Set color to sprite palette
    setColor(0, 0, 0x8000A65F);
    setColor(0, 1, 0x80FFFFFF);
    setColor(0, 2, 0x80000000);
    int pellet_x = 100;
    int pellet_y = 100;
    int center_x = pellet_x + 4;
    int center_y = pellet_y + 4;
    int step_size = 3;
    
    drawPellet();
    SMALL_SPRITE_CONTROLS[0][0] = calcSmallSpriteControl(pellet_x,pellet_y,8,8,0);

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
            controller_status = getStatus();
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
                SMALL_SPRITE_CONTROLS[0][0] = calcSmallSpriteControl(pellet_x,pellet_y,8,8,0);
                center_x = pellet_x + 4;
                center_y = pellet_y + 4;
            }

            alive = checkAlive(cur_x, cur_y, budget);
            *SMALL_SPRITE_CONTROLS[control_idx] = calcSmallSpriteControl(cur_x, cur_y, 6, 6, 0);
            control_idx++;
            if (control_idx == budget){
                control_idx = 1;
            }
            last_global = global;
            last_status = current_status;
        }
    }
    setTextMode();
    char *ptr = malloc(12);
    ptr[0] = 'G';
    ptr[1] = 'A';
    ptr[2] = 'M';
    ptr[3] = 'E';
    ptr[4] = ' ';
    ptr[5] = 'O';
    ptr[6] = 'V';
    ptr[7] = 'E';
    ptr[8] = 'R';
    ptr[9] = '!';
    ptr[10] = '!';
    ptr[11] = '!';
    ptr[12] = '\0';
    strcpy(VIDEO_MEMORY, ptr);
    return 0;
}

int checkAlive(int cur_x, int cur_y, int budget){
    int alive = 1;
    int x, y;
    if (cur_x != 0){
        for (int i = 1; i < budget; i++){
            x = ((*SMALL_SPRITE_CONTROLS[i] >> 2) & 0x3FF) - SMALL_SPRITE_CTRL_OFFSET;
            y = ((*SMALL_SPRITE_CONTROLS[i] >> 12) & 0x1FF) - SMALL_SPRITE_CTRL_OFFSET;
            if (x == cur_x & y == cur_y){
                alive = 0;
                break;
            }
        }
    }
    return alive;
}

int checkGetPellet(int cur_x, int cur_y, int center_x, int center_y, int budget){
    return (cur_x < center_x + 10) & (cur_y < center_y + 10) & (cur_x > center_x - 10) & (cur_y > center_y - 10) & (budget <= 129);
}

void initSpriteControllers(){
    for (int i = 0; i < 128; i++){
        SMALL_SPRITE_CONTROLS[i] = (volatile uint32_t *)(0x50000000 + 0xFF214 + i * 4);
    }
}

void initSpriteData(){
    for (int i = 0; i < 128; i++){
        SMALL_SPRITE_DATAS[i] = (volatile uint8_t *)(0x50000000 + 0xF4000 + i * 256);
    }
}

void drawPellet(){
    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 16; x++){
            SMALL_SPRITE_DATAS[0][(y<<4) + x] = ((x >= 3) & (x <= 5) & (y >= 0) & (y <= 8)) | ((x >= 2) & (x <= 6) & (y >= 3) & (y <= 5)) ? 1 : 2;
        }
    }
}