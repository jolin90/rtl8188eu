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

#include <osdep_service.h>
#include <drv_types.h>
#include <rtl8188e_hal.h>
#include <rtl8188e_led.h>
#include <usb_ops_linux.h>

/*  LED object. */

/*  LED_819xUsb routines. */
/*	Description: */
/*		Turn on LED according to LedPin specified. */
void SwLedOn(struct adapter *adapter, struct LED_871x *pLed)
{
	u8	LedCfg;

	if (adapter->bSurpriseRemoved || adapter->bDriverStopped)
		return;
	LedCfg = usb_read8(adapter, REG_LEDCFG2);
	usb_write8(adapter, REG_LEDCFG2, (LedCfg&0xf0) | BIT(5) | BIT(6)); /*  SW control led0 on. */
	pLed->bLedOn = true;
}

/*	Description: */
/*		Turn off LED according to LedPin specified. */
void SwLedOff(struct adapter *adapter, struct LED_871x *pLed)
{
	u8	LedCfg;

	if (adapter->bSurpriseRemoved || adapter->bDriverStopped)
		goto exit;

	LedCfg = usb_read8(adapter, REG_LEDCFG2);/* 0x4E */

	/*  Open-drain arrangement for controlling the LED) */
	LedCfg &= 0x90; /*  Set to software control. */
	usb_write8(adapter, REG_LEDCFG2, (LedCfg | BIT(3)));
	LedCfg = usb_read8(adapter, REG_MAC_PINMUX_CFG);
	LedCfg &= 0xFE;
	usb_write8(adapter, REG_MAC_PINMUX_CFG, LedCfg);
exit:
	pLed->bLedOn = false;
}

/*  Interface to manipulate LED objects. */
/*  Default LED behavior. */

/*	Description: */
/*		Initialize all LED_871x objects. */
void rtw_hal_sw_led_init(struct adapter *adapter)
{
	struct led_priv *pledpriv = &(adapter->ledpriv);

	InitLed871x(adapter, &(pledpriv->SwLed0));
}

/*	Description: */
/*		DeInitialize all LED_819xUsb objects. */
void rtw_hal_sw_led_deinit(struct adapter *adapter)
{
	struct led_priv	*ledpriv = &(adapter->ledpriv);

	DeInitLed871x(&(ledpriv->SwLed0));
}
