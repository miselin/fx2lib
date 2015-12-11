
#include <fx2macros.h>

#define autoptr_memcpy(dst, src, size) \
	do { \
		BYTE i = 0; \
		AUTOPTRSETUP = bmAPTR2INC | bmAPTR1INC | bmAPTREN; \
		LOADWORD(AUTOPTR1, src); \
		LOADWORD(AUTOPTR2, dst); \
		for(i=size; i > 0; i--) { \
			XAUTODAT2 = XAUTODAT1; \
		} \
	} while(0)

inline void autoptr_memcpy_inline(__xdata BYTE * dst, __xdata BYTE * src, BYTE size) {
	BYTE i = 0;
	AUTOPTRSETUP = bmAPTR2INC | bmAPTR1INC | bmAPTREN;
	LOADWORD(AUTOPTR1, src);
	LOADWORD(AUTOPTR2, dst);
	for(i=size; i > 0; i--) {
		XAUTODAT2 = XAUTODAT1;
	}
}

#define autoptr_memcpy_asm(dst, src, size) \
	do { \
		WORD i = 0; \
		AUTOPTRSETUP = bmAPTR2INC | bmAPTR1INC | bmAPTREN; \
		__asm__("; src -> DP0"); \
		LOADWORD(AUTOPTR2, dst); \
		for(i=size; i > 0; i--) { \
			XAUTODAT2 = XAUTODAT1; \
		} \
	} while(0)

#define dptr_memcpy(dst, src, size) \
	do { \
		__asm__("; src -> DP0"); \
		__asm__("mov	dptr,#_" # src "	; 3 cy"); \
		__asm__("inc	_DPS		; 2 cy"); \
		__asm__("; dst -> DP1"); \
		__asm__("mov	dptr,#_" # dst "	; 3 cy"); \
		__asm__("inc	_DPS		; 2 cy"); \
		__asm__("mov	r0,#" # size "		; 2 cy"); \
		__asm__("00001$:"); \
		__asm__("movx	a, @dptr	; 2 cy"); \
		__asm__("inc	dptr		; 3 cy"); \
		__asm__("inc	_DPS		; 2 cy"); \
		__asm__("movx	@dptr, a	; 2 cy"); \
		__asm__("inc	dptr		; 3 cy"); \
		__asm__("inc	_DPS		; 2 cy"); \
		__asm__("djnz	r0,00001$	; 3 cy"); \
	} while(0)
/*
		LOADWORD(DP0, src); \
		LOADWORD(DP1, dst); \
*/
