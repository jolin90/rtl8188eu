#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define EXT_HEADER(header) ((header & 0x1F) == 0x0F)

struct pgpkt {
	u8 offset;
	u8 word_en;
	u8 data[8];
	u8 word_cnts;
};

static u8 logic_map[EFUSE_MAP_LEN_88E];

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

static u8 Efuse_CalculateWordCnts(u8 word_en)
{
	u8 word_cnts = 0;

	if (!(word_en & BIT(0)))
		word_cnts++;			/*  0 : write enable */
	if (!(word_en & BIT(1)))
		word_cnts++;
	if (!(word_en & BIT(2)))
		word_cnts++;
	if (!(word_en & BIT(3)))
		word_cnts++;

	return word_cnts;
}

static u16 Efuse_GetCurrentSize()
{
	u16 efuse_addr = 0;
	u8 offset = 0, word_en = 0;
	u8 efuse_data, word_cnts = 0;

	while (((efuse_data = phymap[efuse_addr]) != 0xff)
		   && AVAILABLE_EFUSE_ADDR(efuse_addr)) {
		DBG_88E("efuse_addr=0x%02x, ", efuse_addr);

		efuse_data = phymap[efuse_addr];
		if (EXT_HEADER(efuse_data)) {
			offset = efuse_data;
			efuse_data = phymap[++efuse_addr];
			offset = ((efuse_data & 0xf0) >> 1) | ((offset & 0xe0) >> 5);
			word_en = efuse_data & 0x0f;
		} else {
			offset = (efuse_data >> 4) & 0x0f;
			word_en = efuse_data & 0x0f;
		}

		printf("offset=0x%02x, word_en=%x, ", offset, word_en);
		word_cnts = Efuse_CalculateWordCnts(word_en);
		printf("data size=0x%02x\n", (word_cnts * 2));

		/* read next header */
		efuse_addr = efuse_addr + (word_cnts * 2) + 1;
	}

	return efuse_addr;
}

static void efuse_WordEnableDataRead(u8 word_en, u8 * sourdata, u8 * targetdata)
{
	if (!(word_en & BIT(0))) {
		targetdata[0] = sourdata[0];
		targetdata[1] = sourdata[1];
	}
	if (!(word_en & BIT(1))) {
		targetdata[2] = sourdata[2];
		targetdata[3] = sourdata[3];
	}
	if (!(word_en & BIT(2))) {
		targetdata[4] = sourdata[4];
		targetdata[5] = sourdata[5];
	}
	if (!(word_en & BIT(3))) {
		targetdata[6] = sourdata[6];
		targetdata[7] = sourdata[7];
	}
}

static void hal_EfuseConstructPGPkt(u8 offset, u8 word_en, u8 * pData,
									struct pgpkt *pTargetPkt)
{
	memset((void *)pTargetPkt->data, 0xFF, sizeof(u8) * 8);
	pTargetPkt->offset = offset;
	pTargetPkt->word_en = word_en;
	efuse_WordEnableDataRead(word_en, pData, pTargetPkt->data);
	pTargetPkt->word_cnts = Efuse_CalculateWordCnts(pTargetPkt->word_en);
}

static bool hal_EfusePgPacketWriteHeader(u16 * pefuse_addr,
										 struct pgpkt *pTargetPkt)
{
	u8 pg_header;
	u16 efuse_addr = *pefuse_addr;

	DBG_88E("pTargetPkt->offset:%x\n", pTargetPkt->offset);
	if (pTargetPkt->offset >= EFUSE_MAX_SECTION_BASE) {
		/*0x07 <=> 0b111 */
		pg_header = ((pTargetPkt->offset & 0x07) << 5) | 0x0F;
		phymap[efuse_addr] = pg_header;
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, pg_header);

		/*0x78 <=> 0b01111000 */
		pg_header = ((pTargetPkt->offset & 0x78) << 1) | pTargetPkt->word_en;
		phymap[++efuse_addr] = pg_header;
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, pg_header);
	} else {
		pg_header = ((pTargetPkt->offset << 4) & 0xf0) | pTargetPkt->word_en;
		phymap[efuse_addr] = pg_header;
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, pg_header);
	}

	*pefuse_addr = ++efuse_addr;

	DBG_88E("pTargetPkt->offset:%x\n", pTargetPkt->offset);

	return true;
}

static void hal_EfusePgPacketWriteData(u16 * pefuse_addr,
									   struct pgpkt *pTargetPkt)
{
	u16 efuse_addr = *pefuse_addr;
	u8 word_en = pTargetPkt->word_en;
	u8 *data = pTargetPkt->data;

	if (!(word_en & BIT(0))) {
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[0]);
		phymap[efuse_addr++] = data[0];
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[1]);
		phymap[efuse_addr++] = data[1];
	}
	if (!(word_en & BIT(1))) {
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[2]);
		phymap[efuse_addr++] = data[2];
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[3]);
		phymap[efuse_addr++] = data[3];
	}
	if (!(word_en & BIT(2))) {
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[4]);
		phymap[efuse_addr++] = data[4];
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[5]);
		phymap[efuse_addr++] = data[5];
	}
	if (!(word_en & BIT(3))) {
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[6]);
		phymap[efuse_addr++] = data[6];
		DBG_88E("phymap[0x%x]=0x%x\n", efuse_addr, data[7]);
		phymap[efuse_addr++] = data[7];
	}
}

static bool Efuse_PgPacketWrite(u8 offset, u8 word_en, u8 * pData)
{
	struct pgpkt targetPkt;
	u16 startAddr = 0;
	u16 efuse_max_available_len =
		EFUSE_REAL_CONTENT_LEN_88E - EFUSE_OOB_PROTECT_BYTES_88E;

	hal_EfuseConstructPGPkt(offset, word_en, pData, &targetPkt);

	startAddr = Efuse_GetCurrentSize();
	DBG_88E("startAddr:0x%x\n", startAddr);
	if (startAddr >= efuse_max_available_len)
		return false;
	DBG_88E("startAddr:0x%x\n", startAddr);
	hal_EfusePgPacketWriteHeader(&startAddr, &targetPkt);
	DBG_88E("startAddr:0x%x\n", startAddr);
	hal_EfusePgPacketWriteData(&startAddr, &targetPkt);

	DBG_88E("\n");

	return true;
}

static void efuse_map_write(u8 addr, u8 count, u8 * data)
{
	int i, j, index;
	u8 newdata[PGPKT_DATA_SIZE];
	u8 word_en, offset;
	index = 0;
	offset = (addr >> 3);

	while (index < count) {

		word_en = 0x0f;
		j = (addr + index) & 0x7;
		memcpy(newdata, &logic_map[offset << 3], PGPKT_DATA_SIZE);
		for (i = j; i < PGPKT_DATA_SIZE && index < count; i++, index++) {
			if (data[index] != logic_map[addr + index]) {
				word_en &= ~BIT(i >> 1);
				newdata[i] = data[index];
			}
		}

		if (word_en != 0xF) {
			DBG_88E("offset=%d\n", (int)offset);
			Efuse_PgPacketWrite(offset, word_en, newdata);
		}

		offset++;
	}

	memset(logic_map, 0, sizeof(logic_map));
	efuse_phymap_to_logical(phymap, 0, EFUSE_MAP_LEN_88E, logic_map);
}

int main(int argc, char *argv[])
{
	memset(logic_map, 0, sizeof(logic_map));

	efuse_phymap_to_logical(phymap, 0, EFUSE_MAP_LEN_88E, logic_map);
	if (!memcmp(ok, logic_map, sizeof(logic_map)))
		printf("ok\n");
	dump_data(phymap, sizeof(phymap));
	dump_data(logic_map, sizeof(logic_map));

#if 1
	u8 data[] = {
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
	};
	efuse_map_write(0x29, sizeof(data), data);
	dump_data(logic_map, sizeof(logic_map));

	u8 data1[] = {
		0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
	};
	efuse_map_write(0x29, sizeof(data1), data1);
	dump_data(logic_map, sizeof(logic_map));
#endif

	return 0;
}
