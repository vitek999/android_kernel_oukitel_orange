#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

#ifdef BUILD_LK
#include <stdio.h>
#include <string.h>
#else
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#define _LCM_DEBUG_

#include <cust_gpio_usage.h>

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             							0XFEFF
#define REGFLAG_END_OF_TABLE      							0xFFFF   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(gpio_num,val)    						(lcm_util.set_gpio_out((gpio_num),(val)))


#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))



//#define _SYNA_INFO_
//#define _SYNA_DEBUG_
//#define _LCM_DEBUG_
//#define _LCM_INFO_
/*
#ifdef _LCM_DEBUG_
#define lcm_debug(fmt, args...) printk(fmt, ##args)
#else
#define lcm_debug(fmt, args...) do { } while (0)
#endif

#ifdef _LCM_INFO_
#define lcm_info(fmt, args...) printk(fmt, ##args)
#else
#define lcm_info(fmt, args...) do { } while (0)
#endif
#define lcm_err(fmt, args...) printk(fmt, ##args) */

#ifdef _LCM_DEBUG_
  #ifdef BUILD_LK
  #define LCM_PRINT printf
  #else
  #define LCM_PRINT printk
  #endif
#else
	#define LCM_PRINT
#endif

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    
       

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
	
	{0xB9,3,{0xFF,0x83,0x94}},
	
	{0xBA,6,{0x63,0x03,0x68,0x6B,0xB2,0xC0}},
	
	
	
	{0xB1,10,{0x50,0x12,0x72,0x09,0x30,0x11,0x71,0x31,0x4D,0x2F}},
	
	{0xB2,6,{0x00,0x80,0x64,0x10,0x0D,0x2F}},
	
	{0xB4,21,{0x03,0x79,0x03,0x51,0x03,0x51,0x01,0x05,0x7E,0x35,0x00,0x3F,0x03,0x79,0x03,0x51,0x03,0x51,0x05,0x01,0x7E}},
	
	{0xB6,2,{0xE0,0xE0}},
	
	{0xD3,33,{0x00,0x00,0x0F,0x0F,0x01,0x2F,0x08,0x00,0x32,0x10,0x0B,0x00,0x0B,0x32,0x15,0x07,0x05,0x07,0x32,0x10,0x00,0x00,0x00,0x37,0x33
	,0x0B,0x0B,0x37,0x10,0x07,0x07,0x10,0x40}},
	
	{0xD5,44,{0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x02,0x03,0x18,0x18
	,0x00,0x01,0x06,0x07,0x04,0x05,0x20,0x21,0x22,0x23,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},
	
	{0xCC,1,{0x0B}},
	
	{0xE0,58,{0x4F,0x50,0x52,0x55,0x54,0x56,0x57,0x53,0xA3,0xAC,0xB6,0xB0,0xAF,0xBA,0xBA,0xB8,0xC1,0xBF,0xB9,0xC4,0xD1,0x66,0x64,0x68
	,0x6B,0x6E,0x7C,0x7F,0x7F,0x4F,0x50,0x52,0x55,0x54,0x56,0x57,0x53,0xA3,0xAC,0xB6,0xB0,0xAF,0xBA,0xBA,0xB8,0xC1,0xBF,0xB9,0xC4,0xD1,0x66,0x64
	,0x68,0x6B,0x6E,0x7C,0x7F,0x7F}},
	
	{0xC0,2,{0x1F,0x73}},
	
	{0xD4,1,{0x02}},
	
	{0xBD,1,{0x01}},
	
	{0xB1,1,{0x60}},
	
	{0xBD,1,{0x00}},
	
	{0xBF,7,{0x40,0x81,0x50,0x00,0x1A,0xFC,0x01}},
	
	{0x11, 0, {0x00}},
	{REGFLAG_DELAY, 200, {}},
	
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 140, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}

};

static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
  {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;
	LCM_PRINT("%s %d\n", __func__,__LINE__);
    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	LCM_PRINT("%s %d\n", __func__,__LINE__);
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
  	LCM_PRINT("junTang %s %d\n", __func__,__LINE__);
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	 //params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
	 //params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = BURST_VDO_MODE;
//	params->dsi.mode   = SYNC_PULSE_VDO_MODE;	
#endif
	
	// DSI
	/* Command mode setting */

	params->dsi.LANE_NUM				= LCM_FOUR_LANE; 

	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting		
	params->dsi.intermediat_buffer_num = 0;

	params->dsi.vertical_sync_active				= 6;// 3    2
	params->dsi.vertical_backporch					= 18;// 20   1
	params->dsi.vertical_frontporch					= 20; // 1  12
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 50;// 50  2
	params->dsi.horizontal_backporch				= 80;
	params->dsi.horizontal_frontporch				= 70;
	params->dsi.HS_TRAIL                             =20;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.CLK_HS_POST=26;

	#if 1
		params->dsi.clk_lp_per_line_enable 		= 0;
		params->dsi.esd_check_enable 			= 1;
		params->dsi.customization_esd_check_enable 	= 1;
		params->dsi.lcm_esd_check_table[0].cmd          = 0x09;
		params->dsi.lcm_esd_check_table[0].count        = 3;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x80;
		params->dsi.lcm_esd_check_table[0].para_list[1] = 0x73;
		params->dsi.lcm_esd_check_table[0].para_list[2] = 0x06;//04
	#else
		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 1;
		params->dsi.lcm_esd_check_table[0].cmd		   = 0x0a;
		params->dsi.lcm_esd_check_table[0].count	   = 1;
		params->dsi.lcm_esd_check_table[0].para_list[0]   = 0x1c;
		
	#endif


	
#ifdef USE_LCM_THREE_LANE
    params->dsi.PLL_CLOCK = 250;//156;
#else
	params->dsi.PLL_CLOCK = 214;//156;
#endif
	params->dsi.ssc_disable = 1;
}


static unsigned int lcm_compare_id(void){
	return 1;
}



static void lcm_init(void)
{
	unsigned int data_array[16];
	SET_RESET_PIN(1);
	MDELAY(5);
    SET_RESET_PIN(0);
	MDELAY(5);//50
    SET_RESET_PIN(1);
	MDELAY(120);//100
	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);
}


static void lcm_suspend(void)
{
	push_table(lcm_sleep_in_setting,sizeof(lcm_sleep_in_setting)/sizeof(lcm_sleep_in_setting[0]),1);
}



static void lcm_resume(void)
{
	lcm_init();
}


LCM_DRIVER hx8394f_hd720_dsi_vdo_cmi = 
{
    .name			= "hx8394f_hd720_dsi_vdo_cmi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
};
