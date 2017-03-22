#ifndef _802_11_H_
#define _802_11_H_

/* Rate Defines */
/* Valid rates for the Supported Rates and Extended Supported Rates IEs.
 * Encoding is the rate in 500kbps units, rouding up for fractional values.
 * 802.11-2012, section 6.5.5.2, DATA_RATE parameter enumerates all the values.
 * The rate values cover DSSS, HR/DSSS, ERP, and OFDM phy rates.
 * The defines below do not cover the rates specific to 10MHz, {3, 4.5, 27},
 * and 5MHz, {1.5, 2.25, 3, 4.5, 13.5}, which are not supported by Broadcom devices.
 */
#define DOT11_RATE_1M   2       /* 1  Mbps in 500kbps units */
#define DOT11_RATE_2M   4       /* 2  Mbps in 500kbps units */
#define DOT11_RATE_5M5  11      /* 5.5 Mbps in 500kbps units */
#define DOT11_RATE_11M  22      /* 11 Mbps in 500kbps units */
#define DOT11_RATE_6M   12      /* 6  Mbps in 500kbps units */
#define DOT11_RATE_9M   18      /* 9  Mbps in 500kbps units */
#define DOT11_RATE_12M  24      /* 12 Mbps in 500kbps units */
#define DOT11_RATE_18M  36      /* 18 Mbps in 500kbps units */
#define DOT11_RATE_24M  48      /* 24 Mbps in 500kbps units */
#define DOT11_RATE_36M  72      /* 36 Mbps in 500kbps units */
#define DOT11_RATE_48M  96      /* 48 Mbps in 500kbps units */
#define DOT11_RATE_54M  108     /* 54 Mbps in 500kbps units */
#define DOT11_RATE_MAX  108     /* highest rate (54 Mbps) in 500kbps units */

#endif
