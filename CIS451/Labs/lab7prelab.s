	lw r3, 4(r1)  ;item1
	lw r4, 8(r1)  ;item2
	add r2, r3, r4
	sw 0(r1), r2  ;storing into sum	
	lw r6, 16(r1) ;tax
	add r5, r2, r6
	sw 12(r1), r5 ;storing total
	lw r8, 24(r1) ;item3
	lw r9, 28(r1) ;item4
	add r7, r8, r9
	sw 20(r1), r7 ;storing into sum
	nop
	nop
	nop
	nop
	trap	#0
