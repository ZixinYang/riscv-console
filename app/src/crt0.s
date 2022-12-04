.section .init, "ax"
.global _start
_start:
    .cfi_startproc
    .cfi_undefined ra
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, __stack_top
    add s0, sp, zero
    jal ra, init
    nop
    jal zero, main
    .cfi_endproc
    

.section .text, "ax"
.global getTicks, getStatus, genRandom, setGraphicsMode, setTextMode, setColor, calcSmallSpriteControl, calcLargeSpriteControl, calcBackgroundControl
.global setSmallSpriteControl, setLargeSpriteControl, shiftSmallSpriteControl, shiftLargeSpriteControl
getTicks:
    li a5, 0
    ecall
getStatus:
    li a5, 1
    ecall
genRandom:
    li a5, 2
    ecall
setGraphicsMode:
    li a5, 3
    ecall
setTextMode:
    li a5, 4
    ecall
setColor:
    li a5, 5
    ecall
calcSmallSpriteControl:
    li a5, 6
    ecall
calcLargeSpriteControl:
    li a5, 7
    ecall
calcBackgroundControl:
    li a5, 8
    ecall
setSmallSpriteControl:
    li a5, 9
    ecall
setLargeSpriteControl:
    li a5, 10
    ecall
shiftSmallSpriteControl:
    li a5, 11
    ecall
shiftLargeSpriteControl:
    li a5, 12
    ecall
.end
