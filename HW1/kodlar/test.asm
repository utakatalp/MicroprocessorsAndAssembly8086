assume cs:my_cs, ds: my_ds, SS: my_ss

MAIN    PROC FAR

        push DS
        XOr ax, ax
        push ax
        mov ax,my_ds
        push ax




        push ds
        xor ax,ax
        push ax
        mov ax, my_ds
        push ax;
