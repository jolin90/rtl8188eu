#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>

#include "types.h"
#include "hal_intf.h"

int main(int argc, char *argv[])
{
	int r = 1;

	struct usb_adapter *adapter;
	struct libusb_device_handle *devh;

	adapter = malloc(sizeof(struct usb_adapter));
	if (!adapter) {
		perror("malloc error\n");
		exit(1);
	}

	r = libusb_init(NULL);
	if (r < 0) {
		perror("failed to initialise libusb\n");
		exit(1);
	}

	libusb_set_debug(NULL, 0);

	devh = libusb_open_device_with_vid_pid(NULL, 0x0bda, 0x8179);
	if (!devh) {
		perror("Could not find/open device\n");
		goto out;
	}
	adapter->devh = devh;

	r = libusb_claim_interface(devh, 0);
	if (r < 0) {
		perror("usb_claim_interface error\n");
		goto out;
	}
	printf("claimed interface zero\n");

	hal_read_chip_version(adapter);
	hal_read_chip_info(adapter);

out:
	libusb_close(devh);
	libusb_exit(NULL);
	free(adapter);

	return r;
}
