/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_NT35502.h"

static struct msm_panel_info pinfo;
extern int charging_boot;
#if defined(CONFIG_VARIANT_SECOND_BOOTIMAGE)
extern char Sales_Code[3];
#endif

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSI Bit Clock at 366 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x03, 0x01, 0x01, 0x00},
	/* timing */
	{0xa9, 0x8a, 0x17, 0x00, 0x91, 0x93, 0x1a, 0x8c,
	0x11, 0x03, 0x04},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xbb, 0x02, 0x06, 0x00},
	/* pll control */
	{0x01, 0x69, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
};

static int mipi_video_nt35502_wvga_pt_init(void)
{
	int ret;

	if (msm_fb_detect_client("mipi_video_nt35502_wvga"))
		return 0;

	pinfo.xres = 480;
	pinfo.yres = 800;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_back_porch = 49;
	pinfo.lcdc.h_front_porch = 68;
	pinfo.lcdc.h_pulse_width = 17;
	pinfo.lcdc.v_back_porch = 9;
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 6;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	/* number of dot_clk cycles HSYNC active edge is
	delayed from VSYNC active edge */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.clk_rate = 366000000;
	pinfo.bl_max = 255; /*16; CHECK THIS!!!*/
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;

	pinfo.mipi.mode = DSI_VIDEO_MODE;
	/* send HSA and HE following VS/VE packet */
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hbp_power_stop = FALSE; /* LP-11 during the HBP period */
	pinfo.mipi.hsa_power_stop = FALSE; /* LP-11 during the HSA period */
	/* LP-11 or let Command Mode Engine send packets in
	HS or LP mode for the BLLP of the last line of a frame */
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	/* LP-11 or let Command Mode Engine send packets in
	HS or LP mode for packets sent during BLLP period */
	pinfo.mipi.bllp_power_stop = TRUE;

	pinfo.mipi.traffic_mode = DSI_BURST_MODE;
	pinfo.mipi.dst_format =  DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB; /* RGB */
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;

	pinfo.mipi.t_clk_post = 0x20;
	pinfo.mipi.t_clk_pre = 0x2f;

	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60; /* FIXME */

	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	/* //pinfo.mipi.dlane_swap = 0x01; */
	/* append EOT at the end of data burst */
	pinfo.mipi.tx_eot_append = 0x01;

	ret = mipi_nt35502_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_WVGA_PT);

	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

/* Temporary */
	if (charging_boot != 1) {
#if 0 /* //defined(CONFIG_VARIANT_SECOND_BOOTIMAGE) */
		if ((strncmp(Sales_Code, "ETR", 3) == 0)
			|| (strncmp(Sales_Code, "INS", 3) == 0)
			|| (strncmp(Sales_Code, "INU", 3) == 0)
			|| (strncmp(Sales_Code, "NPL", 3) == 0)
			|| (strncmp(Sales_Code, "SLK", 3) == 0)
			|| (strncmp(Sales_Code, "TML", 3) == 0)
			|| (strncmp(Sales_Code, "XME", 3) == 0)
			|| (strncmp(Sales_Code, "XSE", 3) == 0))
			load_565rle_image(INIT_IMAGE_VARIANT_FILE, 0);
		else
#endif
		load_565rle_image(INIT_IMAGE_FILE, 0);
	}

	return ret;
}

module_init(mipi_video_nt35502_wvga_pt_init);
