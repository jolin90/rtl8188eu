/*
 * Copyright 2017 Jolin Zhang <jolinzhang90@gmail.com>
 */

#ifndef __USB_OPS_H__
#define __USB_OPS_H__

#include <libusb.h>

#include "types.h"

#define LIBUSB_REQUEST_VENDOR_READ		(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)
#define LIBUSB_REQUEST_VENDOR_WRITE		(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)

u8 _usb_read8(struct usb_adapter *adapter, u32 addr);
u16 _usb_read16(struct usb_adapter *adapter, u32 addr);
u32 _usb_read32(struct usb_adapter *adapter, u32 addr);

int _usb_write8(struct usb_adapter *adapter, u32 addr, u8 val);
int _usb_write16(struct usb_adapter *adapter, u32 addr, u16 val);
int _usb_write32(struct usb_adapter *adapter, u32 addr, u32 val);

#endif
