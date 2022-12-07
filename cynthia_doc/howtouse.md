# ECS251 Group 2 OS Document

## OS Abstraction
The designed API is aim for the game developer to develop the gaming environment easier. Our testing game is the Snake Game. The Snake game combined the standard input and output system such as mouse, keyboard, and monitor. We design various API functions for these kernel operations, and we also have some API functions like malloc and random number generator to allocate OS resource and generate random numbers.

## Table of Contents

## Getting Started
The OS is based on RISC-V instruction sets. Before you run the simulator, make sure you have downloaded the X11 display, Docker Engine, git bash on Windows or shell on Linux.

### Clone the Repository
The code is developed on `C` and host on the GitHub. You can download the code by downloading the .zip file provided by the GitHub. Or you can use `git` to `clone` the repository.
```git
git clone [url] 
```

### Install the Development Environment
Before you run the `./rvconsole.sh` shell, make sure you have downloaded the Docker on your working environment. Then run the command under the root folder.
```bash
cd [root-folder-path]
./rvconsole.sh
```

### Start the Development Environment
If the shell run successfully, you will see the docker is running and your terminal shows like this:
```bash
root@[docker-container-id]:code#
```

## API List
### Get Simulator Vㄍㄧㄠ
#### __Name__
`getTicks` - get the current CPU ticks\
`getStatus` - get the simulator's current status
#### __Signature__
`uint32_t getTicks(void);`\
`uint32_t getStatus(void);`
#### __Parameters__
`void`
#### __Return Value__
`uint32_t` - current CPU ticks\
`uint32_t` - simulator's current register status

### Get Random
#### __Name__
`genRandom` - produce the random number
#### __Signature__
`uint32_t genRandom(int high);`
#### __Parameters__
`int high` - the boudary of random numbers
#### __Return Value__
Random number returns in 32 binary bits

### Pallete Control
#### __Name__
`setColor` - set pallete's color under the video mode\
`setBackgroundColor` - set background color under the video mode 
#### __Signature__
`void setColor(int palette_id, int entry_id, uint32_t rgba);`\
`void setBackgroundColor(int palette_id, int entry_id, uint32_t rgba);`
#### __Parameters__
`int palette_id` - find the palette location in register\
`int entry_id` - locate the entry in palette array\
`uint32_t rgba` - set the color by RGBA
#### __Return Value__
No return values

### Sprite Control
#### __Name__
`calcSmallSpriteControl` - control the pixel position on small sprite under the video mode\
`calcLargeSpriteControl` - control the pixel position on large sprite under the video mode\
`calcBackgroundControl` - control the background under the video mode\
`setLargeSpriteControl` - call the large sprite control method\
#### __Signature__
`uint32_t calcSmallSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);`\
`uint32_t calcLargeSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);`\
`uint32_t calcBackgroundControl(uint32_t x, uint32_t y, uint32_t z, uint32_t p);`\

#### __Parameters__
`uint32_t x` - x position of the sprite\
`uint32_t y` - y posision of the sprite\
`uint32_t z` - \
`uint32_t w` - width of the sprite\
`uint32_t h` - height of the sprite\
`uint32_t p` - palette ID
#### __Return Value__
Return the next position of the sprite




