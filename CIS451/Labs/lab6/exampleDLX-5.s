	add	r4, r0, r0	; clear R4 = 0
	add	r5, r0, r0	; clear R5 = 0

	addi	r1, r0, 5	; set R1 (loop counter) = 5

LOOP:	subi	r1, r1, 1	; decrement R1
	addi	r4, r4, 2	; R4 = R4 + 2
	addi	r5, r5, 10	; R5 = R5 + 10
	bnez	r1, LOOP	; if (r1 != 0), goto LOOP
	nop
	add	r6, r4, r5	; R6 = R4 + R5
	
	nop
	nop
	nop
	nop
	trap	#0
