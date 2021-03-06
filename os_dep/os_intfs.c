/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/
#define _OS_INTFS_C_

#define pr_fmt(fmt) "R8188EU: " fmt

#include <linux/version.h>
#include <osdep_service.h>
#include <osdep_intf.h>
#include <drv_types.h>
#include <xmit_osdep.h>
#include <recv_osdep.h>
#include <hal_intf.h>
#include <rtw_ioctl.h>
#include <rtl8188e_hal.h>

#include "jolin_debug.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek Wireless Lan Driver");
MODULE_AUTHOR("Realtek Semiconductor Corp.");
MODULE_VERSION(DRIVERVERSION);

#define RTW_NOTCH_FILTER 0 /* 0:Disable, 1:Enable, */

/* module param defaults */
/* Ndis802_11Infrastructure; infra, ad-hoc, auto */
static int rtw_channel = 1;/* ad-hoc support requirement */
static int rtw_wireless_mode = WIRELESS_11BG_24N;
static int rtw_vrtl_carrier_sense = AUTO_VCS;
static int rtw_vcs_type = RTS_CTS;/*  */
static int rtw_rts_thresh = 2347;/*  */
static int rtw_frag_thresh = 2346;/*  */
static int rtw_preamble = PREAMBLE_LONG;/* long, short, auto */
static int rtw_power_mgnt = 1;
static int rtw_ips_mode = IPS_NORMAL;

static int rtw_smart_ps = 2;

module_param(rtw_ips_mode, int, 0644);
MODULE_PARM_DESC(rtw_ips_mode, "The default IPS mode");

static int rtw_debug = 1;

static int rtw_software_encrypt;
static int rtw_software_decrypt;

static int rtw_acm_method;/*  0:By SW 1:By HW. */

static int rtw_wmm_enable = 1;/*  default is set to enable the wmm. */
static int rtw_uapsd_enable;

static int rtw_ht_enable = 1;
/* 0 :disable, bit(0): enable 2.4g, bit(1): enable 5g */
static int rtw_cbw40_enable = 3;
static int rtw_ampdu_enable = 1;/* for enable tx_ampdu */

/* 0: disable
 * bit(0):enable 2.4g
 * bit(1):enable 5g
 * default is set to enable 2.4GHZ for IOT issue with bufflao's AP at 5GHZ
 */
static int rtw_rx_stbc = 1;
static int rtw_ampdu_amsdu;/*  0: disabled, 1:enabled, 2:auto */

static int rtw_wifi_spec;
static int rtw_channel_plan = RT_CHANNEL_DOMAIN_MAX;

static int rtw_antdiv_cfg = 2; /*  0:OFF , 1:ON, 2:decide by Efuse config */

/* 0: decide by efuse
 * 1: for 88EE, 1Tx and 1RxCG are diversity (2 Ant with SPDT)
 * 2: for 88EE, 1Tx and 2Rx are diversity (2 Ant, Tx and RxCG are both on aux
 *    port, RxCS is on main port)
 * 3: for 88EE, 1Tx and 1RxCG are fixed (1Ant, Tx and RxCG are both on aux port)
 */
static int rtw_antdiv_type;

static int rtw_enusbss;/* 0:disable, 1:enable */

static int rtw_hwpdn_mode = 2;/* 0:disable, 1:enable, 2: by EFUSE config */

int rtw_mc2u_disable;

static int rtw_80211d;

static char *ifname = "wlan%d";
module_param(ifname, charp, 0644);
MODULE_PARM_DESC(ifname, "The default name to allocate for first interface");

static char *if2name = "wlan%d";
module_param(if2name, charp, 0644);
MODULE_PARM_DESC(if2name, "The default name to allocate for second interface");

/* temp mac address if users want to use instead of the mac address in Efuse */
char *rtw_initmac;

module_param(rtw_initmac, charp, 0644);
module_param(rtw_channel_plan, int, 0644);
module_param(rtw_channel, int, 0644);
module_param(rtw_wmm_enable, int, 0644);
module_param(rtw_vrtl_carrier_sense, int, 0644);
module_param(rtw_vcs_type, int, 0644);
module_param(rtw_ht_enable, int, 0644);
module_param(rtw_cbw40_enable, int, 0644);
module_param(rtw_ampdu_enable, int, 0644);
module_param(rtw_rx_stbc, int, 0644);
module_param(rtw_ampdu_amsdu, int, 0644);
module_param(rtw_power_mgnt, int, 0644);
module_param(rtw_smart_ps, int, 0644);
module_param(rtw_wifi_spec, int, 0644);
module_param(rtw_antdiv_cfg, int, 0644);
module_param(rtw_antdiv_type, int, 0644);
module_param(rtw_enusbss, int, 0644);
module_param(rtw_hwpdn_mode, int, 0644);

static uint rtw_max_roaming_times = 2;
module_param(rtw_max_roaming_times, uint, 0644);
MODULE_PARM_DESC(rtw_max_roaming_times, "The max roaming times to try");

static int rtw_fw_iol = 1;/*  0:Disable, 1:enable, 2:by usb speed */
module_param(rtw_fw_iol, int, 0644);
MODULE_PARM_DESC(rtw_fw_iol, "FW IOL");

module_param(rtw_mc2u_disable, int, 0644);

module_param(rtw_80211d, int, 0644);
MODULE_PARM_DESC(rtw_80211d, "Enable 802.11d mechanism");

static uint rtw_notch_filter = RTW_NOTCH_FILTER;
module_param(rtw_notch_filter, uint, 0644);
MODULE_PARM_DESC(rtw_notch_filter, "0:Disable, 1:Enable, 2:Enable only for P2P");
module_param_named(debug, rtw_debug, int, 0444);
MODULE_PARM_DESC(debug, "Set debug level (1-9) (default 1)");

static bool rtw_monitor_enable;
module_param_named(monitor_enable, rtw_monitor_enable, bool, 0444);
MODULE_PARM_DESC(monitor_enable, "Enable monitor inferface (default: false)");

static int netdev_close(struct net_device *net_device);

static void loadparam(struct adapter *adapter, struct net_device *net_device)
{
	struct registry_priv  *registry_par = &adapter->registrypriv;

	GlobalDebugLevel = rtw_debug;

	memcpy(registry_par->ssid.Ssid, "ANY", 3);
	registry_par->ssid.SsidLength = 3;

	registry_par->channel = (u8)rtw_channel;
	registry_par->wireless_mode = (u8)rtw_wireless_mode;
	registry_par->vrtl_carrier_sense = (u8)rtw_vrtl_carrier_sense;
	registry_par->vcs_type = (u8)rtw_vcs_type;
	registry_par->rts_thresh = (u16)rtw_rts_thresh;
	registry_par->frag_thresh = (u16)rtw_frag_thresh;
	registry_par->preamble = (u8)rtw_preamble;
	registry_par->smart_ps =  (u8)rtw_smart_ps;
	registry_par->power_mgnt = (u8)rtw_power_mgnt;
	registry_par->ips_mode = (u8)rtw_ips_mode;
	registry_par->mp_mode = 0;
	registry_par->software_encrypt = (u8)rtw_software_encrypt;
	registry_par->software_decrypt = (u8)rtw_software_decrypt;
	registry_par->acm_method = (u8)rtw_acm_method;

	 /* UAPSD */
	registry_par->wmm_enable = (u8)rtw_wmm_enable;
	registry_par->uapsd_enable = (u8)rtw_uapsd_enable;

	registry_par->ht_enable = (u8)rtw_ht_enable;
	registry_par->cbw40_enable = (u8)rtw_cbw40_enable;
	registry_par->ampdu_enable = (u8)rtw_ampdu_enable;
	registry_par->rx_stbc = (u8)rtw_rx_stbc;
	registry_par->ampdu_amsdu = (u8)rtw_ampdu_amsdu;
	registry_par->wifi_spec = (u8)rtw_wifi_spec;
	registry_par->channel_plan = (u8)rtw_channel_plan;
	registry_par->accept_addba_req = true;
	registry_par->antdiv_cfg = (u8)rtw_antdiv_cfg;
	registry_par->antdiv_type = (u8)rtw_antdiv_type;
	registry_par->hwpdn_mode = (u8)rtw_hwpdn_mode;

	registry_par->max_roaming_times = (u8)rtw_max_roaming_times;

	registry_par->fw_iol = rtw_fw_iol;

	registry_par->enable80211d = (u8)rtw_80211d;
	snprintf(registry_par->ifname, 16, "%s", ifname);
	snprintf(registry_par->if2name, 16, "%s", if2name);
	registry_par->notch_filter = (u8)rtw_notch_filter;
	registry_par->monitor_enable = rtw_monitor_enable;
}

static int rtw_net_set_mac_address(struct net_device *net_device, void *p)
{
	struct adapter *adapter = (struct adapter *)netdev_priv(net_device);
	struct sockaddr *addr = p;

	if (!adapter->bup)
		memcpy(adapter->eeprompriv.mac_addr, addr->sa_data, ETH_ALEN);

	return 0;
}

static struct net_device_stats *rtw_net_get_stats(struct net_device *net_device)
{
	struct adapter *adapter = (struct adapter *)netdev_priv(net_device);
	struct xmit_priv *xmit_priv = &(adapter->xmitpriv);
	struct recv_priv *precvpriv = &(adapter->recvpriv);

	adapter->stats.tx_packets = xmit_priv->tx_pkts;
	adapter->stats.rx_packets = precvpriv->rx_pkts;
	adapter->stats.tx_dropped = xmit_priv->tx_drop;
	adapter->stats.rx_dropped = precvpriv->rx_drop;
	adapter->stats.tx_bytes = xmit_priv->tx_bytes;
	adapter->stats.rx_bytes = precvpriv->rx_bytes;
	return &adapter->stats;
}

/*
 * AC to queue mapping
 *
 * AC_VO -> queue 0
 * AC_VI -> queue 1
 * AC_BE -> queue 2
 * AC_BK -> queue 3
 */
static const u16 rtw_1d_to_queue[8] = { 2, 3, 3, 2, 1, 1, 0, 0 };

/* Given a data frame determine the 802.1p/1d tag to use. */
static unsigned int rtw_classify8021d(struct sk_buff *skb)
{
	unsigned int dscp;

	/* skb->priority values from 256->263 are magic values to
	 * directly indicate a specific 802.1d priority.  This is used
	 * to allow 802.1d priority to be passed directly in from VLAN
	 * tags, etc.
	 */
	if (skb->priority >= 256 && skb->priority <= 263)
		return skb->priority - 256;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		dscp = ip_hdr(skb)->tos & 0xfc;
		break;
	default:
		return 0;
	}

	return dscp >> 5;
}

static u16 rtw_select_queue(struct net_device *dev, struct sk_buff *skb
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 0, 86))
		, void *accel_priv
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 13, 0))
		, select_queue_fallback_t fallback
#endif
		)
{
	struct adapter	*adapter = netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;

	skb->priority = rtw_classify8021d(skb);

	if (pmlmepriv->acm_mask != 0)
		skb->priority = qos_acm(pmlmepriv->acm_mask, skb->priority);

	return rtw_1d_to_queue[skb->priority];
}

u16 rtw_recv_select_queue(struct sk_buff *skb)
{
	struct iphdr *piphdr;
	unsigned int dscp;
	__be16	eth_type;
	u32 priority;
	u8 *pdata = skb->data;

	memcpy(&eth_type, pdata+(ETH_ALEN<<1), 2);

	switch (eth_type) {
	case htons(ETH_P_IP):
		piphdr = (struct iphdr *)(pdata+ETH_HLEN);
		dscp = piphdr->tos & 0xfc;
		priority = dscp >> 5;
		break;
	default:
		priority = 0;
	}

	return rtw_1d_to_queue[priority];
}

static const struct net_device_ops rtw_netdev_ops = {
	.ndo_open = netdev_open,
	.ndo_stop = netdev_close,
	.ndo_start_xmit = rtw_xmit_entry,
	.ndo_select_queue	= rtw_select_queue,
	.ndo_set_mac_address = rtw_net_set_mac_address,
	.ndo_get_stats = rtw_net_get_stats,
	.ndo_do_ioctl = rtw_ioctl,
};

int rtw_init_netdev_name(struct net_device *net_device, const char *ifname)
{
	DBG_88E("ifname;%s\n", ifname);

	if (dev_alloc_name(net_device, ifname) < 0)
		RT_TRACE(_module_os_intfs_c_, _drv_err_, ("dev_alloc_name, fail!\n"));

	netif_carrier_off(net_device);
	return 0;
}

static const struct device_type wlan_type = {
	.name = "wlan",
};

struct net_device *rtw_init_netdev(struct net_device *net_device)
{
	struct adapter *adapter = NULL;

	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("+init_net_dev\n"));

	net_device = alloc_etherdev_mq(sizeof(*adapter), 32);
	if (!net_device)
		return NULL;

	DBG_88E("net_device;%p\n", net_device);

	adapter = netdev_priv(net_device);
	adapter->bDriverStopped = true;
	mutex_init(&adapter->hw_init_mutex);
	adapter->net_device = net_device;

	DBG_88E("register rtw_netdev_ops to netdev_ops\n");
	net_device->dev.type = &wlan_type;
	net_device->netdev_ops = &rtw_netdev_ops;
	net_device->watchdog_timeo = HZ*3; /* 3 second timeout */

#ifdef CONFIG_WIRELESS_EXT
	net_device->wireless_handlers = (struct iw_handler_def *)&rtw_handlers_def;
#endif

	loadparam(adapter, net_device);

	return net_device;
}

static int rtw_start_drv_threads(struct adapter *adapter)
{
	int err = 0;

	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("+rtw_start_drv_threads\n"));

	adapter->cmdThread = kthread_run(rtw_cmd_thread, adapter,
					  "RTW_CMD_THREAD");
	if (IS_ERR(adapter->cmdThread))
		err = PTR_ERR(adapter->cmdThread);
	else
		/* wait for cmd_thread to run */
		wait_for_completion_interruptible(&adapter->cmdpriv.terminate_cmdthread_comp);

	return err;
}

void rtw_stop_drv_threads(struct adapter *adapter)
{
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("+rtw_stop_drv_threads\n"));

	/* Below is to terminate rtw_cmd_thread & event_thread... */
	complete(&adapter->cmdpriv.cmd_queue_comp);
	if (adapter->cmdThread)
		wait_for_completion_interruptible(&adapter->cmdpriv.terminate_cmdthread_comp);

}

static u8 rtw_init_default_value(struct adapter *adapter)
{
	struct registry_priv *pregistrypriv = &adapter->registrypriv;
	struct xmit_priv	*xmit_priv = &adapter->xmitpriv;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct security_priv *psecuritypriv = &adapter->securitypriv;

	/* xmit_priv */
	xmit_priv->vcs_setting = pregistrypriv->vrtl_carrier_sense;
	xmit_priv->vcs = pregistrypriv->vcs_type;
	xmit_priv->vcs_type = pregistrypriv->vcs_type;
	xmit_priv->frag_len = pregistrypriv->frag_thresh;

	/* mlme_priv */
	pmlmepriv->scan_interval = SCAN_INTERVAL;/*  30*2 sec = 60sec */
	pmlmepriv->scan_mode = SCAN_ACTIVE;

	/* ht_priv */
	pmlmepriv->htpriv.ampdu_enable = false;/* set to disabled */

	/* security_priv */
	psecuritypriv->binstallGrpkey = _FAIL;
	psecuritypriv->sw_encrypt = pregistrypriv->software_encrypt;
	psecuritypriv->sw_decrypt = pregistrypriv->software_decrypt;
	psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
	psecuritypriv->dot11PrivacyAlgrthm = _NO_PRIVACY_;
	psecuritypriv->dot11PrivacyKeyIndex = 0;
	psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;
	psecuritypriv->dot118021XGrpKeyid = 1;
	psecuritypriv->ndisauthtype = Ndis802_11AuthModeOpen;
	psecuritypriv->ndisencryptstatus = Ndis802_11WEPDisabled;

	/* registry_priv */
	rtw_init_registrypriv_dev_network(adapter);
	rtw_update_registrypriv_dev_network(adapter);

	/* hal_priv */
	rtw_hal_def_value_init(adapter);

	/* misc. */
	adapter->bReadPortCancel = false;
	adapter->bWritePortCancel = false;
	return _SUCCESS;
}

u8 rtw_reset_drv_sw(struct adapter *adapter)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct pwrctrl_priv *pwrctrlpriv = &adapter->pwrctrlpriv;

	/* hal_priv */
	rtw_hal_def_value_init(adapter);
	adapter->bReadPortCancel = false;
	adapter->bWritePortCancel = false;
	pmlmepriv->scan_interval = SCAN_INTERVAL;/*  30*2 sec = 60sec */

	adapter->xmitpriv.tx_pkts = 0;
	adapter->recvpriv.rx_pkts = 0;

	pmlmepriv->LinkDetectInfo.bBusyTraffic = false;

	_clr_fwstate_(pmlmepriv, _FW_UNDER_SURVEY | _FW_UNDER_LINKING);
	rtw_hal_sreset_init(adapter);
	pwrctrlpriv->pwr_state_check_cnts = 0;

	/* mlmeextpriv */
	DBG_88E("SCAN_DISABLE\n");
	adapter->mlmeextpriv.sitesurvey_res.state = SCAN_DISABLE;

	rtw_set_signal_stat_timer(&adapter->recvpriv);

	return _SUCCESS;
}

u8 rtw_init_drv_sw(struct adapter *adapter)
{
	u8	ret8 = _SUCCESS;


	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("+rtw_init_drv_sw\n"));

	if ((rtw_init_cmd_priv(&adapter->cmdpriv)) == _FAIL) {
		RT_TRACE(_module_os_intfs_c_, _drv_err_, ("\n Can't init cmd_priv\n"));
		ret8 = _FAIL;
		goto exit;
	}

	adapter->cmdpriv.adapter = adapter;

	if (rtw_init_mlme_priv(adapter) == _FAIL) {
		RT_TRACE(_module_os_intfs_c_, _drv_err_, ("\n Can't init mlme_priv\n"));
		ret8 = _FAIL;
		goto exit;
	}

	if (init_mlme_ext_priv(adapter) == _FAIL) {
		RT_TRACE(_module_os_intfs_c_, _drv_err_, ("\n Can't init mlme_ext_priv\n"));
		ret8 = _FAIL;
		goto exit;
	}

	if (_rtw_init_xmit_priv(&adapter->xmitpriv, adapter) == _FAIL) {
		DBG_88E("Can't _rtw_init_xmit_priv\n");
		ret8 = _FAIL;
		goto exit;
	}

	if (_rtw_init_recv_priv(&adapter->recvpriv, adapter) == _FAIL) {
		DBG_88E("Can't _rtw_init_recv_priv\n");
		ret8 = _FAIL;
		goto exit;
	}

	if (_rtw_init_sta_priv(&adapter->stapriv) == _FAIL) {
		DBG_88E("Can't _rtw_init_sta_priv\n");
		ret8 = _FAIL;
		goto exit;
	}

	adapter->stapriv.adapter = adapter;

	rtw_init_bcmc_stainfo(adapter);

	rtw_init_pwrctrl_priv(adapter);

	ret8 = rtw_init_default_value(adapter);

	rtw_hal_dm_init(adapter);
	rtw_hal_sw_led_init(adapter);

	rtw_hal_sreset_init(adapter);

exit:
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("-rtw_init_drv_sw\n"));


	return ret8;
}

void rtw_cancel_all_timer(struct adapter *adapter)
{
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("+rtw_cancel_all_timer\n"));

	del_timer_sync(&adapter->mlmepriv.assoc_timer);
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("rtw_cancel_all_timer:cancel association timer complete!\n"));

	del_timer_sync(&adapter->mlmepriv.scan_to_timer);
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("rtw_cancel_all_timer:cancel scan_to_timer!\n"));

	del_timer_sync(&adapter->mlmepriv.dynamic_chk_timer);
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("rtw_cancel_all_timer:cancel dynamic_chk_timer!\n"));

	/*  cancel sw led timer */
	rtw_hal_sw_led_deinit(adapter);
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("rtw_cancel_all_timer:cancel DeInitSwLeds!\n"));

	del_timer_sync(&adapter->pwrctrlpriv.pwr_state_check_timer);

	del_timer_sync(&adapter->recvpriv.signal_stat_timer);
}

u8 rtw_free_drv_sw(struct adapter *adapter)
{
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("==>rtw_free_drv_sw"));

	free_mlme_ext_priv(&adapter->mlmeextpriv);

	rtw_free_mlme_priv(&adapter->mlmepriv);
	_rtw_free_xmit_priv(&adapter->xmitpriv);

	/* will free bcmc_stainfo here */
	_rtw_free_sta_priv(&adapter->stapriv);

	_rtw_free_recv_priv(&adapter->recvpriv);

	rtw_hal_free_data(adapter);

	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("<== rtw_free_drv_sw\n"));

	mutex_destroy(&adapter->hw_init_mutex);

	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("-rtw_free_drv_sw\n"));

	return _SUCCESS;
}

static int _netdev_open(struct net_device *net_device)
{
	uint status;
	int err;
	struct adapter *adapter = (struct adapter *)netdev_priv(net_device);
	struct pwrctrl_priv *pwrctrlpriv = &adapter->pwrctrlpriv;

	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("+88eu_drv - dev_open\n"));
	DBG_88E("+88eu_drv - drv_open, bup =%d\n", adapter->bup);

	if (pwrctrlpriv->ps_flag) {
		adapter->net_closed = false;
		goto netdev_open_normal_process;
	}

	if (!adapter->bup) {
		adapter->bDriverStopped = false;
		adapter->bSurpriseRemoved = false;

		status = rtw_hal_init(adapter);
		if (status == _FAIL) {
			RT_TRACE(_module_os_intfs_c_, _drv_err_, ("rtl88eu_hal_init(): Can't init h/w!\n"));
			goto netdev_open_error;
		}

		DBG_88E("MAC Address = %pM\n", net_device->dev_addr);

		err = rtw_start_drv_threads(adapter);
		if (err) {
			DBG_88E("Initialize driver software resource Failed!\n");
			goto netdev_open_error;
		}

		if (init_hw_mlme_ext(adapter) == _FAIL) {
			DBG_88E("can't init mlme_ext_priv\n");
			goto netdev_open_error;
		}
		rtw_hal_inirp_init(adapter);

		LedControl8188eu(adapter, LED_CTL_NO_LINK);

		adapter->bup = true;
	}
	adapter->net_closed = false;

	mod_timer(&adapter->mlmepriv.dynamic_chk_timer,
		  jiffies + msecs_to_jiffies(2000));

	adapter->pwrctrlpriv.bips_processing = false;
	rtw_set_pwr_state_check_timer(&adapter->pwrctrlpriv);

	if (!rtw_netif_queue_stopped(net_device))
		netif_tx_start_all_queues(net_device);
	else
		netif_tx_wake_all_queues(net_device);

netdev_open_normal_process:
	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("-88eu_drv - dev_open\n"));
	DBG_88E("-88eu_drv - drv_open, bup =%d\n", adapter->bup);
	return 0;

netdev_open_error:
	adapter->bup = false;
	netif_carrier_off(net_device);
	netif_tx_stop_all_queues(net_device);
	RT_TRACE(_module_os_intfs_c_, _drv_err_, ("-88eu_drv - dev_open, fail!\n"));
	DBG_88E("-88eu_drv - drv_open fail, bup =%d\n", adapter->bup);
	return -1;
}

int netdev_open(struct net_device *net_device)
{
	int ret;
	struct adapter *adapter = (struct adapter *)netdev_priv(net_device);

	if (mutex_lock_interruptible(&adapter->hw_init_mutex))
		return -ERESTARTSYS;
	ret = _netdev_open(net_device);
	mutex_unlock(&adapter->hw_init_mutex);
	return ret;
}

int  ips_netdrv_open(struct adapter *adapter)
{
	int status = _SUCCESS;

	adapter->net_closed = false;
	DBG_88E("===> %s.........\n", __func__);

	adapter->bDriverStopped = false;
	adapter->bSurpriseRemoved = false;

	status = rtw_hal_init(adapter);
	if (status == _FAIL) {
		RT_TRACE(_module_os_intfs_c_, _drv_err_, (" Can't init h/w!\n"));
		goto netdev_open_error;
	}

	rtw_hal_inirp_init(adapter);

	rtw_set_pwr_state_check_timer(&adapter->pwrctrlpriv);
	mod_timer(&adapter->mlmepriv.dynamic_chk_timer,
		  jiffies + msecs_to_jiffies(5000));

	 return _SUCCESS;

netdev_open_error:
	DBG_88E("-drv_open failure, bup =%d\n", adapter->bup);

	return _FAIL;
}


int rtw_ips_pwr_up(struct adapter *adapter)
{
	int result;
	unsigned long start_time = jiffies;

	DBG_88E("===>  ..............\n");
	rtw_reset_drv_sw(adapter);

	result = ips_netdrv_open(adapter);

	LedControl8188eu(adapter, LED_CTL_NO_LINK);

	DBG_88E("<===  .............. in %dms\n",
		jiffies_to_msecs(jiffies - start_time));
	return result;
}

void rtw_ips_pwr_down(struct adapter *adapter)
{
	unsigned long start_time = jiffies;

	DBG_88E("===> rtw_ips_pwr_down...................\n");

	adapter->net_closed = true;

	LedControl8188eu(adapter, LED_CTL_POWER_OFF);

	rtw_ips_dev_unload(adapter);
	DBG_88E("<=== rtw_ips_pwr_down..................... in %dms\n",
		jiffies_to_msecs(jiffies - start_time));
}

void rtw_ips_dev_unload(struct adapter *adapter)
{
	DBG_88E("====> %s...\n", __func__);

	rtw_hal_set_hwreg(adapter, HW_VAR_FIFO_CLEARN_UP, NULL);

	usb_intf_stop(adapter);

	/* s5. */
	if (!adapter->bSurpriseRemoved)
		rtw_hal_deinit(adapter);
}

static int netdev_close(struct net_device *net_device)
{
	struct adapter *adapter = (struct adapter *)netdev_priv(net_device);

	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("+88eu_drv - drv_close\n"));

	if (adapter->pwrctrlpriv.bInternalAutoSuspend) {
		if (adapter->pwrctrlpriv.rf_pwrstate == rf_off)
			adapter->pwrctrlpriv.ps_flag = true;
	}
	adapter->net_closed = true;

	if (adapter->pwrctrlpriv.rf_pwrstate == rf_on) {
		DBG_88E("(2)88eu_drv - drv_close, bup =%d, hw_init_completed =%d\n",
			adapter->bup, adapter->hw_init_completed);

		/* s1. */
		if (net_device) {
			if (!rtw_netif_queue_stopped(net_device))
				netif_tx_stop_all_queues(net_device);
		}

		/* s2. */
		LeaveAllPowerSaveMode(adapter);
		rtw_disassoc_cmd(adapter, 500, false);
		/* s2-2.  indicate disconnect to os */
		rtw_indicate_disconnect(adapter);
		/* s2-3. */
		rtw_free_assoc_resources(adapter);
		/* s2-4. */
		rtw_free_network_queue(adapter, true);
		/*  Close LED */
		LedControl8188eu(adapter, LED_CTL_POWER_OFF);
	}

	RT_TRACE(_module_os_intfs_c_, _drv_info_, ("-88eu_drv - drv_close\n"));
	DBG_88E("-88eu_drv - drv_close, bup =%d\n", adapter->bup);
	return 0;
}
