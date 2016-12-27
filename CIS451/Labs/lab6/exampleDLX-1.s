	lw	r1, 0(r2)     	; r1 <- M[0+r2]
	sub	r3, r4, r5    	; r3 <- r4 - r5
	and	r6, r7, r8    	; r6 <- r7 AND r8
	or	r9, r10, r11  	; r9 <- r10 OR r11
	and	r12, r13, r14 	; r12 <- r13 AND r14
	nop
	nop
	nop
	nop
	trap	#0
