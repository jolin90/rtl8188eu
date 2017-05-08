/*
 * Copyright 2017 Jolin Zhang <jolinzhang90@gmail.com>
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>

#include "rtl8188eu_spec.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

enum {
	false = 0,
	true = 1
};

#define bool int

#define BIT(nr)			(1UL << (nr))

enum HAL_CHIP_TYPE {
	TEST_CHIP = 0,
	NORMAL_CHIP = 1,
	FPGA = 2,
};

enum HAL_CUT_VERSION {
	A_CUT_VERSION = 0,
	B_CUT_VERSION = 1,
	C_CUT_VERSION = 2,
	D_CUT_VERSION = 3,
	E_CUT_VERSION = 4,
	F_CUT_VERSION = 5,
	G_CUT_VERSION = 6,
};

enum HAL_VENDOR {
	CHIP_VENDOR_TSMC = 0,
	CHIP_VENDOR_UMC = 1,
};

struct HAL_VERSION {
	enum HAL_CHIP_TYPE ChipType;
	enum HAL_CUT_VERSION CUTVersion;
	enum HAL_VENDOR VendorType;
};

struct hal_data_8188e {
	struct HAL_VERSION VersionID;
};

#define		HWSET_MAX_SIZE_512		512
#define		EFUSE_MAP_LEN_88E		512

struct eeprom_priv {
	u8 bautoload_fail_flag;
	u8 bloadfile_fail_flag;
	u8 bloadmac_fail_flag;
	u8 mac_addr[6];				/* PermanentAddress */
	u16 channel_plan;
	u8 EepromOrEfuse;
	u8 efuse_eeprom_data[HWSET_MAX_SIZE_512];
};

struct usb_adapter {
	struct libusb_device_handle *devh;
	struct eeprom_priv eeprom_priv;
};

#endif
