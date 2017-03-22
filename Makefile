8188eu-y :=				\
		core/rtw_ap.o		\
		core/rtw_cmd.o		\
		core/rtw_debug.o	\
		core/rtw_efuse.o	\
		core/rtw_ieee80211.o	\
		core/rtw_ioctl_set.o	\
		core/rtw_iol.o		\
		core/rtw_led.o		\
		core/rtw_mlme.o		\
		core/rtw_mlme_ext.o	\
		core/rtw_pwrctrl.o	\
		core/rtw_recv.o		\
		core/rtw_rf.o		\
		core/rtw_security.o	\
		core/rtw_sreset.o	\
		core/rtw_sta_mgt.o	\
		core/rtw_wlan_util.o	\
		core/rtw_xmit.o		\
		hal/fw.o	\
		hal/mac_cfg.o \
		hal/bb_cfg.o \
		hal/rf_cfg.o \
		hal/pwrseqcmd.o \
		hal/pwrseq.o \
		hal/Hal8188ERateAdaptive.o\
		hal/hal_intf.o		\
		hal/hal_com.o		\
		hal/odm.o		\
		hal/odm_HWConfig.o	\
		hal/odm_RTL8188E.o	\
		hal/rtl8188e_cmd.o	\
		hal/rtl8188e_dm.o	\
		hal/rtl8188e_hal_init.o	\
		hal/phy.o \
		hal/rf.o \
		hal/rtl8188e_rxdesc.o	\
		hal/rtl8188e_xmit.o	\
		hal/rtl8188eu_led.o	\
		hal/rtl8188eu_recv.o	\
		hal/rtl8188eu_xmit.o	\
		hal/usb_halinit.o	\
		os_dep/ioctl_linux.o	\
		os_dep/mlme_linux.o	\
		os_dep/mon.o		\
		os_dep/os_intfs.o	\
		os_dep/osdep_service.o	\
		os_dep/recv_linux.o	\
		os_dep/rtw_android.o	\
		os_dep/usb_intf.o	\
		os_dep/usb_ops_linux.o	\
		os_dep/xmit_linux.o

CONFIG_WLAN_CFG80211 = n

ifeq ($(CONFIG_WLAN_CFG80211), y)
r8188eu-y += os_dep/wlan_cfg80211.o
ccflags-y += -DWLAN_CFG80211
endif

export CONFIG_R8188EU = m
obj-$(CONFIG_R8188EU)	:= 8188eu.o

ccflags-y += -I$(srctree)/$(src)/include
ccflags-y += -D__CHECK_ENDIAN__ -I$(src)/include

#for debug
#ccflags-y += -DOS_DEP_IOCTL_LINUX

KSRC ?= "/lib/modules/$(shell uname -r)/build"
#CROSS_COMPILE := /opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-
#KSRC ?= /home/jolin/github/linux-3.0.86/

modules:
	$(MAKE) -C $(KSRC)  M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KSRC)  M=$(CURDIR) clean
