
#include <fx2types.h>

#ifdef assert
#undef assert
#endif
#define assert(...)

#define BUFFER_STRUCT(type) \
	type struct buffer##type

// Buffer stored in "Indirect RAM". Quickly accessible with indirect accessing
// @Ri (where Ri == R0, R1).
// Head pointer only takes a single byte
BUFFER_STRUCT(__idata) {
	BYTE _head; // How many bytes from &(buffer) the data starts
	BYTE _size;  // How many bytes are in the buffer
	BYTE _buffer[];
};

// Buffer stored in xdata ("program space"). Quickly accessible with auto
// pointers.


// Bytes 00-1F - 32 bytes to hold up to 4 banks of the registers R0 to R7
// 0x00 - 0x07  bank0
// 0x08 - 0x0f  bank1
// 0x10 - 0x07  bank2
// 0x18 - 0x1f  bank3

#define CREATE_BUFFER(name, type, size) \
	volatile BYTE name##_head; \
	volatile __bit name##_head_inc; \
	volatile BYTE name##_tail; \
	type BYTE name##_buffer[1<<size];

#define BUFFER(name, type) \
	((BUFFER_STRUCT(type)*)(&name))

// FIXME: How do we make this aligned?
CREATE_BUFFER(buffer, __idata, 4)

inline BYTE buffer_maxsize() {
	return sizeof(buffer_buffer);
}

inline BYTE buffer_maxmask() {
	return (buffer_maxsize()-1);
}

inline BYTE buffer_size() {
	return (buffer_tail - buffer_head) & buffer_maxmask();
}

// MOV ea, C
// CLR ea
// INC @RPi

// SWAP a      -- swap top/bottom nibbles of accumulator
// XCHD A, @Ri -- bottom nibble of accumulator<->bottom nibble of @Ri
/*
	__asm__("mov	a,#0x0f");		// 2 cycles
	__asm__("mov	r0,#_buffer_info");	// 2 cycles
	__asm__("anl	a,@r0");		// 1 cycles
	__asm__("inc	a");			// 1 cycles
	__asm__("xchd	a,@r0");		// 1 cycles

	__asm__("mov	a,#0xf0");		// 2 cycles
	__asm__("mov	r0,#_buffer_info");	// 2 cycles
	__asm__("anl	a,@r0");		// 1 cycles
	__asm__("swap	a");			// 1 cycles
	__asm__("inc	a");			// 1 cycles
	__asm__("swap	a");			// 1 cycles
	__asm__("xchd	a,@r0");		// 1 cycles
	__asm__("mov	@r0,a");		// 1 cycles
*/


inline BYTE buffer_inc() {
  BYTE head = 0;

  if (buffer_head_a) {
    buffer_head_a = 0;

    ++buffer_head2;
    do {
      head = ++buffer_head1;
    } while(buffer_head1 != buffer_head2)

    buffer_head_a = 1;
  } else {
    head = ++buffer_head2;
  }

  return head;
}


/*
inline BYTE buffer_inc() {
	BYTE r = 0;
	__critical {
		r = (buffer_head + 1) & buffer_maxmask();
		assert(buffer_head != buffer_tail);
		buffer_head = r;
	}
	return r;
}
*/

inline BYTE buffer_dec() __critical {
	BYTE r = 0;
	__critical {
		r = buffer_tail;
		buffer_tail = (r + 1) & buffer_maxmask();
		assert(r != buffer_head);
	}
	return r;
}

inline void buffer_add_byte(BYTE data) {
	buffer_buffer[buffer_inc()] = data;
}

inline BYTE buffer_remove_byte() {
	return buffer_buffer[buffer_dec()];
}

