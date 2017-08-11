#include "types.h"
#include "io.h"

enum {
	VOLTAGE_V25 = 0x03,
	LDOE25_SHIFT = 28,
};

void Efuse_PowerSwitch(struct usb_adapter *adapter, u8 bWrite, u8 PwrState)
{
	u8 tempval;
	u16 tmpV16;

	if (PwrState) {
		usb_write8(adapter, REG_EFUSE_ACCESS, EFUSE_ACCESS_ON);

		/*  1.2V Power: From VDDON with Power Cut(0x0000h[15]), defualt valid */
		tmpV16 = usb_read16(adapter, REG_SYS_ISO_CTRL);
		if (!(tmpV16 & PWC_EV12V)) {
			tmpV16 |= PWC_EV12V;
			usb_write16(adapter, REG_SYS_ISO_CTRL, tmpV16);
		}

		/*  Reset: 0x0000h[28], default valid */
		tmpV16 = usb_read16(adapter, REG_SYS_FUNC_EN);
		if (!(tmpV16 & FEN_ELDR)) {
			tmpV16 |= FEN_ELDR;
			usb_write16(adapter, REG_SYS_FUNC_EN, tmpV16);
		}

		/*  Clock: Gated(0x0008h[5]) 8M(0x0008h[1]) clock from ANA, default valid */
		tmpV16 = usb_read16(adapter, REG_SYS_CLKR);
		if ((!(tmpV16 & LOADER_CLK_EN)) || (!(tmpV16 & ANA8M))) {
			tmpV16 |= (LOADER_CLK_EN | ANA8M);
			usb_write16(adapter, REG_SYS_CLKR, tmpV16);
		}

		if (bWrite) {
			/*  Enable LDO 2.5V before read/write action */
			tempval = usb_read8(adapter, EFUSE_TEST + 3);
			tempval &= 0x0F;
			tempval |= (VOLTAGE_V25 << 4);
			usb_write8(adapter, EFUSE_TEST + 3, (tempval | 0x80));
		}
	} else {
		usb_write8(adapter, REG_EFUSE_ACCESS, EFUSE_ACCESS_OFF);

		if (bWrite) {
			/*  Disable LDO 2.5V after read/write action */
			tempval = usb_read8(adapter, EFUSE_TEST + 3);
			usb_write8(adapter, EFUSE_TEST + 3, (tempval & 0x7F));
		}
	}
}
