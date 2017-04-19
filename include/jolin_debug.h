#ifndef _JOLIN_DEBUG_H_
#define _JOLIN_DEBUG_H_

#ifndef _IEEE80211_C
#ifndef _RTW_WLAN_UTIL_C_
#ifndef _RTW_RECV_C_

#undef DBG_88E
#undef RT_TRACE

#define DBG_88E(fmt, args...)                \
	do{                                      \
		pr_info("%06d - %s : "fmt,           \
				__LINE__, __func__, ##args); \
	} while (0)

#define RT_TRACE(_comp, _level, fmt)         \
	do {                                     \
		pr_info("%06d - %s : ",              \
				__LINE__, __func__);         \
		printk fmt;                          \
	} while (0)

#endif
#endif
#endif

static inline void dump_wlancmds_function(u16 cmdcode)
{
	switch (cmdcode) {
	case _JoinBss_CMD_:
		DBG_88E("_JoinBss_CMD_ => join_cmd_hdl\n");
		break;
	case _DisConnect_CMD_:
		DBG_88E("_DisConnect_CMD_ => disconnect_hdl\n");
		break;
	case _CreateBss_CMD_:
		DBG_88E("_CreateBss_CMD_ => createbss_hdl\n");
		break;
	case _SetOpMode_CMD_:
		DBG_88E("_SetOpMode_CMD_ => setopmode_hdl\n");
		break;
	case _SiteSurvey_CMD_:
		DBG_88E("_SiteSurvey_CMD_ => sitesurvey_cmd_hdl\n");
		break;
	case _SetAuth_CMD_:
		DBG_88E("_SetAuth_CMD_ => setauth_hdl\n");
		break;
	case _SetKey_CMD_:
		DBG_88E("_SetKey_CMD_ => setkey_hdl\n");
		break;
	case _SetStaKey_CMD_:
		DBG_88E("_SetStaKey_CMD_ => set_stakey_hdl\n");
		break;
	case _SetAssocSta_CMD_:
		DBG_88E("_SetAssocSta_CMD_ => NULL\n");
		break;
	case _AddBAReq_CMD_:
		DBG_88E("_AddBAReq_CMD_ => add_ba_hdl\n");
		break;
	case _SetChannel_CMD_:
		DBG_88E("_SetChannel_CMD_ => set_ch_hdl\n");
		break;
	case _TX_Beacon_CMD_:
		DBG_88E("_TX_Beacon_CMD_ => tx_beacon_hdl\n");
		break;
	case _Set_MLME_EVT_CMD_:
		DBG_88E("_Set_MLME_EVT_CMD_ => mlme_evt_hdl\n");
		break;
	case _Set_Drv_Extra_CMD_:
		DBG_88E("_Set_Drv_Extra_CMD_ => rtw_drvextra_cmd_hdl\n");
		break;
	case _SetChannelPlan_CMD_:
		DBG_88E("_SetChannelPlan_CMD_ => set_chplan_hdl\n");
		break;
	default:
		break;
	}
}

static inline void rtw_dump_mac_address(const unsigned char *mac)
{
	unsigned char buf[20];

	if (!mac)
		return;

	memset(buf, 0, sizeof(buf));

	sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	DBG_88E("%s\n", buf);
}

#endif
