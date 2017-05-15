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

#define pr_fmt(fmt) "R8188EU: " fmt

#include <rtw_sreset.h>
#include <usb_ops_linux.h>

#include "jolin_debug.h"

void rtw_hal_sreset_init(struct adapter *adapter)
{
	struct sreset_priv *psrtpriv = &adapter->HalData->srestpriv;

	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
}

u8 sreset_get_wifi_status(struct adapter *adapter)
{
	struct sreset_priv *psrtpriv = &adapter->HalData->srestpriv;

	u8 status = WIFI_STATUS_SUCCESS;
	u32 val32 = 0;

	val32 = usb_read32(adapter, REG_TXDMA_STATUS);
	if (val32 == 0xeaeaeaea) {
		psrtpriv->Wifi_Error_Status = WIFI_IF_NOT_EXIST;
	} else if (val32 != 0) {
		DBG_88E("txdmastatu(%x)\n", val32);
		psrtpriv->Wifi_Error_Status = WIFI_MAC_TXDMA_ERROR;
	}

	if (WIFI_STATUS_SUCCESS != psrtpriv->Wifi_Error_Status) {
		DBG_88E("==>%s error_status(0x%x)\n", __func__, psrtpriv->Wifi_Error_Status);
		status = psrtpriv->Wifi_Error_Status & (~(USB_READ_PORT_FAIL|USB_WRITE_PORT_FAIL));
	}
	DBG_88E("==> %s wifi_status(0x%x)\n", __func__, status);

	/* status restore */
	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;

	return status;
}

void sreset_set_wifi_error_status(struct adapter *adapter, u32 status)
{
	adapter->HalData->srestpriv.Wifi_Error_Status = status;
}
