; Copyright (C) 2009 Ubixum, Inc. 
;
; This library is free software; you can redistribute it and/or
; modify it under the terms of the GNU Lesser General Public
; License as published by the Free Software Foundation; either
; version 2.1 of the License, or (at your option) any later version.
; 
; This library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; Lesser General Public License for more details.
; 
; You should have received a copy of the GNU Lesser General Public
; License along with this library; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

; this is a the default 
; full speed and high speed 
; descriptors found in the TRM
; change however you want but leave 
; the descriptor pointers so the setupdat.c file works right
 

.module DEV_DSCR 

; descriptor types
; same as setupdat.h
DSCR_DEVICE_TYPE=1
DSCR_CONFIG_TYPE=2
DSCR_STRING_TYPE=3
DSCR_INTERFACE_TYPE=4
DSCR_ENDPOINT_TYPE=5
DSCR_DEVQUAL_TYPE=6

; for the repeating interfaces
DSCR_INTERFACE_LEN=9
DSCR_ENDPOINT_LEN=7

; endpoint types
ENDPOINT_TYPE_CONTROL=0
ENDPOINT_TYPE_ISO=1
ENDPOINT_TYPE_BULK=2
ENDPOINT_TYPE_INT=3

    .globl	_dev_dscr, _dev_qual_dscr, _highspd_dscr, _fullspd_dscr, _dev_strings, _dev_strings_end, _dev_serial
; These need to be in code memory.  If
; they aren't you'll have to manully copy them somewhere
; in code memory otherwise SUDPTRH:L don't work right
    .area	DSCR_AREA	(CODE)

; DEVICE DESCRIPTOR
_dev_dscr:
	.db dev_dscr_end-_dev_dscr    ; 0 bLength 1 Descriptor size in bytes (12h)
	.db DSCR_DEVICE_TYPE          ; 1 bDescriptorType 1 The constant DEVICE (01h)
	.dw 0x0002                    ; 2 bcdUSB 2 USB specification release number (BCD)
	.db 0x02                      ; 4 bDeviceClass 1 Class code
	.db 0x00                      ; 5 bDeviceSubclass 1 Subclass code
	.db 0x00                      ; 6 bDeviceProtocol 1 Protocol Code
	.db 64                        ; 7 bMaxPacketSize0 1 Maximum packet size for endpoint zero
	.dw 0xB404                    ; 8 idVendor 2 Vendor ID
	.dw 0x0410                    ; 10 idProduct 2 Product ID
	.dw 0x0100                    ; 12 bcdDevice 2 Device release number (BCD)
	.db 1                         ; 14 iManufacturer 1 Index of string descriptor for the manufacturer
	.db 2                         ; 15 iProduct 1 Index of string descriptor for the product
	.db 3                         ; 16 iSerialNumber 1 Index of string descriptor for the serial number
	.db 1                         ; 17 bNumConfigurations 1 Number of possible configurations
dev_dscr_end:

_dev_qual_dscr:
	.db	dev_qualdscr_end-_dev_qual_dscr
	.db	DSCR_DEVQUAL_TYPE
	.dw	0x0002                              ; usb 2.0
	.db	0xff
	.db	0xff
	.db	0xff
	.db	64                                  ; max packet
	.db	1					; n configs
	.db	0					; extra reserved byte
dev_qualdscr_end:

; CONFIGURATION DESCRIPTOR
_highspd_dscr:
	.db	highspd_dscr_end-_highspd_dscr      ; dscr len											;; Descriptor length
	.db	DSCR_CONFIG_TYPE
    ; can't use .dw because byte order is different
	.db	(highspd_dscr_realend-_highspd_dscr) % 256 ; total length of config lsb
	.db	(highspd_dscr_realend-_highspd_dscr) / 256 ; total length of config msb
	.db	2								 ; n interfaces
	.db	1								 ; config number
	.db	0								 ; config string
	.db	0x80                             ; attrs = bus powered, no wakeup
	.db	0x32                             ; max power = 100ma
highspd_dscr_end:

; Full speed CDC configuration. Max packet size is 512 bytes.
;
; all the interfaces next
; NOTE the default TRM actually has more alt interfaces
; but you can add them back in if you need them.
; here, we just use the default alt setting 1 from the trm
  	; control endpoints
	.db DSCR_INTERFACE_LEN           ; Descriptor length
	.db DSCR_INTERFACE_TYPE          ; Descriptor type
	.db 0x00                         ; Zero-based index of this interface
	.db 0x00                         ; Alternate setting
	.db 0x01                         ; Number of end points
	.db 0x02                         ; Interface class
	.db 0x02                         ; Interface sub class
	.db 0x01                         ; Interface protocol code class
	.db 0x00                         ; Interface descriptor string index

	;; CDC Header Functional Descriptor
	.db 0x05                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x00                         ; Header Functional Descriptor
	.dw 0x1001                       ; bcdCDC

	;; CDC ACM Functional Descriptor
	.db 0x04                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x02                         ; Abstarct Control Management Functional Desc
	.db 0x00                         ; bmCapabilities

	;; CDC Union Functional Descriptor
	.db 0x05                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x06                         ; Union Functional Descriptor
	.db 0x00                         ; bMasterInterface
	.db 0x01                         ; bSlaveInterface0

	;; CDC Call Management (CM) Functional Descriptor
	.db 0x05                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x01                         ; CM Functional Descriptor
	.db 0x01                         ; bmCapabilities
	.db 0x01                         ; bDataInterface

; endpoint 1 in
	.db DSCR_ENDPOINT_LEN            ; Descriptor length
	.db DSCR_ENDPOINT_TYPE           ; Descriptor type
	.db 0x81                         ; Endpoint number, and direction
	.db ENDPOINT_TYPE_INT            ; Endpoint type
	.db 0x40                         ; Maximum packet size (LSB)
	.db 0x00                         ; Max packet size (MSB)
	.db 0x40                         ; Polling interval

	;; CDC Virtual COM Port Data Interface Descriptor
	; data endpoints
	.db DSCR_INTERFACE_LEN           ; Descriptor length
	.db DSCR_INTERFACE_TYPE          ; Descriptor type
	.db 0x01                         ; Zero-based index of this interface
	.db 0x00                         ; Alternate setting
	.db 0x02                         ; Number of end points
	.db 0x0A                         ; Interface class
	.db 0x00                         ; Interface sub class
	.db 0x00                         ; Interface protocol code class
	.db 0x00                         ; Interface descriptor string index

; endpoint 2 out
	.db DSCR_ENDPOINT_LEN            ; Descriptor length
	.db DSCR_ENDPOINT_TYPE           ; Descriptor type
	.db 0x02                         ; Endpoint number (ep2), and direction (out)
	.db ENDPOINT_TYPE_BULK           ; Endpoint type
	.db 0x00                         ; Maximum packet size (LSB)
	.db 0x02                         ; Max packet size (MSB) == 512 bytes
	.db 0x00                         ; Polling interval

; endpoint 6 in
	.db DSCR_ENDPOINT_LEN            ; Descriptor length
	.db DSCR_ENDPOINT_TYPE           ; Descriptor type
	.db 0x86                         ; Endpoint number (ep6), and direction (in)
	.db ENDPOINT_TYPE_BULK           ; Endpoint type
	.db 0x00                         ; Maximum packet size (LSB)
	.db 0x02                         ; Max packet size (MSB) == 512 bytes
	.db 0x00                         ; Polling interval

highspd_dscr_realend:

; Full speed CDC configuration. Max packet size is 64 bytes.
	.even
_fullspd_dscr:
	.db	fullspd_dscr_end-_fullspd_dscr      ; dscr len
	.db	DSCR_CONFIG_TYPE
    ; can't use .dw because byte order is different
	.db	(fullspd_dscr_realend-_fullspd_dscr) % 256 ; total length of config lsb
	.db	(fullspd_dscr_realend-_fullspd_dscr) / 256 ; total length of config msb
	.db	2								 ; n interfaces
	.db	1								 ; config number
	.db	0								 ; config string
	.db	0x80                             ; attrs = bus powered, no wakeup
	.db	0x32                             ; max power = 100ma
fullspd_dscr_end:

; all the interfaces next
; NOTE the default TRM actually has more alt interfaces
; but you can add them back in if you need them.
; here, we just use the default alt setting 1 from the trm
	.db	DSCR_INTERFACE_LEN
	.db	DSCR_INTERFACE_TYPE
	.db	0				 ; index
	.db	0				 ; alt setting idx
	.db	2				 ; n endpoints
	.db	0x2			 ; class
	.db	0x2
	.db	0x1
	.db	3	             ; string index

	;; CDC Header Functional Descriptor
	.db 0x05                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x00                         ; Header Functional Descriptor
	.dw 0x1001                       ; bcdCDC

	;; CDC ACM Functional Descriptor
	.db 0x04                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x02                         ; Abstarct Control Management Functional Desc
	.db 0x00                         ; bmCapabilities

	;; CDC Union Functional Descriptor
	.db 0x05                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x06                         ; Union Functional Descriptor
	.db 0x00                         ; bMasterInterface
	.db 0x01                         ; bSlaveInterface0

	;; CDC Call Management (CM) Functional Descriptor
	.db 0x05                         ; Descriptor Size in Bytes (5)
	.db 0x24                         ; CS_Interface
	.db 0x01                         ; CM Functional Descriptor
	.db 0x01                         ; bmCapabilities
	.db 0x01                         ; bDataInterface

; endpoint 1 in
	.db DSCR_ENDPOINT_LEN            ; Descriptor length
	.db DSCR_ENDPOINT_TYPE           ; Descriptor type
	.db 0x81                         ; Endpoint number (ep1), and direction (in)
	.db ENDPOINT_TYPE_INT            ; Endpoint type
	.db 0x40                         ; Maximum packet size (LSB)
	.db 0x00                         ; Max packet size (MSB) == 512 bytes
	.db 0x40                         ; Polling interval

	;; CDC Virtual COM Port Data Interface Descriptor
	; data endpoints
	.db	DSCR_INTERFACE_LEN
	.db	DSCR_INTERFACE_TYPE
	.db	0x01				; index
	.db	0x00				; alt setting idx
	.db	0x02				; n endpoints
	.db	0x0a				; class
	.db	0x00
	.db	0x00
	.db	0x00				; string index

; endpoint 2 out
	.db DSCR_ENDPOINT_LEN            ; Descriptor length
	.db DSCR_ENDPOINT_TYPE           ; Descriptor type
	.db 0x02                         ; Endpoint number (ep2), and direction (out)
	.db ENDPOINT_TYPE_BULK           ; Endpoint type
	.db 0x40                         ; Maximum packet size (LSB)
	.db 0x00                         ; Max packet size (MSB) == 64 bytes
	.db 0x00                         ; Polling interval

; endpoint 6 in
	.db DSCR_ENDPOINT_LEN            ; Descriptor length
	.db DSCR_ENDPOINT_TYPE           ; Descriptor type
	.db 0x86                         ; Endpoint number (ep6), and direction (in)
	.db ENDPOINT_TYPE_BULK           ; Endpoint type
	.db 0x40                         ; Maximum packet size (LSB)
	.db 0x00                         ; Max packet size (MSB) == 64 bytes
	.db 0x00                         ; Polling interval

fullspd_dscr_realend:

.even
_dev_strings:
; sample string
_string0:
	.db	string0end-_string0 ; len
	.db	DSCR_STRING_TYPE
	.db 0x09, 0x04     ; who knows
string0end:
; add more strings here

_string1:
	.db string1end-_string1
	.db DSCR_STRING_TYPE
	.ascii 'H'
	.db 0
	.ascii 'i'
	.db 0
string1end:

_string2:
	.db string2end-_string2
	.db DSCR_STRING_TYPE
	.ascii 'T'
	.db 0
	.ascii 'h'
	.db 0
	.ascii 'e'
	.db 0
	.ascii 'r'
	.db 0
	.ascii 'e'
	.db 0
string2end:

_string3:
    .db string3end-_string3
    .db DSCR_STRING_TYPE
_dev_serial:
    .ascii '0'
    .db 0
    .ascii '1'
    .db 0
    .ascii '2'
    .db 0
    .ascii '3'
    .db 0
    .ascii '4'
    .db 0
    .ascii '5'
    .db 0
    .ascii '6'
    .db 0
    .ascii '7'
    .db 0
    .ascii '8'
    .db 0
    .ascii '9'
    .db 0
    .ascii 'a'
    .db 0
    .ascii 'b'
    .db 0
    .ascii 'c'
    .db 0
    .ascii 'd'
    .db 0
    .ascii 'e'
    .db 0
    .ascii 'f'
    .db 0
string3end:

_dev_strings_end:
	.dw 0x0000   ; just in case someone passes an index higher than the end to the firmware
