

# TODO

```c
    /* FTDI FT2232C requirecments */
    /** FT2232C interface number: 0 or 1 */
    int interface;   /* 0 or 1 */
    /** FT2232C index number: 1 or 2 */
    int index;       /* 1 or 2 */
    /* Endpoints */
    /** FT2232C end points: 1 or 2 */
    int in_ep;
    int out_ep;      /* 1 or 2 */

    /** Bitbang mode. 1: (default) Normal bitbang mode, 2: FT2232C SPI bitbang mode */
    unsigned char bitbang_mode;
```


`libusb_control_transfer`

 * bmRequestType   -- `FTDI_DEVICE_OUT_REQTYPE` | `FTDI_DEVICE_IN_REQTYPE`
 * bRequest        -- `SIO_xxxx_?REQUEST?`
 * wValue          -- value field for setup packet
 * wIndex          -- index field for setup packet
 * data            -- extra data
 * wLength         -- packet length size, 


 * `ftdi_set_baudrate` - 
   * bmRequestType = `FTDI_DEVICE_OUT_REQTYPE` 
   * bRequest = `SIO_SET_BAUDRATE_REQUEST`
   * wValue / wValue == baudrate?
   * No data


 * `ftdi_set_line_property / ftd_set_line_property2`
   * `OUT_REQTYPE`
   * `SIO_SET_DATA_REQUEST`
   * value == bitfield
     * bits 10..8 == parity?
     * bits 12..11 == stop bit
     * bit  14 == break on/off

 * `ftdi_write_data`

```c
        if (libusb_bulk_transfer(ftdi->usb_dev, ftdi->in_ep, buf+offset, write_size, &actual_length, ftdi->usb_write_timeout) < 0)
```

 * `ftdi_read_data_cb`
   * FTDI status bits?


 * `ftdi_set_bitmode` || `ftdi_disable_bitbang` (bitmask == 0)

  * `FTDI_DEVICE_OUT_REQTYPE`
  * `SIO_SET_BITMODE_REQUEST`
  * value == bitmask?
  * index == ??

 * `ftdi_poll_modem_status`
  * TODO
