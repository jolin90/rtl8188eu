#include <unistd.h>

#include "pwrseqcmd.h"
#include "io.h"

#define udelay usleep

/* This routine deals with the Power Configuration CMDs parsing
 * for RTL8723/RTL8188E Series IC.
 */
u8 rtl88eu_pwrseqcmdparsing(struct usb_adapter *padapter, u8 cut_vers,
							struct wl_pwr_cfg pwrseqcmd[])
{
	struct wl_pwr_cfg pwrcfgcmd = { 0 };
	u8 poll_bit = false;
	u32 aryidx = 0;
	u8 value = 0;
	u32 offset = 0;
	u32 poll_count = 0;			/*  polling autoload done. */
	u32 max_poll_count = 5000;

	do {
		pwrcfgcmd = pwrseqcmd[aryidx];

		printf("rtl88eu_pwrseqcmdparsing: offset(%#x) cut_msk(%#x)"
			   " cmd(%#x)"
			   "msk(%#x) value(%#x)\n",
			   GET_PWR_CFG_OFFSET(pwrcfgcmd),
			   GET_PWR_CFG_CUT_MASK(pwrcfgcmd),
			   GET_PWR_CFG_CMD(pwrcfgcmd),
			   GET_PWR_CFG_MASK(pwrcfgcmd), GET_PWR_CFG_VALUE(pwrcfgcmd));

		/* Only Handle the command whose CUT is matched */
		if (GET_PWR_CFG_CUT_MASK(pwrcfgcmd) & cut_vers) {
			switch (GET_PWR_CFG_CMD(pwrcfgcmd)) {
			case PWR_CMD_READ:
				printf("rtl88eu_pwrseqcmdparsing: PWR_CMD_READ\n");
				break;
			case PWR_CMD_WRITE:
				printf("rtl88eu_pwrseqcmdparsing: PWR_CMD_WRITE\n");
				offset = GET_PWR_CFG_OFFSET(pwrcfgcmd);

				/*  Read the value from system register */
				value = usb_read8(padapter, offset);

				value &= ~(GET_PWR_CFG_MASK(pwrcfgcmd));
				value |= (GET_PWR_CFG_VALUE(pwrcfgcmd) &
						  GET_PWR_CFG_MASK(pwrcfgcmd));

				/*  Write the value back to system register */
				usb_write8(padapter, offset, value);
				break;
			case PWR_CMD_POLLING:
				printf("rtl88eu_pwrseqcmdparsing: PWR_CMD_POLLING\n");

				poll_bit = false;
				offset = GET_PWR_CFG_OFFSET(pwrcfgcmd);
				do {
					value = usb_read8(padapter, offset);
					value &= GET_PWR_CFG_MASK(pwrcfgcmd);

					if (value == (GET_PWR_CFG_VALUE(pwrcfgcmd) &
								  GET_PWR_CFG_MASK(pwrcfgcmd)))
						poll_bit = true;
					else
						udelay(10);

					if (poll_count++ > max_poll_count) {
						printf("Fail to polling Offset[%#x]\n", offset);
						return false;
					}
				} while (!poll_bit);
				break;
			case PWR_CMD_DELAY:
				printf("rtl88eu_pwrseqcmdparsing: PWR_CMD_DELAY\n");
				if (GET_PWR_CFG_VALUE(pwrcfgcmd) == PWRSEQ_DELAY_US)
					udelay(GET_PWR_CFG_OFFSET(pwrcfgcmd));
				else
					udelay(GET_PWR_CFG_OFFSET(pwrcfgcmd) * 1000);
				break;
			case PWR_CMD_END:
				/* When this command is parsed, end the process */
				printf("rtl88eu_pwrseqcmdparsing: PWR_CMD_END\n");
				return true;
			default:
				printf("rtl88eu_pwrseqcmdparsing: Unknown CMD!!\n");
				break;
			}
		}

		aryidx++;				/* Add Array Index */
	} while (1);
	return true;
}
