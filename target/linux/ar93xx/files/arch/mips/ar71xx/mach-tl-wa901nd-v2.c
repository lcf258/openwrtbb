/*
 *  TP-LINK TL-WA901ND v2 board support
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2010 Pieter Hollants <pieter@hollants.com>
 *  Copyright (C) 2011 Jonathan Bennett <jbscience87@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-ar9xxx-wmac.h"

#define TL_WA901ND_V2_GPIO_LED_QSS		4
#define TL_WA901ND_V2_GPIO_LED_SYSTEM		2
#define TL_WA901ND_V2_GPIO_LED_WLAN		9


#define TL_WA901ND_V2_GPIO_BTN_RESET		3
#define TL_WA901ND_V2_GPIO_BTN_QSS		7

#define TL_WA901ND_V2_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WA901ND_V2_KEYS_DEBOUNCE_INTERVAL	3 
#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tl_wa901nd_v2_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.offset		= 0x020000,
		.size		= 0x140000,
	}, {
		.name		= "rootfs",
		.offset		= 0x160000,
		.size		= 0x290000,
	}, {
		.name		= "art",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x020000,
		.size		= 0x3d0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data tl_wa901nd_v2_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= tl_wa901nd_v2_partitions,
	.nr_parts	= ARRAY_SIZE(tl_wa901nd_v2_partitions),
#endif
};

static struct gpio_led tl_wa901nd_v2_leds_gpio[] __initdata = {
	{
		.name		= "tl-wa901nd-v2:green:system",
		.gpio		= TL_WA901ND_V2_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "tl-wa901nd-v2:green:qss",
		.gpio		= TL_WA901ND_V2_GPIO_LED_QSS,
	}, {
		.name		= "tl-wa901nd-v2:green:wlan",
		.gpio		= TL_WA901ND_V2_GPIO_LED_WLAN,
		.active_low	= 1,
	}
};

static struct gpio_button tl_wa901nd_v2_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold = TL_WA901ND_V2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WA901ND_V2_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "qss",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold = TL_WA901ND_V2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WA901ND_V2_GPIO_BTN_QSS,
		.active_low	= 1,
	}
};

static void __init tl_wa901nd_v2_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *eeprom  = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = 0x00001000;
	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.reset_bit = RESET_MODULE_GE0_MAC |
				     RESET_MODULE_GE0_PHY;
	ar71xx_add_device_eth(0);

	ar71xx_add_device_m25p80(&tl_wa901nd_v2_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_wa901nd_v2_leds_gpio),
					tl_wa901nd_v2_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, TL_WA901ND_V2_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(tl_wa901nd_v2_gpio_keys),
					 tl_wa901nd_v2_gpio_keys);

        ar9xxx_add_device_wmac(eeprom, mac);
}

MIPS_MACHINE(AR71XX_MACH_TL_WA901ND_V2, "TL-WA901ND-v2",
	     "TP-LINK TL-WA901ND v2", tl_wa901nd_v2_setup);
