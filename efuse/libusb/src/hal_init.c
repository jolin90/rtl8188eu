#include <unistd.h>

#include "types.h"
#include "rtl8188eu_spec.h"
#include "io.h"
#include "efuse.h"
#include "hal.h"
#include "pwrseqcmd.h"
#include "pwrseq.h"

#define		EFUSE_MAP_LEN_88E			512
#define		EFUSE_MAX_SECTION_88E		64
#define		EFUSE_MAX_SECTION_BASE		16
#define		EFUSE_MAX_WORD_UNIT			4
#define		EFUSE_REAL_CONTENT_LEN_88E	256
#define		PGPKT_DATA_SIZE				8
#define		EFUSE_OOB_PROTECT_BYTES_88E (2+8)

#define AVAILABLE_EFUSE_ADDR(addr)	(addr < EFUSE_REAL_CONTENT_LEN_88E)
#define BIT(nr)						(1UL << (nr))
#define EXT_HEADER(header) ((header & 0x1F) == 0x0F)

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })

static void dump_chip_info(struct HAL_VERSION chip_vers)
{
	uint cnt = 0;
	char buf[128];

	cnt += sprintf((buf + cnt), "Chip Version Info: CHIP_8188E_");
	cnt += sprintf((buf + cnt), "%s_", chip_vers.ChipType == NORMAL_CHIP ?
				   "Normal_Chip" : "Test_Chip");
	cnt +=
		sprintf((buf + cnt), "%s_",
				chip_vers.VendorType == CHIP_VENDOR_TSMC ? "TSMC" : "UMC");
	if (chip_vers.CUTVersion == A_CUT_VERSION)
		cnt += sprintf((buf + cnt), "A_CUT_");
	else if (chip_vers.CUTVersion == B_CUT_VERSION)
		cnt += sprintf((buf + cnt), "B_CUT_");
	else if (chip_vers.CUTVersion == C_CUT_VERSION)
		cnt += sprintf((buf + cnt), "C_CUT_");
	else if (chip_vers.CUTVersion == D_CUT_VERSION)
		cnt += sprintf((buf + cnt), "D_CUT_");
	else if (chip_vers.CUTVersion == E_CUT_VERSION)
		cnt += sprintf((buf + cnt), "E_CUT_");
	else
		cnt += sprintf((buf + cnt), "UNKNOWN_CUT(%d)_", chip_vers.CUTVersion);
	cnt += sprintf((buf + cnt), "1T1R_");
	cnt += sprintf((buf + cnt), "RomVer(0)\n");

	printf("%s", buf);
}

void hal_read_chip_version(struct usb_adapter *adapter)
{
	u32 value32;
	struct HAL_VERSION ChipVersion;

	value32 = usb_read32(adapter, REG_SYS_CFG);
	ChipVersion.ChipType = ((value32 & RTL_ID) ? TEST_CHIP : NORMAL_CHIP);
	ChipVersion.VendorType =
		((value32 & VENDOR_ID) ? CHIP_VENDOR_UMC : CHIP_VENDOR_TSMC);
	ChipVersion.CUTVersion =
		(value32 & CHIP_VER_RTL_MASK) >> CHIP_VER_RTL_SHIFT;

	dump_chip_info(ChipVersion);
}

static bool hal_power_on(struct usb_adapter *adapter)
{
	u16 value16;
	if (!rtl88eu_pwrseqcmdparsing(adapter, PWR_CUT_ALL_MSK,
								  rtl8188E_power_on_flow)) {
		printf("%s: run power on flow fail\n", __func__);
		return false;
	}

	/*  Enable MAC DMA/WMAC/SCHEDULE/SEC block */
	/*  Set CR bit10 to enable 32k calibration. Suggested by SD1 Gimmy. Added by tynli. 2011.08.31. */
	usb_write16(adapter, REG_CR, 0x00);	/* suggseted by zhouzhou, by page, 20111230 */

	/*  Enable MAC DMA/WMAC/SCHEDULE/SEC block */
	value16 = usb_read16(adapter, REG_CR);
	value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
				| PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
	/*  for SDIO - Set CR bit10 to enable 32k calibration. Suggested by SD1 Gimmy. Added by tynli. 2011.08.31. */

	usb_write16(adapter, REG_CR, value16);

	return true;
}

static void Reset88E_8051(struct usb_adapter *adapter)
{
	u8 u1bTmp;

	u1bTmp = usb_read8(adapter, REG_SYS_FUNC_EN + 1);
	usb_write8(adapter, REG_SYS_FUNC_EN + 1, u1bTmp & (~BIT(2)));
	usb_write8(adapter, REG_SYS_FUNC_EN + 1, u1bTmp | (BIT(2)));
}

static void iol_mode_enable(struct usb_adapter *adapter, u8 enable)
{
	u8 reg_0xf0 = 0;

	if (enable) {
		/* Enable initial offload */
		reg_0xf0 = usb_read8(adapter, REG_SYS_CFG);
		usb_write8(adapter, REG_SYS_CFG, reg_0xf0 | SW_OFFLOAD_EN);

		Reset88E_8051(adapter);
	} else {
		/* disable initial offload */
		reg_0xf0 = usb_read8(adapter, REG_SYS_CFG);
		usb_write8(adapter, REG_SYS_CFG, reg_0xf0 & ~SW_OFFLOAD_EN);
	}
}

static bool iol_execute(struct usb_adapter *adapter, u8 control)
{
	bool status = false;
	u8 reg_0x88 = 0;

	control = control & 0x0f;
	reg_0x88 = usb_read8(adapter, REG_HMEBOX_E0);
	usb_write8(adapter, REG_HMEBOX_E0, reg_0x88 | control);

	while ((reg_0x88 = usb_read8(adapter, REG_HMEBOX_E0)) & control)
		usleep(1000 * 1000);

	reg_0x88 = usb_read8(adapter, REG_HMEBOX_E0);
	status = (reg_0x88 & control) ? false : true;

	if (reg_0x88 & control << 4)
		status = false;

	return status;
}

/* beacon head, where FW store len(2-byte) and efuse physical map.buffer to store efuse physical map */
/* for efuse content: the max byte to read. will update to byte read */
static void efuse_read_phymap_from_txpktbuf(struct usb_adapter *adapter,
											int bcnhead, u8 * content,
											u16 * size)
{
	u16 dbg_addr = 0;
	u8 reg_0x143 = 0;
	u32 lo32 = 0, hi32 = 0;
	u16 len = 0, count = 0;
	int i = 0;
	u16 limit = *size;

	u8 *pos = content;

	if (bcnhead < 0)			/* if not valid */
		bcnhead = usb_read8(adapter, REG_TDECTRL + 1);

	printf("%s bcnhead:%d\n", __func__, bcnhead);

	usb_write8(adapter, REG_PKT_BUFF_ACCESS_CTRL, TXPKT_BUF_SELECT);

	dbg_addr = bcnhead * 128 / 8;	/* 8-bytes addressing */

	while (1) {
		usb_write16(adapter, REG_PKTBUF_DBG_ADDR, dbg_addr + i);

		usb_write8(adapter, REG_TXPKTBUF_DBG, 0);
		while (!(reg_0x143 = usb_read8(adapter, REG_TXPKTBUF_DBG)))
			usleep(1000);

		lo32 = usb_read32(adapter, REG_PKTBUF_DBG_DATA_L);
		hi32 = usb_read32(adapter, REG_PKTBUF_DBG_DATA_H);

		if (i == 0) {
			usb_read8(adapter, REG_PKTBUF_DBG_DATA_L);
			usb_read8(adapter, REG_PKTBUF_DBG_DATA_L + 1);
			len = *((u16 *) & lo32);
			limit = min_t(u16, len - 2, limit);
			memcpy(pos, ((u8 *) & lo32) + 2,
				   (limit >= count + 2) ? 2 : limit - count);
			count += (limit >= count + 2) ? 2 : limit - count;
			pos = content + count;
		} else {
			memcpy(pos, ((u8 *) & lo32),
				   (limit >= count + 4) ? 4 : limit - count);
			count += (limit >= count + 4) ? 4 : limit - count;
			pos = content + count;
		}

		if (limit > count && len - 2 > count) {
			memcpy(pos, (u8 *) & hi32,
				   (limit >= count + 4) ? 4 : limit - count);
			count += (limit >= count + 4) ? 4 : limit - count;
			pos = content + count;
		}

		if (limit <= count || len - 2 <= count)
			break;
		i++;
	}

	usb_write8(adapter, REG_PKT_BUFF_ACCESS_CTRL, DISABLE_TRXPKT_BUF_ACCESS);
	*size = count;
	printf("%s read count:%u\n", __func__, *size);
}

static inline void dump_data(u8 * data, int size)
{

#if 0
	int i, j;

	printf("########    ");
	for (i = 0; i < (0x0f + 1); i++) {
		printf("0x%02x, ", i);
	}
	printf("\n-------------------------------");
	printf("---------------------------------");
	printf("---------------------------------");
	printf("-------------------------------\n");

	printf("0x000 =>    ");
	for (i = 0, j = 0; i < size; i++) {
		printf("%02X ", data[i]);
		if (!(++j % 16)) {
			printf("\n");
			if (j != size)
				printf("0x%03x =>    ", j);
		}
	}
	printf("\n");
#else
	int i, j;
	printf("OFFSET\tVALUE(hex)\n");
	for (i = 0; i < size; i += 16) {
		printf("0x%02x\t", i);
		for (j = 0; j < 8; j++) {
			printf("%02X ", data[i + j]);
		}
		printf("\t");
		for (; j < 16; j++) {
			printf("%02X ", data[i + j]);
		}
		printf("\n");
	}
	printf("\n");

#endif
}

static void efuse_phymap_to_logical(u8 * phymap, u16 _offset, u16 _size_byte,
									u8 * pbuf)
{
	unsigned int i, j;
	u16 efuse_addr = 0;
	u8 efuse_data, word_en, offset;

	u8 efuseTbl[EFUSE_MAP_LEN_88E];
	u16 efuseWord[EFUSE_MAX_SECTION_88E][EFUSE_MAX_WORD_UNIT];

	memset(efuseWord, 0xff, sizeof(efuseWord));

	while (((efuse_data = phymap[efuse_addr]) != 0xff)
		   && AVAILABLE_EFUSE_ADDR(efuse_addr)) {
		if (EXT_HEADER(efuse_data)) {
			offset = efuse_data;
			efuse_data = phymap[++efuse_addr];
			offset = ((efuse_data & 0xf0) >> 1) | ((offset & 0xe0) >> 5);
			word_en = efuse_data & 0x0f;
		} else {
			offset = (efuse_data >> 4) & 0x0f;
			word_en = efuse_data & 0x0f;
		}

		if (offset < EFUSE_MAX_SECTION_88E) {
			for (i = 0; i < EFUSE_MAX_WORD_UNIT; i++) {
				if (!(word_en & 0x01)) {
					efuseWord[offset][i] = phymap[++efuse_addr];
					efuseWord[offset][i] |= (u16) phymap[++efuse_addr] << 8;
				}
				word_en >>= 1;
			}
		}
		efuse_addr++;
	}

	for (i = 0; i < EFUSE_MAX_SECTION_88E; i++) {
		for (j = 0; j < EFUSE_MAX_WORD_UNIT; j++) {
			efuseTbl[(i * 8) + (j * 2)] = (efuseWord[i][j] & 0xff);
			efuseTbl[(i * 8) + ((j * 2) + 1)] = ((efuseWord[i][j] >> 8) & 0xff);
		}
	}

	for (i = 0; i < _size_byte; i++)
		pbuf[i] = efuseTbl[_offset + i];
}

static void ReadEFuseByte(struct usb_adapter *adapter, u16 _offset, u8 * pbuf)
{
	u32 value32;
	u8 readbyte;
	u16 retry;

	//Write Address
	usb_write8(adapter, EFUSE_CTRL + 1, (_offset & 0xff));
	readbyte = usb_read8(adapter, EFUSE_CTRL + 2);
	usb_write8(adapter, EFUSE_CTRL + 2,
			   ((_offset >> 8) & 0x03) | (readbyte & 0xfc));

	//Write bit 32 0
	readbyte = usb_read8(adapter, EFUSE_CTRL + 3);
	usb_write8(adapter, EFUSE_CTRL + 3, (readbyte & 0x7f));

	//Check bit 32 read-ready
	retry = 0;
	value32 = usb_read32(adapter, EFUSE_CTRL);
	while (!(((value32 >> 24) & 0xff) & 0x80) && (retry < 10000)) {
		value32 = usb_read32(adapter, EFUSE_CTRL);
		retry++;
	}

	usleep(50);
	value32 = usb_read32(adapter, EFUSE_CTRL);

	*pbuf = (u8) (value32 & 0xff);
}

static void Hal_EfuseReadEFuse88E(struct usb_adapter *adapter)
{
	int i;
	u8 buf[256];

	for (i = 0; i < 256; i++)
		ReadEFuseByte(adapter, i, &buf[i]);

	dump_data(buf, 256);
}

static bool iol_read_efuse(struct usb_adapter *adapter, u8 txpktbuf_bndy,
						   u16 offset, u16 size_byte, u8 * logical_map)
{
	bool status = false;
	u8 physical_map[256];
	u16 size = 512;

	usb_write8(adapter, REG_TDECTRL + 1, txpktbuf_bndy);
	memset(physical_map, 0xFF, sizeof(physical_map));
	usb_write8(adapter, REG_PKT_BUFF_ACCESS_CTRL, TXPKT_BUF_SELECT);
	status = iol_execute(adapter, CMD_READ_EFUSE_MAP);
	if (status == true)
		efuse_read_phymap_from_txpktbuf(adapter, txpktbuf_bndy, physical_map,
										&size);
	dump_data(physical_map, sizeof(physical_map));

	efuse_phymap_to_logical(physical_map, offset, size_byte, logical_map);
	dump_data(logical_map, size_byte);

	return status;
}

static void efuse_ReadEFuse(struct usb_adapter *adapter, u16 _offset,
							u16 _size_byte, u8 * pbuf)
{
	hal_power_on(adapter);
	iol_mode_enable(adapter, 1);
	iol_read_efuse(adapter, 0, _offset, _size_byte, pbuf);
	iol_mode_enable(adapter, 0);
}

static void hal_InitPGData88E(struct usb_adapter *adapter)
{
	struct eeprom_priv *pEEPROM = &adapter->eeprom_priv;
	memset(pEEPROM->efuse_eeprom_data, 0xFF, EFUSE_MAP_LEN_88E);

	if ((!pEEPROM->bautoload_fail_flag) && (!pEEPROM->EepromOrEfuse)) {
		Efuse_PowerSwitch(adapter, false, true);
		efuse_ReadEFuse(adapter, 0, EFUSE_MAP_LEN_88E,
						pEEPROM->efuse_eeprom_data);
		Hal_EfuseReadEFuse88E(adapter);
		Efuse_PowerSwitch(adapter, false, false);
	}
}

void hal_read_chip_info(struct usb_adapter *adapter)
{
	struct eeprom_priv *eeprom = &adapter->eeprom_priv;
	u8 eeValue;

	/* check system boot selection */
	eeValue = usb_read8(adapter, REG_9346CR);
	eeprom->EepromOrEfuse = (eeValue & BOOT_FROM_EEPROM) ? true : false;
	eeprom->bautoload_fail_flag = (eeValue & EEPROM_EN) ? false : true;

	printf("Boot from %s, Autoload %s !\n",
		   (eeprom->EepromOrEfuse ? "EEPROM" : "EFUSE"),
		   (eeprom->bautoload_fail_flag ? "Fail" : "OK"));

	hal_InitPGData88E(adapter);
}
