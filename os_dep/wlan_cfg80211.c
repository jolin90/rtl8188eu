#define pr_fmt(fmt) "R8188EU: " fmt

#include <net/cfg80211.h>
#include <proto/ethernet.h>
#include <proto/802.11.h>

#include "drv_types.h"
#include "wlan_cfg80211.h"

#define CHAN2G(_channel, _freq, _flags)		\
{											\
	.band			= IEEE80211_BAND_2GHZ,	\
	.center_freq		= (_freq),			\
	.hw_value		= (_channel),			\
	.flags			= (_flags),				\
	.max_antenna_gain	= 0,				\
	.max_power		= 30,					\
}

static struct ieee80211_channel __wl_2ghz_channels[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

#define RATE_TO_BASE100KBPS(rate)   (((rate) * 10) / 2)

#define RATETAB_ENT(_rateid, _flags)			\
{												\
	.bitrate	= RATE_TO_BASE100KBPS(_rateid),	\
	.hw_value	= (_rateid),					\
	.flags	  = (_flags),						\
}

static struct ieee80211_rate __wl_rates[] = {
	RATETAB_ENT(DOT11_RATE_1M, 0),
	RATETAB_ENT(DOT11_RATE_2M, IEEE80211_RATE_SHORT_PREAMBLE),
	RATETAB_ENT(DOT11_RATE_5M5, IEEE80211_RATE_SHORT_PREAMBLE),
	RATETAB_ENT(DOT11_RATE_11M, IEEE80211_RATE_SHORT_PREAMBLE),
	RATETAB_ENT(DOT11_RATE_6M, 0),
	RATETAB_ENT(DOT11_RATE_9M, 0),
	RATETAB_ENT(DOT11_RATE_12M, 0),
	RATETAB_ENT(DOT11_RATE_18M, 0),
	RATETAB_ENT(DOT11_RATE_24M, 0),
	RATETAB_ENT(DOT11_RATE_36M, 0),
	RATETAB_ENT(DOT11_RATE_48M, 0),
	RATETAB_ENT(DOT11_RATE_54M, 0)
};

#define wl_a_rates         (__wl_rates + 4)
#define wl_a_rates_size    8
#define wl_g_rates         (__wl_rates + 0)
#define wl_g_rates_size    12

static struct ieee80211_supported_band __wl_band_2ghz = {
	.band = IEEE80211_BAND_2GHZ,
	.channels = __wl_2ghz_channels,
	.n_channels = ARRAY_SIZE(__wl_2ghz_channels),
	.bitrates = wl_g_rates,
	.n_bitrates = wl_g_rates_size
};

static const struct ieee80211_txrx_stypes
wlan_cfg80211_default_mgmt_stypes[NUM_NL80211_IFTYPES] = {
	[NL80211_IFTYPE_ADHOC] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4)
	},
	[NL80211_IFTYPE_STATION] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_AP] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
			BIT(IEEE80211_STYPE_DISASSOC >> 4) |
			BIT(IEEE80211_STYPE_AUTH >> 4) |
			BIT(IEEE80211_STYPE_DEAUTH >> 4) |
			BIT(IEEE80211_STYPE_ACTION >> 4)
	},
	[NL80211_IFTYPE_AP_VLAN] = {
		/* copy AP */
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
			BIT(IEEE80211_STYPE_DISASSOC >> 4) |
			BIT(IEEE80211_STYPE_AUTH >> 4) |
			BIT(IEEE80211_STYPE_DEAUTH >> 4) |
			BIT(IEEE80211_STYPE_ACTION >> 4)
	},
	[NL80211_IFTYPE_P2P_CLIENT] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_P2P_GO] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
			BIT(IEEE80211_STYPE_DISASSOC >> 4) |
			BIT(IEEE80211_STYPE_AUTH >> 4) |
			BIT(IEEE80211_STYPE_DEAUTH >> 4) |
			BIT(IEEE80211_STYPE_ACTION >> 4)
	},
};

static const u32 wlan_cipher_suites[] = {
	WLAN_CIPHER_SUITE_WEP40,
	WLAN_CIPHER_SUITE_WEP104,
	WLAN_CIPHER_SUITE_TKIP,
	WLAN_CIPHER_SUITE_CCMP,
	WLAN_CIPHER_SUITE_AES_CMAC,
#ifdef WAPI_SUPPORT
	WLAN_CIPHER_SUITE_SMS4,
#endif
};

static int wlan_cfg80211_add_key(struct wiphy *wiphy, struct net_device *dev,
								 u8 key_index, bool pairwise,
								 const u8 * mac_addr, struct key_params *params)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_del_key(struct wiphy *wiphy, struct net_device *netdev,
								 u8 key_index, bool pairwise,
								 const u8 * mac_addr)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_set_default_key(struct wiphy *wiphy,
										 struct net_device *dev,
										 u8 key_index, bool unicast,
										 bool multicast)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_auth(struct wiphy *wiphy, struct net_device *dev,
							  struct cfg80211_auth_request *req)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_assoc(struct wiphy *wiphy, struct net_device *dev,
							   struct cfg80211_assoc_request *req)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_deauth
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 0, 86))
 (struct wiphy *wiphy, struct net_device *dev,
  struct cfg80211_deauth_request *req, void *cookie)
#else
 (struct wiphy *wiphy, struct net_device *dev,
  struct cfg80211_deauth_request *req)
#endif
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_disassoc
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 0, 86))
 (struct wiphy *wiphy, struct net_device *dev,
  struct cfg80211_disassoc_request *req, void *cookie)
#else
 (struct wiphy *wiphy, struct net_device *dev,
  struct cfg80211_disassoc_request *req)
#endif
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_connect(struct wiphy *wiphy, struct net_device *dev,
								 struct cfg80211_connect_params *sme)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_disconnect(struct wiphy *wiphy, struct net_device *dev,
									u16 reason_code)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_join_ibss(struct wiphy *wiphy, struct net_device *dev,
								   struct cfg80211_ibss_params *params)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_leave_ibss(struct wiphy *wiphy, struct net_device *dev)
{

	int ret = 0;
	return ret;

}

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 0, 86))
static struct net_device *wlan_cfg80211_add_virtual_intf(struct wiphy *wiphy,
														 char *name,
														 enum nl80211_iftype
														 type, u32 * flags,
														 struct vif_params
														 *params)
#else
static struct wireless_dev *wlan_cfg80211_add_virtual_intf(struct wiphy *wiphy,
														   const char *name,
														   enum nl80211_iftype
														   type, u32 * flags,
														   struct vif_params
														   *params)
#endif
{
	return NULL;
}

static int wlan_cfg80211_del_virtual_intf
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 0, 86))
 (struct wiphy *wiphy, struct net_device *dev)
#else
 (struct wiphy *wiphy, struct wireless_dev *wdev)
#endif
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_add_station(struct wiphy *wiphy,
									 struct net_device *dev, u8 * mac,
									 struct station_parameters *params)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_del_station(struct wiphy *wiphy,
									 struct net_device *dev, u8 * mac)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_add_mpath(struct wiphy *wiphy, struct net_device *dev,
								   u8 * dst, u8 * next_hop)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_del_mpath(struct wiphy *wiphy, struct net_device *dev,
								   u8 * dst)
{
	int ret = 0;
	return ret;

}

static int wlan_cfg80211_join_mesh(struct wiphy *wiphy, struct net_device *dev,
								   const struct mesh_config *conf,
								   const struct mesh_setup *setup)
{

	int ret = 0;
	return ret;
}

static int wlan_cfg80211_leave_mesh(struct wiphy *wiphy, struct net_device *dev)
{
	int ret = 0;
	return ret;
}

static int wlan_cfg80211_scan(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
							  struct net_device *dev,
#endif
							  struct cfg80211_scan_request *request)
{
	int ret = 0;
	return ret;
}

static struct cfg80211_ops wlan_cfg80211_ops = {
	.add_virtual_intf = wlan_cfg80211_add_virtual_intf,
	.del_virtual_intf = wlan_cfg80211_del_virtual_intf,
	/*.change_virtual_intf = wlan_cfg80211_change_virtual_intf, */
	.add_station = wlan_cfg80211_add_station,
	.del_station = wlan_cfg80211_del_station,
	/*.get_station = wlan_cfg80211_get_station, */
	.add_mpath = wlan_cfg80211_add_mpath,
	.del_mpath = wlan_cfg80211_del_mpath,
	.add_key = wlan_cfg80211_add_key,
	/*.get_key = wlan_cfg80211_get_key, */
	.del_key = wlan_cfg80211_del_key,
	.set_default_key = wlan_cfg80211_set_default_key,
	.auth = wlan_cfg80211_auth,
	.assoc = wlan_cfg80211_assoc,
	.deauth = wlan_cfg80211_deauth,
	.disassoc = wlan_cfg80211_disassoc,
	.scan = wlan_cfg80211_scan,
	/*.set_wiphy_params = wlan_cfg80211_set_wiphy_params, */
	.connect = wlan_cfg80211_connect,
	.disconnect = wlan_cfg80211_disconnect,
	.join_ibss = wlan_cfg80211_join_ibss,
	.leave_ibss = wlan_cfg80211_leave_ibss,
	/*.set_tx_power = wlan_cfg80211_set_tx_power, */
	/*.get_tx_power = wlan_cfg80211_get_tx_power, */
	/*.set_power_mgmt = wlan_cfg80211_set_power_mgmt, */
	/*.set_pmksa = wlan_cfg80211_set_pmksa, */
	/*.del_pmksa = wlan_cfg80211_del_pmksa, */
	/*.flush_pmksa = wlan_cfg80211_flush_pmksa, */
	.join_mesh = wlan_cfg80211_join_mesh,
	.leave_mesh = wlan_cfg80211_leave_mesh,
};

static int wlan_setup_wiphy(struct wireless_dev *wdev, struct device *dev)
{
	int err = 0;
	struct wiphy *wiphy;

	wiphy = wiphy_new(&wlan_cfg80211_ops, sizeof(struct adapter *));
	if (unlikely(!wiphy)) {
		pr_err("Couldn not allocate wiphy device\n");
		err = -ENOMEM;
		return err;
	}

	wiphy->max_scan_ie_len = WL_SCAN_IE_LEN_MAX;
	/* Report how many SSIDs Driver can support per Scan request */
	wiphy->max_scan_ssids = WL_SCAN_PARAMS_SSID_MAX;
	wiphy->max_num_pmkids = WL_NUM_PMKIDS_MAX;
	wiphy->max_remain_on_channel_duration = 5000;

	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION)
		| BIT(NL80211_IFTYPE_ADHOC)
		| BIT(NL80211_IFTYPE_AP)
		| BIT(NL80211_IFTYPE_P2P_CLIENT)
		| BIT(NL80211_IFTYPE_P2P_GO);

	wiphy->mgmt_stypes = wlan_cfg80211_default_mgmt_stypes;
	wiphy->cipher_suites = wlan_cipher_suites;
	wiphy->n_cipher_suites = ARRAY_SIZE(wlan_cipher_suites);
	wiphy->bands[IEEE80211_BAND_2GHZ] = &__wl_band_2ghz;
	wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0))
	wiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL | WIPHY_FLAG_OFFCHAN_TX;
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 2, 0))
	wiphy->flags |= WIPHY_FLAG_SUPPORTS_TDLS;
#endif

	wiphy->flags &= ~WIPHY_FLAG_PS_ON_BY_DEFAULT;

	/* Now we can register wiphy with cfg80211 module */
	err = wiphy_register(wiphy);
	if (unlikely(err < 0)) {
		pr_err("Couldn not register wiphy device (%d)\n", err);
		wiphy_free(wiphy);
		err = -ENOMEM;
		return err;
	}

	set_wiphy_dev(wiphy, dev);
	wdev->wiphy = wiphy;
	return err;
}

int wlan_cfg80211_attach(struct adapter *padapter, struct device *dev)
{
	int err = 0;
	struct net_device *pnetdev = padapter->pnetdev;
	struct wireless_dev *wdev;
	struct wlan_wdev_priv *pwdev_priv;

	pr_info("%s %d\n", __func__, __LINE__);

	wdev = kzalloc(sizeof(*wdev), GFP_KERNEL);
	if (unlikely(!wdev)) {
		pr_err("Could not allocate wireless device\n");
		return -ENOMEM;
	}

	pr_info("%s %d\n", __func__, __LINE__);
	err = wlan_setup_wiphy(wdev, dev);
	if (unlikely(err)) {
		kfree(wdev);
		return -ENOMEM;
	}

	*((struct adapter **)wiphy_priv(wdev->wiphy)) = padapter;
	wdev->netdev = pnetdev;
	wdev->iftype = NL80211_IFTYPE_STATION;
	pnetdev->ieee80211_ptr = wdev;
	padapter->wdev = wdev;

	pwdev_priv = &padapter->wdev_priv;
	pwdev_priv->pwdev = wdev;
	pwdev_priv->padapter = padapter;
	pwdev_priv->scan_request = NULL;

	pr_info("%s %d\n", __func__, __LINE__);
	return err;
}

void wlan_cfg80211_detach(struct wireless_dev *wdev)
{
	pr_info("%s %d\n", __func__, __LINE__);

	if (wdev->wiphy)
		wiphy_free(wdev->wiphy);
	if (wdev)
		kfree(wdev);

	pr_info("%s %d\n", __func__, __LINE__);
}