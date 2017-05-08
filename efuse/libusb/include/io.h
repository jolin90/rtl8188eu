#ifndef _DEBUG_IO_H_
#define _DEBUG_IO_H_

#include "types.h"
#include "usb_ops.h"

u8 dbg_usb_read8(struct usb_adapter *adapter, u32 addr, const char *caller,
				 const int line);
u16 dbg_usb_read16(struct usb_adapter *adapter, u32 addr, const char *caller,
				   const int line);
u32 dbg_usb_read32(struct usb_adapter *adapter, u32 addr, const char *caller,
				   const int line);
int dbg_usb_write8(struct usb_adapter *adapter, u32 addr, u8 val,
				   const char *caller, const int line);
int dbg_usb_write16(struct usb_adapter *adapter, u32 addr, u16 val,
					const char *caller, const int line);
int dbg_usb_write32(struct usb_adapter *adapter, u32 addr, u32 val,
					const char *caller, const int line);

//#define DBUG_IO
#ifdef DBUG_IO
#define usb_read8(adapter, addr) dbg_usb_read8((adapter), (addr), __func__, __LINE__)
#define usb_read16(adapter, addr) dbg_usb_read16((adapter), (addr), __func__, __LINE__)
#define usb_read32(adapter, addr) dbg_usb_read32((adapter), (addr), __func__, __LINE__)
#define usb_write8(adapter, addr, val) dbg_usb_write8((adapter), (addr), (val), __func__, __LINE__)
#define usb_write16(adapter, addr, val) dbg_usb_write16((adapter), (addr), (val), __func__, __LINE__)
#define usb_write32(adapter, addr, val) dbg_usb_write32((adapter), (addr), (val), __func__, __LINE__)
#else
#define usb_read8(adapter, addr) _usb_read8((adapter), (addr))
#define usb_read16(adapter, addr) _usb_read16((adapter), (addr))
#define usb_read32(adapter, addr) _usb_read32((adapter), (addr))
#define usb_write8(adapter, addr, val) _usb_write8((adapter), (addr), (val))
#define usb_write16(adapter, addr, val) _usb_write16((adapter), (addr), (val))
#define usb_write32(adapter, addr, val) _usb_write32((adapter), (addr), (val))
#endif

#endif
