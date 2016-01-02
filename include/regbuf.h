
#include <fx2types.h>

#ifdef assert
#undef assert
#endif
#define assert(...)

// bank 3
// _head == R0
// _tail == R1
// _buffer == indirect memory
__idata __at(0x18) struct register_buffer { \
	__idata BYTE* _head;
	__idata BYTE* _tail;
}


// 0x18 - 0x1f  bank3

// 0bXXXXXX00 == 0xfc
//
// buffer[0] == 0xX0 <-- d[0]
// buffer[1] == 0xX1 <-- head
// buffer[2] == 0xX2
// buffer[3] == 0xX3
// buffer[4] == 0xX4
// buffer[5] == 0xX5 <-- d[3] == tail
// buffer[6] == 0xX6 <-- d[2]
// buffer[7] == 0xX7 <-- d[1]


// buffer[0] == 0xX0
// buffer[1] == 0xX1 <-- d[3] == tail
// buffer[2] == 0xX2 <-- d[2]
// buffer[3] == 0xX3 <-- d[1]
// buffer[4] == 0xX4 <-- d[0]
// buffer[5] == 0xX5 <-- head
// buffer[6] == 0xX6
// buffer[7] == 0xX7


// 0xX1 + 0xX5 == 0xX6

// head = 0x

__idata buffer[8];

	__asm__("mov a, R0");
	__asm__("subb a, R1");
	__asm__("andl a, #0x0f");



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
	type __at(0x18) struct buffer_##name { \
		BYTE _head; \
		BYTE _size; \
		BYTE _buffer[size]; \
	} name

#define BUFFER(name, type) \
	((BUFFER_STRUCT(type)*)(&name))

// FIXME: How do we make this aligned?
CREATE_BUFFER(test, __idata, 6);

inline BYTE buffer_maxsize() {
	return sizeof(test._buffer);
}

inline BYTE buffer_size() {
	return test._size;
}

inline BYTE* buffer_start() {
	return &(test._buffer[test._head]);
}

inline BYTE* buffer_end() {
	return &(test._buffer[(test._head+test._size) % buffer_maxsize()]);
}

inline void buffer_push_byte(BYTE data) {
	assert((buffer_size()+1) < buffer_maxsize());
	__critical {
		test._size += 1;
		*buffer_end() = data;
	}
}

inline BYTE buffer_pop_byte() {
	BYTE r;
	assert(buffer_size() > 0);
	__critical {
		r = *buffer_start();
		test._head += 1;
		test._size -= 1;
	}
	return r;
}

