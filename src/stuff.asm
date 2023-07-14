EXTERN _ms
EXTERN _tmp8

PUBLIC _do_x_asm
SECTION	code_compiler

_do_x_asm:
    ; border white
    ld a,  0x07
    out(0xfe),a

    ; tmp8 = x + vx

    ld hl, (_ms)
    ld a, (hl) ; x
    inc hl
    inc hl
    ld l, (hl) ; vx
    add a, l
    ld (_tmp8), a

    ; if vx > 0
    ld a, l
    and a
   	jp m,dox_i_8	; jump if minus (not more than zero)

    ; if tmp8 > 240
   	ld	hl,(_tmp8)
   	ld	a,240
   	sub	l
   	jr	nc,dox_i_9	; jump if carry not set
    
    ; x = 240
   	ld	hl,(_ms)
   	ld	(hl),240 ; (_ms)+0 = x = 240
    
    ; vx = newspeed(vx)
    ; hl already contains _ms address
   	inc	hl
   	inc	hl
    ld a, (hl)    
    cpl ; invert bits
    inc a ; inc
    ld (hl), a ; vx is now the inverse of vx (-vx)

    ; spriteFlags =  0x08
   	ld	hl,(_ms)
   	ld	bc,4
   	add	hl,bc
   	ld	(hl),8
    ret ; // done
   	; jp	dox_i_11	;EOS
   .dox_i_9
    
    ; else ( tmp8 <= 240 )
    ; x = tmp8;
   	ld  hl, (_ms) ; with the pointer at _ms
   	ld	a,(_tmp8) ; store tmp8 into a
   	ld	(hl),a ; (_ms)+0 = x = tmp8 (ms->x)
   	ret
   .dox_i_8
    
    ; else (vx is negative)
    ; if tmp8 > x 
   	ld	hl,(_ms)
    ld hl, (hl)
    ld  a, l
   	ld	hl,(_tmp8)
    sbc l
   	jr	nc,dox_i_12	;
    
    ; x = 0
   	ld	hl,(_ms)
   	ld	(hl),0

    ; vx = newspeed(vx)
    ; hl already contains _ms address
   	inc	hl
   	inc	hl
    ld a, (hl)    
    dec a
    cpl ; invert bits
    ; inc a ; inc
    ld (hl), a ; vx is now the inverse of vx (-vx)


    ; spriteFlags = 0
   	ld	hl,(_ms)
   	ld	bc,4
   	add	hl,bc
   	ld	(hl),0
    ret
   	; jp	i_13	;EOS
   .dox_i_12
    ; x = tmp8
   	ld	hl,(_ms)
   	ld	a,(_tmp8)
   	ld	(hl),a
   .dox_i_11
   	ret
   
