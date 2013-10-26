/*
 *  linux/drivers/sound/jzcodec.c
 *
 *  JzSOC internal audio driver.
 *
 */
#include <linux/init.h>
#include <linux/interrupt.h>
#include <sound/driver.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include <linux/sound.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <linux/proc_fs.h>
#include <linux/soundcard.h>
#include <linux/dma-mapping.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <asm/hardirq.h>
#include <asm/jzsoc.h>

#include "sound_config.h"
#include "jzdlv.h"

#define USE_NONE 1
#define USE_MIC 2
#define USE_LINEIN 3

extern mixer_info info;
extern _old_mixer_info old_info;
extern int codec_volue_shift;

extern void (*set_codec_mode)(void);
extern void (*each_time_init_codec)(void);
extern int (*set_codec_startup_param)(void);
extern void (*set_codec_record)(void);
extern void (*set_codec_replay)(void);
extern void (*set_codec_replay_record)(void);
extern void (*turn_on_codec)(void);
extern void (*turn_off_codec)(void);
extern void (*set_codec_speed)(int rate);
extern void (*reset_codec)(void);
extern void (*codec_mixer_old_info_id_name)(void);
extern void (*codec_mixer_info_id_name)(void);
extern void (*set_codec_bass)(int val);
extern void (*set_codec_volume)(int val);
extern void (*set_codec_mic)(int val);
extern void (*set_codec_line)(int val);
extern void (*i2s_resume_codec)(void);
extern void (*i2s_suspend_codec)(void);
extern void (*set_codec_direct_mode)(void);
extern void (*clear_codec_direct_mode)(void);


void set_dlv_mode(void);
void each_time_init_jzcodec(void);
int set_dlv_startup_param(void);
void set_dlvjzcodec_volume_table(void);
void set_dlv_replay(void);
void set_dlv_record(void);
void set_dlv_speed(int rate);
void reset_dlv(void);
void jzcodec_mixer_old_info_id_name(void);
void jzcodec_mixer_info_id_name(void);
void set_dlv_volume(int val);
void set_dlv_mic(int val);

extern int jz_mic_only;
int read_codec_file(int addr)
{
	while (__icdc_rgwr_ready());
	__icdc_set_addr(addr);
	mdelay(1);
	return(__icdc_get_value());
}

#if 0
void printk_codec_files(void)
{
	int cnt;

	printk("\n");
	
	printk("REG_CPM_I2SCDR=0x%08x\n",REG_CPM_I2SCDR);
	printk("REG_CPM_CLKGR=0x%08x\n",REG_CPM_CLKGR);
	printk("REG_CPM_CPCCR=0x%08x\n",REG_CPM_CPCCR);
	printk("REG_AIC_FR=0x%08x\n",REG_AIC_FR);
	printk("REG_AIC_CR=0x%08x\n",REG_AIC_CR);		
	printk("REG_AIC_I2SCR=0x%08x\n",REG_AIC_I2SCR);
	printk("REG_AIC_SR=0x%08x\n",REG_AIC_SR);
	printk("REG_ICDC_RGDATA=0x%08x\n",REG_ICDC_RGDATA);

	for (cnt = 0; cnt <= 27 ; cnt++) {
		printk(" ( %d  :  0x%x ) ",cnt ,read_codec_file(cnt));
	}
	printk("\n");
}
#endif

void write_codec_file(int addr, int val)
{
	while (__icdc_rgwr_ready());
	__icdc_set_addr(addr);
	__icdc_set_cmd(val); /* write */
	mdelay(1);
	__icdc_set_rgwr();
	mdelay(1);
}

int write_codec_file_bit(int addr, int bitval, int mask_bit)
{
	int val;
	while (__icdc_rgwr_ready());
	__icdc_set_addr(addr);
	mdelay(1);
	val = __icdc_get_value(); /* read */

	while (__icdc_rgwr_ready());
	__icdc_set_addr(addr);
	val &= ~(1 << mask_bit);
	if (bitval == 1)
		val |= 1 << mask_bit;

	__icdc_set_cmd(val); /* write */
	mdelay(1);
	__icdc_set_rgwr();
	mdelay(1);

	while (__icdc_rgwr_ready());
	__icdc_set_addr(addr);
	val = __icdc_get_value(); /* read */	
	
	if (((val >> mask_bit) & bitval) == bitval)
		return 1;
	else 
		return 0;
}
void set_dlv_mode(void)
{
	/*REG_CPM_CPCCR &= ~(1 << 31);
	  REG_CPM_CPCCR &= ~(1 << 30);*/
	write_codec_file(0, 0xf);

	REG_AIC_I2SCR = 0x10;
	__i2s_internal_codec();
	__i2s_as_slave();
	__i2s_select_i2s();
	__aic_select_i2s();
	__aic_reset();
	mdelay(10);
	REG_AIC_I2SCR = 0x10;
	mdelay(20);

	/* power on DLV */
	write_codec_file(9, 0xff);
	write_codec_file(8, 0x3f);
	mdelay(10);
}
void reset_dlv_codec(void)
{
	/* reset DLV codec. from hibernate mode to sleep mode */
	write_codec_file(0, 0xf);
	write_codec_file_bit(6, 0, 0);
	write_codec_file_bit(6, 0, 1);
	mdelay(200);
	//write_codec_file(0, 0xf);
	write_codec_file_bit(5, 0, 7);//PMR1.SB_DAC->0
	write_codec_file_bit(5, 0, 4);//PMR1.SB_ADC->0
	mdelay(10);//wait for stability
}

void each_time_init_dlv(void)
{
        __i2s_disable();
	__i2s_as_slave();
	__aic_internal_codec();
	__i2s_set_oss_sample_size(16);
	__i2s_set_iss_sample_size(16);
}

int set_dlv_startup_param(void)
{
	__i2s_disable_transmit_intr();
	__i2s_disable_receive_intr();

	return 1;
}
/* set Audio data replay */
void set_audio_data_replay(void)
{
	/* DAC path */
	write_codec_file(9, 0xff);
	//write_codec_file(8, 0x30);
	write_codec_file(8, 0x20);
	mdelay(10);
	write_codec_file_bit(1, 0, 4);//CR1.HP_DIS->0
	write_codec_file_bit(5, 1, 3);//PMR1.SB_LIN->1
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1
	
	write_codec_file_bit(1, 0, 2);//CR1.BYPASS->0
	write_codec_file_bit(1, 1, 3);//CR1.DACSEL->1
	
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
	//mdelay(100);
	//write_codec_file_bit(5, 0, 6);//PMR1.SB_OUT->0
	write_codec_file_bit(1, 1, 7);//CR1.SB_MICBIAS->1
	//mdelay(300);
}

#if 1 /* mask warning */
/* set Record MIC input audio without playback */
void set_record_mic_input_audio_without_playback(void)
{
	/* ADC path for MIC IN */
	jz_mic_only = 1;
	write_codec_file(9, 0xff);
	write_codec_file(8, 0x3f);
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	mdelay(10);
	write_codec_file_bit(1, 1, 2);
	//write_codec_file_bit(1, 1, 6);//CR1.MONO->1
	
	write_codec_file(22, 0x40);//mic 1
	write_codec_file_bit(3, 1, 7);//CR1.HP_DIS->1
	write_codec_file_bit(5, 1, 3);//PMR1.SB_LIN->1
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1
	
	write_codec_file_bit(1, 0, 2);//CR1.BYPASS->0
	write_codec_file_bit(1, 0, 3);//CR1.DACSEL->0
	//write_codec_file_bit(6, 1, 3);// gain set
	
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
	mdelay(100);
	write_codec_file_bit(5, 0, 6);//PMR1.SB_OUT->0
	write_codec_file(1, 0x4);
}
#endif

#if 1 /* mask warning */
/* unset Record MIC input audio without playback */
void unset_record_mic_input_audio_without_playback(void)
{
	/* ADC path for MIC IN */
	jz_mic_only = 0;
	write_codec_file_bit(5, 1, 4);//SB_ADC->1
	write_codec_file_bit(1, 1, 7);//CR1.SB_MICBIAS->1
	write_codec_file(22, 0xc0);//CR3.SB_MIC1
	write_codec_file_bit(5, 1, 6);//PMR1.SB_OUT->1
	write_codec_file_bit(1, 1, 5);//DAC_MUTE->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

#if 0 /* mask warning */
/* set Record LINE input audio without playback */
void set_record_line_input_audio_without_playback(void)
{
	/* ADC path for LINE IN */	
	jz_mic_only = 1;
	write_codec_file(9, 0xff);
	write_codec_file(8, 0x3f);
	mdelay(10);
	write_codec_file(22, 0xf6);//line in 1
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	write_codec_file_bit(3, 1, 7);//CR1.HP_DIS->1
	write_codec_file_bit(5, 0, 3);//PMR1.SB_LIN->0
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1
	
	write_codec_file_bit(1, 0, 2);//CR1.BYPASS->0
	write_codec_file_bit(1, 0, 3);//CR1.DACSEL->0
	mdelay(10);
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
	mdelay(100);
	write_codec_file_bit(5, 0, 6);//PMR1.SB_OUT->0
	write_codec_file(1, 0x4);
}
#endif

#if 0 /* mask warning */ 
/* unset Record LINE input audio without playback */
void unset_record_line_input_audio_without_playback(void)
{
	/* ADC path for LINE IN */
	write_codec_file_bit(5, 1, 4);//SB_ADC->1
	write_codec_file_bit(5, 1, 3);//ONR1.SB_LIN->1
	
	write_codec_file(22, 0xc0);//CR3.SB_MIC1
	write_codec_file_bit(5, 1, 6);//PMR1.SB_OUT->1
	write_codec_file_bit(1, 1, 5);//DAC_MUTE->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

#if 0 /* mask warning */
/* set Playback LINE input audio direct only */
void set_playback_line_input_audio_direct_only(void)
{
	jz_audio_reset();//or init_codec()
	REG_AIC_I2SCR = 0x10;
	write_codec_file(9, 0xff);
	write_codec_file(8, 0x3f);
	mdelay(10);
	write_codec_file(22, 0xf6);//line in 1
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	mdelay(10);
	write_codec_file_bit(1, 1, 2);//CR1.HP_BYPASS->1
	write_codec_file_bit(1, 0, 4);//CR1.HP_DIS->0
	write_codec_file_bit(1, 0, 3);//CR1.DACSEL->0
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1
	write_codec_file_bit(5, 0, 3);//PMR1.SB_LIN->0

	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
	mdelay(100);
	write_codec_file_bit(5, 0, 6);//PMR1.SB_OUT->0
	//write_codec_file_bit(5, 1, 7);//PMR1.SB_DAC->1
	//write_codec_file_bit(5, 1, 4);//PMR1.SB_ADC->1
}
#endif

#if 0 /* mask warning */
/* unset Playback LINE input audio direct only */
void unset_playback_line_input_audio_direct_only(void)
{
	write_codec_file_bit(6, 0, 3);//GIM->0
	write_codec_file_bit(1, 0, 2);//PMR1.BYPASS->0
	write_codec_file_bit(5, 1, 3);//PMR1.SB_LINE->1
	write_codec_file_bit(5, 1, 6);//PMR1.SB_OUT->1
	mdelay(100);
	write_codec_file_bit(5, 1, 5);//PMR1.SB_MIX->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

#if 0 /* mask warning */
/* set Record MIC input audio with direct playback */
void set_record_mic_input_audio_with_direct_playback(void)
{		
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	jz_mic_only = 0;
	write_codec_file(9, 0xff);
	write_codec_file(8, 0x3f);
	mdelay(10);
	
	write_codec_file(22, 0x60);//mic 1
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	write_codec_file_bit(5, 1, 3);//PMR1.SB_LIN->1
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1
	write_codec_file_bit(1, 0, 7);//CR1.SB_MICBIAS->0
	write_codec_file_bit(1, 0, 4);//CR1.HP_DIS->0
	
	write_codec_file_bit(1, 0, 2);//CR1.BYPASS->0
	write_codec_file_bit(1, 0, 3);//CR1.DACSEL->0
	write_codec_file_bit(6, 1, 3);// gain set
	
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
	mdelay(100);
	write_codec_file_bit(5, 0, 6);//PMR1.SB_OUT->0
	//write_codec_file(1, 0x4);
}
#endif

#if 0 /* mask warning */
/* unset Record MIC input audio with direct playback */
void unset_record_mic_input_audio_with_direct_playback(void)
{
	/* ADC path for MIC IN */
	jz_mic_only = 0;
	write_codec_file_bit(5, 1, 4);//SB_ADC->1
	write_codec_file_bit(1, 1, 7);//CR1.SB_MICBIAS->1
	write_codec_file(22, 0xc0);//CR3.SB_MIC1
	write_codec_file_bit(5, 1, 6);//PMR1.SB_OUT->1
	write_codec_file_bit(1, 1, 5);//DAC_MUTE->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

#if 0 /* mask warning */
/* set Record playing audio mixed with MIC input audio */
void set_record_playing_audio_mixed_with_mic_input_audio(void)
{
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	write_codec_file(9, 0xff);
	//write_codec_file(8, 0x30);
	write_codec_file(8, 0x20);
	mdelay(10);
	
	write_codec_file(22, 0x63);//mic 1
	
	write_codec_file_bit(1, 0, 2);//CR1.BYPASS->0
	write_codec_file_bit(6, 1, 3);// gain set

	write_codec_file_bit(1, 0, 4);//CR1.HP_DIS->0	
	write_codec_file_bit(5, 1, 3);//PMR1.SB_LIN->1
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1
	write_codec_file_bit(1, 0, 7);//CR1.SB_MICBIAS->0
	write_codec_file_bit(22, 0, 7);//CR3.SB_MIC->0
	write_codec_file_bit(1, 1, 3);//CR1.DACSEL->1
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
	write_codec_file_bit(5, 0, 4);//PMR1.SB_MIX->0
}
#endif

#if 0 /* mask warning */
/* unset Record playing audio mixed with MIC input audio */
void unset_record_playing_audio_mixed_with_mic_input_audio(void)
{
	/* ADC path */
	write_codec_file_bit(5, 1, 4);//SB_ADC->1
	write_codec_file_bit(1, 1, 7);//CR1.SB_MICBIAS->1
	//write_codec_file_bit(1, 1, 6);//CR1.MONO->1
	write_codec_file(22, 0xc0);//CR3.SB_MIC1->1
	//write_codec_file_bit(1, 1, 5);//DAC_MUTE->1
	//write_codec_file_bit(5, 1, 6);//SB_OUT->1
	write_codec_file_bit(5, 1, 7);//SB_DAC->1
	write_codec_file_bit(5, 1, 5);//SB_MIX->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

#if 1 /* mask warning */
/* set Record MIC input audio with Audio data replay (full duplex) */
void set_record_mic_input_audio_with_audio_data_replay(void)
{		
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	write_codec_file(9, 0xff);
	//write_codec_file(8, 0x30);
	write_codec_file(8, 0x20);
	write_codec_file_bit(1, 0, 4);//CR1.HP_DIS->0	
	write_codec_file_bit(5, 1, 3);//PMR1.SB_LIN->1
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1

	write_codec_file_bit(22, 0, 7);//CR3.SB_MIC->0
	write_codec_file_bit(1, 0, 7);//CR1.SB_MICBIAS->0

	write_codec_file_bit(1, 1, 3);//CR1.DACSEL->1
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
}
#endif

#if 1 /* mask warning */
/* unset Record MIC input audio with Audio data replay (full duplex) */
void unset_record_mic_input_audio_with_audio_data_replay(void)
{
	/* ADC path */
	write_codec_file_bit(5, 1, 4);//SB_ADC->1
	write_codec_file_bit(1, 1, 7);//CR1.SB_MICBIAS->1
	//write_codec_file_bit(1, 1, 6);//CR1.MONO->1
	write_codec_file(22, 0xc0);//CR3.SB_MIC1->1
	write_codec_file_bit(5, 1, 7);//SB_DAC->1
	write_codec_file_bit(5, 1, 5);//SB_MIX->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

#if 1 /* mask warning */
/* set Record LINE input audio with Audio data replay (full duplex for linein) */
void set_record_line_input_audio_with_audio_data_replay(void)
{		
	write_codec_file(9, 0xff);
	//write_codec_file(8, 0x30);
	write_codec_file(8, 0x20);
	write_codec_file_bit(1, 0, 4);//CR1.HP_DIS->0	
	write_codec_file_bit(5, 0, 3);//PMR1.SB_LIN->0
	write_codec_file_bit(5, 1, 0);//PMR1.SB_IND->1
	write_codec_file_bit(1, 1, 7);//CR1.SB_MICBIAS->1
	//write_codec_file_bit(22, 1, 7);//CR3.SB_MIC->1
	write_codec_file_bit(1, 1, 3);//CR1.DACSEL->1
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
 

	//jz_mic_only = 1;
	write_codec_file(22, 0xc6);//line in 1
	write_codec_file_bit(23, 0, 7);//AGC1.AGC_EN->0
	write_codec_file_bit(1, 0, 2);//CR1.BYPASS->0
	write_codec_file_bit(5, 0, 5);//PMR1.SB_MIX->0
}
#endif

#if 1 /* mask warning */
/* unset Record LINE input audio with Audio data replay (full duplex for linein) */
void unset_record_line_input_audio_with_audio_data_replay(void)
{
	/* ADC path */
	write_codec_file_bit(5, 1, 4);//SB_ADC->1
	write_codec_file_bit(1, 1, 7);//CR1.SB_MICBIAS->1
	//write_codec_file_bit(1, 1, 6);//CR1.MONO->1
	write_codec_file(22, 0xc0);//CR3.SB_MIC1->1
	write_codec_file_bit(5, 1, 7);//SB_DAC->1
	write_codec_file_bit(5, 1, 5);//SB_MIX->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

#if 1
/* unset Audio data replay */
void unset_audio_data_replay(void)
{
	//write_codec_file_bit(1, 1, 5);//DAC_MUTE->1
	//mdelay(800);
	//write_codec_file_bit(5, 1, 6);//SB_OUT->1
	//mdelay(800);
	write_codec_file_bit(5, 1, 7);//SB_DAC->1
	write_codec_file_bit(5, 1, 4);//SB_MIX->1
	write_codec_file_bit(6, 1, 0);//SB_SLEEP->1
	write_codec_file_bit(6, 1, 1);//SB->1
}
#endif

void set_dlv_replay(void)
{
	set_audio_data_replay();
}

void set_dlv_speed(int rate)
{
	int speed = 0, val;
	speed = 0;
	switch (rate) {
	case 8000:
		speed = 10;
		break;
	case 9600:
		speed = 9;
		break;
	case 11025:
		speed = 8;
		break;
	case 12000:
		speed = 7;
		break;
	case 16000:
		speed = 6;
		break;
	case 22050:
		speed = 5;
		break;
	case 24000:
		speed = 4;
		break;
	case 32000:
		speed = 3;
		break;
	case 44100:
		speed = 2;
		break;
	case 48000:
		speed = 1;
		break;
	case 96000:
		speed = 0;
		break;
	default:
		break;
	}

	val = read_codec_file(4);
	val = (speed << 4) | speed;
	write_codec_file(4, val);
}

void reset_jzcodec(void)
{

}

void dlv_mixer_old_info_id_name(void)
{
	strncpy(info.id, "JZDLV", sizeof(info.id));
	strncpy(info.name,"Jz internal codec dlv on jz4750", sizeof(info.name));
}

void dlv_mixer_info_id_name(void)
{
	strncpy(old_info.id, "JZDLV", sizeof(old_info.id));
	strncpy(old_info.name,"Jz internal codec dlv on jz4750", sizeof(old_info.name));
}

void set_dlv_mic(int val)
{
	int cur_vol ;
	/* set gain */
	//write_codec_file_bit(6, 1, 3);//GIM
	cur_vol = 31 * val / 100; 
	cur_vol |= cur_vol << 4;
	write_codec_file(19, cur_vol);//GIL,GIR
}

void set_dlv_line(int val)
{
	int cur_vol;
	/* set gain */
	cur_vol = 31 * val / 100; 
	cur_vol &= 0x1f;
	write_codec_file(11, cur_vol);//GO1L
	write_codec_file(12, cur_vol);//GO1R
}

void set_dlv_volume(int val)
{
	unsigned long cur_vol;
	cur_vol = 31 * (100 - val) / 100; 
	write_codec_file(17, cur_vol | 0xc0);
	write_codec_file(18, cur_vol);
}

static int __init init_dlv(void)
{
	set_codec_mode = set_dlv_mode;
	each_time_init_codec = each_time_init_dlv;
	reset_codec = reset_dlv_codec;
	set_codec_startup_param = set_dlv_startup_param;

	set_codec_replay = set_dlv_replay;

	set_codec_speed = set_dlv_speed;

	codec_mixer_old_info_id_name = dlv_mixer_old_info_id_name;
	codec_mixer_info_id_name = dlv_mixer_info_id_name;

	set_codec_volume = set_dlv_volume;
	set_codec_mic = set_dlv_mic;
	set_codec_line = set_dlv_line;

	return 0;
}


static void __exit cleanup_dlv(void)
{

}

module_init(init_dlv);
module_exit(cleanup_dlv);
