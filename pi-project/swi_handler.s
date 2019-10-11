.global swi_handler

	@ Wrapper for the SWI handler
	@ You can do this in gcc with __attribute__((interrupt("SWI")))
	@ but it doesn't handle returning the value in r0

	@ TODO: save SPSR, etc?

	@ Note: currently do not call swi from SVC mode or things
	@       will get corrupted

swi_handler:

	push    {r0, r1, r2, r3, r4, r7, ip, lr}


	@ Call the C version of the handler

	bl	swi_handler_c

	@ Put our return value of r0 on the stack so it is
	@ restored with the rest of the saved registers

	str	r0,[sp,#0]

	@ Restore saved values.  The ^ means to restore the userspace registers
	ldm	sp!, {r0, r1, r2, r3, r4, r7, ip, pc}^
