#ifndef _CFG80211_H_
#define _CFG80211_H_

#include <drv_types.h>

/* wl_iscan_results status values */
#define WL_SCAN_RESULTS_SUCCESS 0
#define WL_SCAN_RESULTS_PARTIAL 1
#define WL_SCAN_RESULTS_PENDING 2
#define WL_SCAN_RESULTS_ABORTED 3
#define WL_SCAN_RESULTS_NO_MEM  4

#define MAXPMKID                4 //zhangjiulin

#define WL_SCAN_PARAMS_SSID_MAX 9 //zhangjiulin
#define GET_SSID                "SSID="
#define GET_CHANNEL             "CH="
#define GET_NPROBE              "NPROBE="
#define GET_ACTIVE_ASSOC_DWELL  "ACTIVE="
#define GET_PASSIVE_ASSOC_DWELL "PASSIVE="
#define GET_HOME_DWELL          "HOME="
#define GET_SCAN_TYPE           "TYPE="

#define WL_SCAN_RETRY_MAX       3
#define WL_NUM_PMKIDS_MAX       MAXPMKID
#define WL_SCAN_BUF_MAX         (1024 * 8)
#define WL_TLV_INFO_MAX         1500
#define WL_SCAN_IE_LEN_MAX      2304 //zhangjiulin
#define WL_BSS_INFO_MAX         2048
#define WL_ASSOC_INFO_MAX       512
#define WL_IOCTL_LEN_MAX        2048
#define WL_EXTRA_BUF_MAX        2048
#define WL_SCAN_ERSULTS_LAST    (WLAN_SCAN_RESULTS_NO_MEM+1)
#define WL_AP_MAX               256
#define WL_FILE_NAME_MAX        256
#define WL_DWELL_TIME           200
#define WL_MED_DWELL_TIME       400
#define WL_MIN_DWELL_TIME       100
#define WL_LONG_DWELL_TIME      1000
#define IFACE_MAX_CNT             2
#define WL_SCAN_CONNECT_DWELL_TIME_MS       200
#define WL_SCAN_JOIN_PROBE_INTERVAL_MS      20
#define WL_SCAN_JOIN_ACTIVE_DWELL_TIME_MS   320
#define WL_SCAN_JOIN_PASSIVE_DWELL_TIME_MS  400
#define WL_AF_TX_MAX_RETRY                  5

#define WL_AF_SEARCH_TIME_MAX           450
#define WL_AF_TX_EXTRA_TIME_MAX         200

#define WL_SCAN_TIMER_INTERVAL_MS       10000	/* Scan timeout */
#define WL_CHANNEL_SYNC_RETRY   5
#define WL_INVALID              -1

int wlan_cfg80211_attach(struct adapter *adapter, struct device *dev);
void wlan_cfg80211_detach(struct wireless_dev *wdev);
#endif
