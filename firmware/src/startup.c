#include <stdint.h>

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

// Adapted from https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code
__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void){
    uint32_t result;
    asm volatile ("csrr %0, mstatus" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val){
    asm volatile ("csrw mstatus, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val){
    asm volatile ("csrw mie, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
    asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
    asm volatile ("csrci mstatus, 0x8");
}

#define MTIME_LOW       (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH      (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW    (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH   (*((volatile uint32_t *)0x40000014))
#define CONTROLLER      (*((volatile uint32_t *)0x40000018))

int rand(int high);
uint32_t calcSmallSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t calcLargeSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t calcBackgroundControl(uint32_t x, uint32_t y, uint32_t z, uint32_t p);
void setSmallSpriteControl(int sprite_id, uint32_t addr);
void setLargeSpriteControl(int sprite_id, uint32_t addr);
void shiftSmallSpriteControl(int sprite_id, uint32_t x, uint32_t y);
void shiftLargeSpriteControl(int sprite_id, uint32_t x, uint32_t y);
void setGraphicsMode(void);
void setTextMode(void);
void setColor(int palette_id, int entry_id, uint32_t rgba);
void initSpriteControllers();

static unsigned long int next = 1;

extern volatile int global;
extern volatile uint32_t controller_status;
volatile uint32_t *INT_PEND_REG = (volatile uint32_t *)(0x40000004);
volatile uint32_t *MODE_CTRL_REG = (volatile uint32_t *)(0x500FF414);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile uint32_t *SMALL_SPRITE_CONTROLS[128];
volatile uint32_t *LARGE_SPRITE_CONTROLS[128];

void init(void){
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while(Base < End){
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while(Base < End){
        *Base++ = 0;
    }

    csr_write_mie(0x888);       // Enable all interrupt soruces
    csr_enable_interrupts();    // Global interrupt enable
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;
}

void c_interrupt_handler(uint32_t mcause){
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH)<<32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare>>32;
    MTIMECMP_LOW = NewCompare;
    global++;
    controller_status = CONTROLLER;
    initSpriteControllers();
}

uint32_t c_system_call(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t call){
    if(call == 0){
        return global;
    }
    else if(call == 1){
        return CONTROLLER;
    }
    else if (call == 2){
        int r = rand(a0);
        return r;
    }
    else if (call == 3){
        setGraphicsMode();
    }
    else if (call == 4){
        setTextMode();
    }
    else if (call == 5){
        setColor(a0, a1, a2);
    }
    else if (call == 6){
        uint32_t r = calcSmallSpriteControl(a0, a1, a2, a3, a4);
        return r;
    }
    else if (call == 7){
        uint32_t r = calcLargeSpriteControl(a0, a1, a2, a3, a4);
        return r;
    }
    else if (call == 8){
        uint32_t r = calcBackgroundControl(a0, a1, a2, a3);
        return r;
    }
    else if (call == 9){
        setSmallSpriteControl(a0, a1);
    }
    else if (call == 10){
        setLargeSpriteControl(a0, a1);
    }
    else if (call == 11){
        shiftSmallSpriteControl(a0, a1, a2);
    }
    else if (call == 12){
        shiftLargeSpriteControl(a0, a1, a2);
    }
    return -1;
}

void initSpriteControllers(){
    for (int i = 0; i < 128; i++){
        SMALL_SPRITE_CONTROLS[i] = (volatile uint32_t *)(0x500FF214 + i * 4);
        LARGE_SPRITE_CONTROLS[i] = (volatile uint32_t *)(0x500FF114 + i * 4);
    }
}

void setSmallSpriteControl(int sprite_id, uint32_t addr){
    *SMALL_SPRITE_CONTROLS[sprite_id] = addr;
}

void setLargeSpriteControl(int sprite_id, uint32_t addr){
    *LARGE_SPRITE_CONTROLS[sprite_id] = addr;
}

void shiftSmallSpriteControl(int sprite_id, uint32_t x, uint32_t y){
    *SMALL_SPRITE_CONTROLS[sprite_id] &= 0xFFE00003;
    *SMALL_SPRITE_CONTROLS[sprite_id] |= (((y+16)<<12) | ((x+16)<<2));
}

void shiftLargeSpriteControl(int sprite_id, uint32_t x, uint32_t y){
    *LARGE_SPRITE_CONTROLS[sprite_id] &= 0xFFE00003;
    *LARGE_SPRITE_CONTROLS[sprite_id] |= (((y+64)<<12) | ((x+64)<<2));
}

int rand(int high)
{
    next = ((next * 214013L + 2531011L) >> 16) & 0x7fff;
    return next % high;
}

uint32_t calcSmallSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p){
    return ((h-1)<<25) | ((w-1)<<21) | ((y+16)<<12) | ((x+16)<<2) | p;
}

uint32_t calcLargeSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p){
    return ((h-33)<<26) | ((w-33)<<21) | ((y+64)<<12) | ((x+64)<<2) | p;
}

uint32_t calcBackgroundControl(uint32_t x, uint32_t y, uint32_t z, uint32_t p){
    return ((z<<22) | ((y+288)<<12) | ((x+512)<<2)) | p;
}

void setGraphicsMode(){
    *MODE_CTRL_REG |= 0x1;
}

void setTextMode(){
    *MODE_CTRL_REG &= 0x0;
}

void setColor(int palette_id, int entry_id, uint32_t rgba){
    volatile uint32_t *SPRITE_PALETTE = (volatile uint32_t *)(0x500FD000 + 1024 * palette_id);
    SPRITE_PALETTE[entry_id] = rgba;
}