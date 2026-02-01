        .module startup

        .globl  _main

        ; -----------------------------
        ; Vector table (linker must place VECTORS at 0x8000)
        ; -----------------------------
        .area   VECTORS

        .dw     reset_handler

        ; -----------------------------
        ; Code
        ; -----------------------------
        .area   CODE

reset_handler:
        ; SP cannot be loaded immediate on STM8.
        ; Use X as a temp: X <- #imm, SP <- X
        ldw     x, #0x07FF
        ldw     sp, x

        ; Call main
        call    _main

1$:
        jra     1$
