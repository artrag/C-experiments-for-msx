	psect	text

; void bmove(void* s, void* d, uint n);
;
; Note: this implementation is not correct when source and destination
;       areas overlap

	global _bmove
_bmove:
; _s passed in de
	ex	de,hl	; hl = source
; _d passed in bc
	ld	e,c
	ld	d,b	; de = destination
; _n passed on the stack
	pop	af
	pop	bc	; bc = count
	push	af
	ld	a,b
	or	c
	ret	z
	ldir
	ret
