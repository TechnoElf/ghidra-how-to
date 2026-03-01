.global _start
.section .text._start
_start:
    # Set up the stack
    la sp, _stack_top

    # Clear .bss
    la t0, _sbss
    la t1, _ebss
1:
    bge t0, t1, 2f
    sw zero, 0(t0)
    addi t0, t0, 4
    j 1b
2:

    # Initialize data
    la t0, _sidata
    la t1, _sdata
    la t2, _edata
3:
    bge t1, t2, 4f
    lw t3, 0(t0)
    sw t3, 0(t1)
    addi t0, t0, 4
    addi t1, t1, 4
    j 3b
4:

    # Jump to main
    j main

    # Hang indefinitely
5:
    j 5b
