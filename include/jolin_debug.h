#ifndef _JOLIN_DEBUG_H_
#define _JOLIN_DEBUG_H_

#ifndef _IEEE80211_C
#ifndef _RTW_WLAN_UTIL_C_
#ifndef _RTW_RECV_C_

#undef DBG_88E
#undef RT_TRACE

#define DBG_88E(fmt, args...)                \
	do{                                      \
		pr_info("%06d - %s :"fmt,            \
				__LINE__, __func__, ##args); \
	} while (0)

#define RT_TRACE(_comp, _level, fmt)         \
	do {                                     \
		pr_info("%06d - %s :",               \
				__LINE__, __func__);         \
		printk fmt;                          \
	} while (0)

#endif
#endif
#endif

static inline void dump_wlancmds_function(u16 cmdcode, bool start)
{
	char *p;
	static u8 print_start[] = "==>";
	static u8 print_end[] = "<==";

	if (start)
		p = print_start;
	else
		p = print_end;

	switch (cmdcode) {
	case _JoinBss_CMD_:
		DBG_88E("_JoinBss_CMD_ %s join_cmd_hdl\n", p);
		break;
	case _DisConnect_CMD_:
		DBG_88E("_DisConnect_CMD_ %s disconnect_hdl\n", p);
		break;
	case _CreateBss_CMD_:
		DBG_88E("_CreateBss_CMD_ %s createbss_hdl\n", p);
		break;
	case _SetOpMode_CMD_:
		DBG_88E("_SetOpMode_CMD_ %s setopmode_hdl\n", p);
		break;
	case _SiteSurvey_CMD_:
		DBG_88E("_SiteSurvey_CMD_ %s sitesurvey_cmd_hdl\n", p);
		break;
	case _SetAuth_CMD_:
		DBG_88E("_SetAuth_CMD_ %s setauth_hdl\n", p);
		break;
	case _SetKey_CMD_:
		DBG_88E("_SetKey_CMD_ %s setkey_hdl\n", p);
		break;
	case _SetStaKey_CMD_:
		DBG_88E("_SetStaKey_CMD_ %s set_stakey_hdl\n", p);
		break;
	case _SetAssocSta_CMD_:
		DBG_88E("_SetAssocSta_CMD_ %s NULL\n", p);
		break;
	case _AddBAReq_CMD_:
		DBG_88E("_AddBAReq_CMD_ %s add_ba_hdl\n", p);
		break;
	case _SetChannel_CMD_:
		DBG_88E("_SetChannel_CMD_ %s set_ch_hdl\n", p);
		break;
	case _TX_Beacon_CMD_:
		DBG_88E("_TX_Beacon_CMD_ %s tx_beacon_hdl\n", p);
		break;
	case _Set_MLME_EVT_CMD_:
		DBG_88E("_Set_MLME_EVT_CMD_ %s mlme_evt_hdl\n", p);
		break;
	case _Set_Drv_Extra_CMD_:
		DBG_88E("_Set_Drv_Extra_CMD_ %s rtw_drvextra_cmd_hdl\n", p);
		break;
	case _SetChannelPlan_CMD_:
		DBG_88E("_SetChannelPlan_CMD_ %s set_chplan_hdl\n", p);
		break;
	default:
		break;
	}
}

static inline void rtw_dump_mac_address(const unsigned char *mac)
{
	u8	buf[20];

	if (!mac)
		return;

	memset(buf, 0, sizeof(buf));

	sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	DBG_88E("%s\n", buf);
}

#endif
