#ifndef __RTL8188EU_SPEC_H__
#define __RTL8188EU_SPEC_H__

#define RTL_ID					BIT(23)	/*  TestChip ID, 1:Test(RLE); 0:MP(RL) */
#define VENDOR_ID				BIT(19)

#define CHIP_VER_RTL_MASK		0xF000	/* Bit 12 ~ 15 */
#define CHIP_VER_RTL_SHIFT		12

/*	0x0000h ~ 0x00FFh	System Configuration */
#define REG_SYS_ISO_CTRL		0x0000
#define REG_SYS_FUNC_EN			0x0002
#define REG_APS_FSMCO			0x0004
#define REG_SYS_CLKR			0x0008
#define REG_9346CR				0x000A
#define REG_EE_VPD				0x000C
#define REG_AFE_MISC			0x0010
#define REG_SPS0_CTRL			0x0011
#define REG_SPS_OCP_CFG			0x0018
#define REG_RSV_CTRL			0x001C
#define REG_RF_CTRL				0x001F
#define REG_LDOA15_CTRL			0x0020
#define REG_LDOV12D_CTRL		0x0021
#define REG_LDOHCI12_CTRL		0x0022
#define REG_LPLDO_CTRL			0x0023
#define REG_AFE_XTAL_CTRL		0x0024
#define REG_AFE_PLL_CTRL		0x0028
#define REG_APE_PLL_CTRL_EXT	0x002c
#define REG_EFUSE_CTRL			0x0030
#define REG_EFUSE_TEST			0x0034
#define REG_GPIO_MUXCFG			0x0040
#define REG_GPIO_IO_SEL			0x0042
#define REG_MAC_PINMUX_CFG		0x0043
#define REG_GPIO_PIN_CTRL		0x0044
#define REG_GPIO_INTM			0x0048
#define REG_LEDCFG0				0x004C
#define REG_LEDCFG1				0x004D
#define REG_LEDCFG2				0x004E
#define REG_LEDCFG3				0x004F
#define REG_FSIMR				0x0050
#define REG_FSISR				0x0054
#define REG_HSIMR				0x0058
#define REG_HSISR				0x005c
#define REG_GPIO_PIN_CTRL_2		0x0060	/*  RTL8723 WIFI/BT/GPS * Multi-Function GPIO Pin Control. */
#define REG_GPIO_IO_SEL_2		0x0062	/*  RTL8723 WIFI/BT/GPS * Multi-Function GPIO Select. */
#define REG_BB_PAD_CTRL			0x0064
#define REG_MULTI_FUNC_CTRL		0x0068	/*  RTL8723 WIFI/BT/GPS * Multi-Function control source. */
#define REG_GPIO_OUTPUT			0x006c
#define REG_AFE_XTAL_CTRL_EXT	0x0078	/* RTL8188E */
#define REG_XCK_OUT_CTRL		0x007c	/* RTL8188E */
#define REG_MCUFWDL				0x0080
#define REG_WOL_EVENT			0x0081	/* RTL8188E */
#define REG_MCUTSTCFG			0x0084
#define REG_HMEBOX_E0			0x0088
#define REG_HMEBOX_E1			0x008A
#define REG_HMEBOX_E2			0x008C
#define REG_HMEBOX_E3			0x008E
#define REG_HMEBOX_EXT_0		0x01F0
#define REG_HMEBOX_EXT_1		0x01F4
#define REG_HMEBOX_EXT_2		0x01F8
#define REG_HMEBOX_EXT_3		0x01FC
#define REG_HIMR_88E			0x00B0
#define REG_HISR_88E			0x00B4
#define REG_HIMRE_88E			0x00B8
#define REG_HISRE_88E			0x00BC
#define REG_EFUSE_ACCESS		0x00CF	/*  Efuse access protection * for RTL8723 */
#define REG_BIST_SCAN			0x00D0
#define REG_BIST_RPT			0x00D4
#define REG_BIST_ROM_RPT		0x00D8
#define REG_USB_SIE_INTF		0x00E0
#define REG_PCIE_MIO_INTF		0x00E4
#define REG_PCIE_MIO_INTD		0x00E8
#define REG_HPON_FSM			0x00EC
#define REG_SYS_CFG				0x00F0
#define REG_GPIO_OUTSTS			0x00F4	/*  For RTL8723 only. */
#define REG_TYPE_ID				0x00FC

#define REG_MAC_PHY_CTRL_NORMAL		0x00f8

/*	0x0100h ~ 0x01FFh	MACTOP General Configuration */
#define REG_CR						0x0100
#define REG_PBP						0x0104
#define REG_PKT_BUFF_ACCESS_CTRL	0x0106
#define REG_TRXDMA_CTRL				0x010C
#define REG_TRXFF_BNDY				0x0114
#define REG_TRXFF_STATUS			0x0118
#define REG_RXFF_PTR				0x011C
/* define REG_HIMR			0x0120 */
/* define REG_HISR			0x0124 */
#define REG_HIMRE			0x0128
#define REG_HISRE			0x012C
#define REG_CPWM			0x012F
#define REG_FWIMR			0x0130
#define REG_FTIMR			0x0138
#define REG_FWISR			0x0134
#define REG_PKTBUF_DBG_CTRL		0x0140
#define REG_PKTBUF_DBG_ADDR		(REG_PKTBUF_DBG_CTRL)
#define REG_RXPKTBUF_DBG		(REG_PKTBUF_DBG_CTRL+2)
#define REG_TXPKTBUF_DBG		(REG_PKTBUF_DBG_CTRL+3)
#define REG_RXPKTBUF_CTRL		(REG_PKTBUF_DBG_CTRL+2)
#define REG_PKTBUF_DBG_DATA_L		0x0144
#define REG_PKTBUF_DBG_DATA_H		0x0148

#define REG_TC0_CTRL			0x0150
#define REG_TC1_CTRL			0x0154
#define REG_TC2_CTRL			0x0158
#define REG_TC3_CTRL			0x015C
#define REG_TC4_CTRL			0x0160
#define REG_TCUNIT_BASE			0x0164
#define REG_MBIST_START			0x0174
#define REG_MBIST_DONE			0x0178
#define REG_MBIST_FAIL			0x017C
#define REG_32K_CTRL			0x0194	/* RTL8188E */
#define REG_C2HEVT_MSG_NORMAL	0x01A0
#define REG_C2HEVT_CLEAR		0x01AF
#define REG_MCUTST_1			0x01c0
#define REG_FMETHR				0x01C8
#define REG_HMETFR				0x01CC
#define REG_HMEBOX_0			0x01D0
#define REG_HMEBOX_1			0x01D4
#define REG_HMEBOX_2			0x01D8
#define REG_HMEBOX_3			0x01DC
#define REG_LLT_INIT			0x01E0

/*	0x0200h ~ 0x027Fh	TXDMA Configuration */
#define REG_RQPN				0x0200
#define REG_FIFOPAGE			0x0204
#define REG_TDECTRL				0x0208
#define REG_TXDMA_OFFSET_CHK	0x020C
#define REG_TXDMA_STATUS		0x0210
#define REG_RQPN_NPQ			0x0214

/* 2EFUSE_CTRL */
#define ALD_EN					BIT(18)
#define EF_PD					BIT(19)
#define EF_FLAG					BIT(31)

#define EF_TRPT					BIT(7)	/* 2 EFUSE_TEST (For RTL8723 partially) */
#define EF_CELL_SEL				(BIT(8)|BIT(9))	/*  00: Wifi Efuse, 01: BT Efuse0, 10: BT Efuse1, 11: BT Efuse2 */
#define LDOE25_EN				BIT(31)
#define EFUSE_SEL(x)			(((x) & 0x3) << 8)
#define EFUSE_SEL_MASK			0x300
#define EFUSE_WIFI_SEL_0		0x0
#define EFUSE_BT_SEL_0			0x1
#define EFUSE_BT_SEL_1			0x2
#define EFUSE_BT_SEL_2			0x3

#define EFUSE_ACCESS_ON			0x69	/*  For RTL8723 only. */
#define EFUSE_ACCESS_OFF		0x00	/*  For RTL8723 only. */

/* 2 SYS_ISO_CTRL */
#define ISO_MD2PP			BIT(0)
#define ISO_UA2USB			BIT(1)
#define ISO_UD2CORE			BIT(2)
#define ISO_PA2PCIE			BIT(3)
#define ISO_PD2CORE			BIT(4)
#define ISO_IP2MAC			BIT(5)
#define ISO_DIOP			BIT(6)
#define ISO_DIOE			BIT(7)
#define ISO_EB2CORE			BIT(8)
#define ISO_DIOR			BIT(9)
#define PWC_EV12V			BIT(15)

/* 2 SYS_FUNC_EN */
#define FEN_BBRSTB			BIT(0)
#define FEN_BB_GLB_RSTn		BIT(1)
#define FEN_USBA			BIT(2)
#define FEN_UPLL			BIT(3)
#define FEN_USBD			BIT(4)
#define FEN_DIO_PCIE		BIT(5)
#define FEN_PCIEA			BIT(6)
#define FEN_PPLL			BIT(7)
#define FEN_PCIED			BIT(8)
#define FEN_DIOE			BIT(9)
#define FEN_CPUEN			BIT(10)
#define FEN_DCORE			BIT(11)
#define FEN_ELDR			BIT(12)
#define FEN_DIO_RF			BIT(13)
#define FEN_HWPDN			BIT(14)
#define FEN_MREGEN			BIT(15)

/* 2 APS_FSMCO */
#define PFM_LDALL			BIT(0)
#define PFM_ALDN			BIT(1)
#define PFM_LDKP			BIT(2)
#define PFM_WOWL			BIT(3)
#define EnPDN				BIT(4)
#define PDN_PL				BIT(5)
#define APFM_ONMAC			BIT(8)
#define APFM_OFF			BIT(9)
#define APFM_RSM			BIT(10)
#define AFSM_HSUS			BIT(11)
#define AFSM_PCIE			BIT(12)
#define APDM_MAC			BIT(13)
#define APDM_HOST			BIT(14)
#define APDM_HPDN			BIT(15)
#define RDY_MACON			BIT(16)
#define SUS_HOST			BIT(17)
#define ROP_ALD				BIT(20)
#define ROP_PWR				BIT(21)
#define ROP_SPS				BIT(22)
#define SOP_MRST			BIT(25)
#define SOP_FUSE			BIT(26)
#define SOP_ABG				BIT(27)
#define SOP_AMB				BIT(28)
#define SOP_RCK				BIT(29)
#define SOP_A8M				BIT(30)
#define XOP_BTCK			BIT(31)

/* 2 SYS_CLKR */
#define ANAD16V_EN			BIT(0)
#define ANA8M				BIT(1)
#define MACSLP				BIT(4)
#define LOADER_CLK_EN		BIT(5)

/* 2 9346CR */

#define		BOOT_FROM_EEPROM	BIT(4)
#define		EEPROM_EN			BIT(5)

#define	EFUSE_CTRL			REG_EFUSE_CTRL	/*  E-Fuse Control. */
#define	EFUSE_TEST			REG_EFUSE_TEST	/*  E-Fuse Test. */

/*  download firmware related data structure */
#define FW_8188E_SIZE				0x4000	/* 16384,16k */
#define FW_8188E_START_ADDRESS		0x1000
#define FW_8188E_END_ADDRESS		0x1FFF	/* 0x5FFF */

#define HCI_TXDMA_EN			BIT(0)
#define HCI_RXDMA_EN			BIT(1)
#define TXDMA_EN				BIT(2)
#define RXDMA_EN				BIT(3)
#define PROTOCOL_EN				BIT(4)
#define SCHEDULE_EN				BIT(5)
#define MACTXEN					BIT(6)
#define MACRXEN					BIT(7)
#define ENSWBCN					BIT(8)
#define ENSEC					BIT(9)
#define CALTMR_EN				BIT(10)	/*  32k CAL TMR enable */

/* 2 REG_SYS_CFG */
#define XCLK_VLD			BIT(0)
#define ACLK_VLD			BIT(1)
#define UCLK_VLD			BIT(2)
#define PCLK_VLD			BIT(3)
#define PCIRSTB				BIT(4)
#define V15_VLD				BIT(5)
#define SW_OFFLOAD_EN		BIT(7)
#define SIC_IDLE			BIT(8)
#define BD_MAC2				BIT(9)
#define BD_MAC1				BIT(10)
#define IC_MACPHY_MODE		BIT(11)
#define CHIP_VER			(BIT(12)|BIT(13)|BIT(14)|BIT(15))
#define BT_FUNC				BIT(16)
#define VENDOR_ID			BIT(19)
#define PAD_HWPD_IDN		BIT(22)
#define TRP_VAUX_EN			BIT(23)	/*  RTL ID */
#define TRP_BT_EN			BIT(24)
#define BD_PKG_SEL			BIT(25)
#define BD_HCI_SEL			BIT(26)

#define MAC_ADDR_LEN				6
/*  8188E PKT_BUFF_ACCESS_CTRL value */
#define TXPKT_BUF_SELECT			0x69
#define RXPKT_BUF_SELECT			0xA5
#define DISABLE_TRXPKT_BUF_ACCESS	0x0

/* IOL config for REG_FDHM0(Reg0x88) */
#define CMD_INIT_LLT				BIT(0)
#define CMD_READ_EFUSE_MAP			BIT(1)
#define CMD_EFUSE_PATCH				BIT(2)
#define CMD_IOCONFIG				BIT(3)
#define CMD_INIT_LLT_ERR			BIT(4)
#define CMD_READ_EFUSE_MAP_ERR		BIT(5)
#define CMD_EFUSE_PATCH_ERR			BIT(6)
#define CMD_IOCONFIG_ERR			BIT(7)

#endif
