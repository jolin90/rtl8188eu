/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
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
#define _XMIT_OSDEP_C_

#include <osdep_service.h>
#include <drv_types.h>

#include <wifi.h>
#include <mlme_osdep.h>
#include <xmit_osdep.h>
#include <osdep_intf.h>

uint rtw_remainder_len(struct pkt_file *pfile)
{
	return pfile->buf_len - ((size_t)(pfile->cur_addr) -
	       (size_t)(pfile->buf_start));
}

void _rtw_open_pktfile(struct sk_buff *pktptr, struct pkt_file *pfile)
{

	pfile->pkt = pktptr;
	pfile->cur_addr = pktptr->data;
	pfile->buf_start = pktptr->data;
	pfile->pkt_len = pktptr->len;
	pfile->buf_len = pktptr->len;

	pfile->cur_buffer = pfile->buf_start;

}

uint _rtw_pktfile_read(struct pkt_file *pfile, u8 *rmem, uint rlen)
{
	uint	len = 0;


	len =  rtw_remainder_len(pfile);
	len = min(rlen, len);

	if (rmem)
		skb_copy_bits(pfile->pkt, pfile->buf_len-pfile->pkt_len, rmem, len);

	pfile->cur_addr += len;
	pfile->pkt_len -= len;


	return len;
}

int rtw_os_xmit_resource_alloc(struct adapter *adapter, struct xmit_buf *xmit_buf, u32 alloc_sz)
{
	int i;

	xmit_buf->pallocated_buf = kzalloc(alloc_sz, GFP_KERNEL);
	if (xmit_buf->pallocated_buf == NULL)
		return _FAIL;

	xmit_buf->pbuf = PTR_ALIGN(xmit_buf->pallocated_buf, XMITBUF_ALIGN_SZ);
	xmit_buf->dma_transfer_addr = 0;

	for (i = 0; i < 8; i++) {
		xmit_buf->pxmit_urb[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (xmit_buf->pxmit_urb[i] == NULL) {
			DBG_88E("xmit_buf->pxmit_urb[i]==NULL");
			return _FAIL;
		}
	}
	return _SUCCESS;
}

void rtw_os_xmit_resource_free(struct xmit_buf *xmit_buf)
{
	int i;

	for (i = 0; i < 8; i++)
		usb_free_urb(xmit_buf->pxmit_urb[i]);

	kfree(xmit_buf->pallocated_buf);
}

#define WMM_XMIT_THRESHOLD	(NR_XMITFRAME*2/5)

void rtw_os_pkt_complete(struct adapter *adapter, struct sk_buff *pkt)
{
	u16	queue;
	struct xmit_priv *xmit_priv = &adapter->xmitpriv;

	queue = skb_get_queue_mapping(pkt);
	if (adapter->registrypriv.wifi_spec) {
		if (__netif_subqueue_stopped(adapter->pnetdev, queue) &&
		    (xmit_priv->hwxmits[queue].accnt < WMM_XMIT_THRESHOLD))
			netif_wake_subqueue(adapter->pnetdev, queue);
	} else {
		if (__netif_subqueue_stopped(adapter->pnetdev, queue))
			netif_wake_subqueue(adapter->pnetdev, queue);
	}

	dev_kfree_skb_any(pkt);
}

void rtw_os_xmit_complete(struct adapter *adapter, struct xmit_frame *pxframe)
{
	if (pxframe->pkt)
		rtw_os_pkt_complete(adapter, pxframe->pkt);
	pxframe->pkt = NULL;
}

void rtw_os_xmit_schedule(struct adapter *adapter)
{
	struct xmit_priv *xmit_priv;

	if (!adapter)
		return;

	xmit_priv = &adapter->xmitpriv;

	spin_lock_bh(&xmit_priv->lock);

	if (rtw_txframes_pending(adapter))
		tasklet_hi_schedule(&xmit_priv->xmit_tasklet);

	spin_unlock_bh(&xmit_priv->lock);
}

static void rtw_check_xmit_resource(struct adapter *adapter, struct sk_buff *pkt)
{
	struct xmit_priv *xmit_priv = &adapter->xmitpriv;
	u16	queue;

	queue = skb_get_queue_mapping(pkt);
	if (adapter->registrypriv.wifi_spec) {
		/* No free space for Tx, tx_worker is too slow */
		if (xmit_priv->hwxmits[queue].accnt > WMM_XMIT_THRESHOLD)
			netif_stop_subqueue(adapter->pnetdev, queue);
	} else {
		if (xmit_priv->free_xmitframe_cnt <= 4) {
			if (!netif_tx_queue_stopped(netdev_get_tx_queue(adapter->pnetdev, queue)))
				netif_stop_subqueue(adapter->pnetdev, queue);
		}
	}
}

static int rtw_mlcst2unicst(struct adapter *adapter, struct sk_buff *skb)
{
	struct	sta_priv *pstapriv = &adapter->stapriv;
	struct xmit_priv *xmit_priv = &adapter->xmitpriv;
	struct list_head *phead, *plist;
	struct sk_buff *newskb;
	struct sta_info *psta = NULL;
	s32	res;

	spin_lock_bh(&pstapriv->asoc_list_lock);
	phead = &pstapriv->asoc_list;
	plist = phead->next;

	/* free sta asoc_queue */
	while (phead != plist) {
		psta = container_of(plist, struct sta_info, asoc_list);

		plist = plist->next;

		/* avoid   come from STA1 and send back STA1 */
		if (!memcmp(psta->hwaddr, &skb->data[6], 6))
			continue;

		newskb = skb_copy(skb, GFP_ATOMIC);

		if (newskb) {
			memcpy(newskb->data, psta->hwaddr, 6);
			res = rtw_xmit(adapter, &newskb);
			if (res < 0) {
				DBG_88E("%s()-%d: rtw_xmit() return error!\n", __func__, __LINE__);
				xmit_priv->tx_drop++;
				dev_kfree_skb_any(newskb);
			} else {
				xmit_priv->tx_pkts++;
			}
		} else {
			DBG_88E("%s-%d: skb_copy() failed!\n", __func__, __LINE__);
			xmit_priv->tx_drop++;

			spin_unlock_bh(&pstapriv->asoc_list_lock);
			return false;	/*  Caller shall tx this multicast frame via normal way. */
		}
	}

	spin_unlock_bh(&pstapriv->asoc_list_lock);
	dev_kfree_skb_any(skb);
	return true;
}


int rtw_xmit_entry(struct sk_buff *pkt, struct  net_device *pnetdev)
{
	struct adapter *adapter = (struct adapter *)rtw_netdev_priv(pnetdev);
	struct xmit_priv *xmit_priv = &adapter->xmitpriv;
	struct mlme_priv	*pmlmepriv = &adapter->mlmepriv;
	s32 res = 0;


	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("+xmit_enry\n"));

	if (rtw_if_up(adapter) == false) {
		RT_TRACE(_module_xmit_osdep_c_, _drv_err_, ("rtw_xmit_entry: rtw_if_up fail\n"));
		goto drop_packet;
	}

	rtw_check_xmit_resource(adapter, pkt);

	if (!rtw_mc2u_disable && check_fwstate(pmlmepriv, WIFI_AP_STATE) &&
	    (IP_MCAST_MAC(pkt->data) || ICMPV6_MCAST_MAC(pkt->data)) &&
	    (adapter->registrypriv.wifi_spec == 0)) {
		if (xmit_priv->free_xmitframe_cnt > (NR_XMITFRAME/4)) {
			res = rtw_mlcst2unicst(adapter, pkt);
			if (res)
				goto exit;
		}
	}

	res = rtw_xmit(adapter, &pkt);
	if (res < 0)
		goto drop_packet;

	xmit_priv->tx_pkts++;
	RT_TRACE(_module_xmit_osdep_c_, _drv_info_, ("rtw_xmit_entry: tx_pkts=%d\n", (u32)xmit_priv->tx_pkts));
	goto exit;

drop_packet:
	xmit_priv->tx_drop++;
	dev_kfree_skb_any(pkt);
	RT_TRACE(_module_xmit_osdep_c_, _drv_notice_, ("rtw_xmit_entry: drop, tx_drop=%d\n", (u32)xmit_priv->tx_drop));

exit:


	return 0;
}
