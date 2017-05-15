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
#ifndef __RECV_OSDEP_H_
#define __RECV_OSDEP_H_

#include <osdep_service.h>
#include <drv_types.h>


int _rtw_init_recv_priv(struct recv_priv *precvpriv, struct adapter *adapter);
void _rtw_free_recv_priv(struct recv_priv *precvpriv);


s32  rtw_recv_entry(struct recv_frame *precv_frame);
int rtw_recv_indicatepkt(struct adapter *adapter,
			 struct recv_frame *recv_frame);

void rtw_handle_tkip_mic_err(struct adapter *adapter, u8 bgroup);

int rtw_os_recvbuf_resource_alloc(struct adapter *adapt, struct recv_buf *buf);

void rtw_init_recv_timer(struct recv_reorder_ctrl *preorder_ctrl);

#endif /*  */
