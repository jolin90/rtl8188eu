/*
 * Copyright 2017 Jolin Zhang <jolinzhang90@gmail.com>
 */

#include "usb_ops.h"

static int libusb_control_msg(libusb_device_handle * dev_handle,
							  uint8_t bmRequestType, uint8_t bRequest,
							  uint16_t wValue, uint16_t wIndex,
							  unsigned char *data, uint16_t wLength,
							  unsigned int timeout)
{
	int status, retry_times = 0;

	while (retry_times++ < 10) {
		status =
			libusb_control_transfer(dev_handle, bmRequestType, bRequest, wValue,
									wIndex, data, wLength, timeout);
		if ((status == wLength)
			|| (wValue >= FW_8188E_START_ADDRESS
				&& wValue <= FW_8188E_END_ADDRESS))
			break;
	}

	return status;
}

u8 _usb_read8(struct usb_adapter * adapter, u32 addr)
{
	struct libusb_device_handle *devh = adapter->devh;
	uint8_t bmRequestType = LIBUSB_REQUEST_VENDOR_READ;
	uint8_t bRequest = LIBUSB_REQUEST_SET_ADDRESS;
	uint16_t wValue = addr & 0xffff;
	uint16_t wIndex = 0;
	uint8_t value8 = 0;
	uint16_t wLength = 1;
	unsigned int timeout = 0;

	libusb_control_msg(devh, bmRequestType, bRequest, wValue, wIndex,
					   (uint8_t *) & value8, wLength, timeout);

	return value8;
}

u16 _usb_read16(struct usb_adapter * adapter, u32 addr)
{
	struct libusb_device_handle *devh = adapter->devh;
	uint8_t bmRequestType = LIBUSB_REQUEST_VENDOR_READ;
	uint8_t bRequest = LIBUSB_REQUEST_SET_ADDRESS;
	uint16_t wValue = addr & 0xffff;
	uint16_t wIndex = 0;
	uint16_t value16 = 0;
	uint16_t wLength = 2;
	unsigned int timeout = 0;

	libusb_control_msg(devh, bmRequestType, bRequest, wValue, wIndex,
					   (uint8_t *) & value16, wLength, timeout);

	return value16;
}

u32 _usb_read32(struct usb_adapter * adapter, u32 addr)
{
	struct libusb_device_handle *devh = adapter->devh;
	uint8_t bmRequestType = LIBUSB_REQUEST_VENDOR_READ;
	uint8_t bRequest = LIBUSB_REQUEST_SET_ADDRESS;
	uint16_t wValue = addr & 0xffff;
	uint16_t wIndex = 0;
	uint32_t value32 = 0;
	uint16_t wLength = 4;
	unsigned int timeout = 0;

	libusb_control_msg(devh, bmRequestType, bRequest, wValue, wIndex,
					   (uint8_t *) & value32, wLength, timeout);

	return value32;
}

int _usb_write8(struct usb_adapter *adapter, u32 addr, u8 val)
{
	struct libusb_device_handle *devh = adapter->devh;
	uint8_t bmRequestType = LIBUSB_REQUEST_VENDOR_WRITE;
	uint8_t bRequest = LIBUSB_REQUEST_SET_ADDRESS;
	uint16_t wValue = addr & 0xffff;
	uint16_t wIndex = 0;
	uint8_t value8 = val;
	uint16_t wLength = sizeof(uint8_t);
	unsigned int timeout = 0;

	return libusb_control_msg(devh, bmRequestType, bRequest, wValue,
							  wIndex, (uint8_t *) & value8, wLength, timeout);
}

int _usb_write16(struct usb_adapter *adapter, u32 addr, u16 val)
{
	struct libusb_device_handle *devh = adapter->devh;
	uint8_t bmRequestType = LIBUSB_REQUEST_VENDOR_WRITE;
	uint8_t bRequest = LIBUSB_REQUEST_SET_ADDRESS;
	uint16_t wValue = addr & 0xffff;
	uint16_t wIndex = 0;
	uint16_t value16 = val;
	uint16_t wLength = sizeof(uint16_t);
	unsigned int timeout = 0;

	return libusb_control_msg(devh, bmRequestType, bRequest, wValue,
							  wIndex, (uint8_t *) & value16, wLength, timeout);
}

int _usb_write32(struct usb_adapter *adapter, u32 addr, u32 val)
{
	struct libusb_device_handle *devh = adapter->devh;
	uint8_t bmRequestType = LIBUSB_REQUEST_VENDOR_WRITE;
	uint8_t bRequest = LIBUSB_REQUEST_SET_ADDRESS;
	uint16_t wValue = addr & 0xffff;
	uint16_t wIndex = 0;
	uint32_t value32 = val;
	uint16_t wLength = sizeof(uint32_t);
	unsigned int timeout = 0;

	return libusb_control_msg(devh, bmRequestType, bRequest, wValue,
							  wIndex, (uint8_t *) & value32, wLength, timeout);
}
