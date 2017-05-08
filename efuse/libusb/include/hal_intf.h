#ifndef __HAL_INTF_H__
#define __HAL_INTF_H__
#include "types.h"

void hal_read_chip_version(struct usb_adapter *adapter);
void hal_read_chip_info(struct usb_adapter *adapter);
#endif
