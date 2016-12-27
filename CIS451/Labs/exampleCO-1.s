; The base address of array a is in r1 
; The base address of array b is in r2
; The base address of array c is in r3

      addi r4, r0, 4		; Set r4 (the loop counter) to MAX
LOOP: lw r5, 0(r1)		; Load from a into r5
      lw r6, 0(r2)		; Load from b into r6
      add r7, r5, r6		; r7 = r6 + r5
      sw  0(r3), r7		; store result back in array c
      addi r1, r1, 4		; increment pointers for arrays a, b, and c
      addi r2, r2, 4
      addi r3, r3, 4
      subi r4, r4, 1		; decrement loop counter
      bnez r4, LOOP		; branch
      nop			; branch delay slot
      sw 0(r3), r0 		; set c[0] to 0. (not important, just something to do after the loop.)
      nop;
      nop; 
      nop;
      nop;	
      trap #0;
