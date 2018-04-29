.include "xc.inc"
.text
.global _write_1, _write_0
    
_write_0:
    bset LATB, #6   ; 1 c
    repeat #3	    ; 1 c
    nop		    ; 4 c
    
    bclr LATB, #6   ; 1 c
    repeat #6	    ; 1 c
    nop		    ; 7 c
    return	    ; 3 c
    
_write_1:	    ; 2 c
    bset LATB, #6   ; 1 c
    repeat #8	    ; 1 c
    nop		    ; 9 c
    
    bclr LATB, #6   ; 1 c
    repeat #1	    ; 1 c
    nop		    ; 3 c
    return	    ; 3 c
    
.end



