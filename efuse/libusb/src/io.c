#include "types.h"
#include "usb_ops.h"

static u16 regs_range[][2] = {
	{0x0, 0xffff},
};

static int regs_range_num = sizeof(regs_range) / sizeof(u16) / 2;

static bool match_regs_range(u16 addr, u16 len)
{
	int i;
	for (i = 0; i < regs_range_num; i++) {
		if (addr + len > regs_range[i][0]
			&& addr <= regs_range[i][1])
			return true;
	}

	return false;
}

u8 dbg_usb_read8(struct usb_adapter * adapter, u32 addr, const char *caller,
				 const int line)
{
	u8 val = _usb_read8(adapter, addr);

	if (match_regs_range(addr, 1))
		printf("DBG_IO %s:%d rtw_read8(0x%04x) return 0x%02x\n", caller, line,
			   addr, val);

	return val;
}

u16 dbg_usb_read16(struct usb_adapter * adapter, u32 addr, const char *caller,
				   const int line)
{
	u16 val = _usb_read16(adapter, addr);

	if (match_regs_range(addr, 2))
		printf("DBG_IO %s:%d rtw_read16(0x%04x) return 0x%04x\n", caller, line,
			   addr, val);

	return val;
}

u32 dbg_usb_read32(struct usb_adapter * adapter, u32 addr, const char *caller,
				   const int line)
{
	u32 val = _usb_read32(adapter, addr);
	if (match_regs_range(addr, 4))
		printf("DBG_IO %s:%d rtw_read32(0x%04x) return 0x%08x\n", caller, line,
			   addr, val);

	return val;
}

int dbg_usb_write8(struct usb_adapter *adapter, u32 addr, u8 val,
				   const char *caller, const int line)
{
	if (match_regs_range(addr, 1))
		printf("DBG_IO %s:%d rtw_write8(0x%04x, 0x%02x)\n", caller, line, addr,
			   val);

	return _usb_write8(adapter, addr, val);
}

int dbg_usb_write16(struct usb_adapter *adapter, u32 addr, u16 val,
					const char *caller, const int line)
{
	if (match_regs_range(addr, 2))
		printf("DBG_IO %s:%d rtw_write16(0x%04x, 0x%04x)\n", caller, line, addr,
			   val);

	return _usb_write16(adapter, addr, val);
}

int dbg_usb_write32(struct usb_adapter *adapter, u32 addr, u32 val,
					const char *caller, const int line)
{
	if (match_regs_range(addr, 4))
		printf("DBG_IO %s:%d rtw_write32(0x%04x, 0x%08x)\n", caller, line, addr,
			   val);

	return _usb_write32(adapter, addr, val);
}
