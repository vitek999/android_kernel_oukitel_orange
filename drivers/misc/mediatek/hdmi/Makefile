
ifeq ($(CONFIG_CUSTOM_KERNEL_HDMI), "Sii8348")
obj-y	+= Sii8348/
endif

ifeq ($(CONFIG_CUSTOM_KERNEL_HDMI), "Sii8338")
obj-y	+= Sii8338/
endif

ifeq ($(CONFIG_CUSTOM_KERNEL_HDMI), "sil9024")
obj-y	+= sil9024/
endif

ifeq ($(CONFIG_CUSTOM_KERNEL_HDMI), "MT8193")
obj-y	+= mt8193/
endif

ifeq ($(CONFIG_MTK_INTERNAL_HDMI_SUPPORT), y)
obj-y  += internal_hdmi/$(subst ",,$(CONFIG_MTK_PLATFORM))/
endif
