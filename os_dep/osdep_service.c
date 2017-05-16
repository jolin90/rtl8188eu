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
#define _OSDEP_SERVICE_C_

#include <osdep_service.h>
#include <osdep_intf.h>
#include <drv_types.h>
#include <recv_osdep.h>
#include <linux/vmalloc.h>
#include <rtw_ioctl_set.h>

u8 *_rtw_malloc(u32 sz)
{
	return kmalloc(sz, in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
}

void *rtw_malloc2d(int h, int w, int size)
{
	int j;
	void **a = kzalloc(h * sizeof(void *) + h * w * size, GFP_KERNEL);

	if (!a)
		goto out;

	for (j = 0; j < h; j++)
		a[j] = ((char *)(a + h)) + j * w * size;
out:
	return a;
}

void _rtw_init_queue(struct __queue *pqueue)
{
	INIT_LIST_HEAD(&pqueue->queue);
	spin_lock_init(&pqueue->lock);
}

u64 rtw_modular64(u64 x, u64 y)
{
	return do_div(x, y);
}

void rtw_buf_free(u8 **buf, u32 *buf_len)
{
	*buf_len = 0;
	kfree(*buf);
	*buf = NULL;
}

void rtw_buf_update(u8 **buf, u32 *buf_len, u8 *src, u32 src_len)
{
	u32 dup_len = 0;
	u8 *ori = NULL;
	u8 *dup = NULL;

	if (!buf || !buf_len)
		return;

	if (!src || !src_len)
		goto keep_ori;

	/* duplicate src */
	dup = rtw_malloc(src_len);
	if (dup) {
		dup_len = src_len;
		memcpy(dup, src, dup_len);
	}

keep_ori:
	ori = *buf;

	/* replace buf with dup */
	*buf_len = 0;
	*buf = dup;
	*buf_len = dup_len;

	/* free ori */
	kfree(ori);
}
