#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "battery.h"

RAM	uint8_t		lowBattDet_enable = 1;
	uint8_t		adc_hw_initialized = 0;
RAM uint16_t	batt_vol_mv;
RAM	volatile unsigned int adc_dat_buf[8];


//LUT to take the non-linearity of the supercapacitor into account
static const uint8_t   batt_lut[101] =
{
0x00U, 0x02U, 0x04U, 0x06U, 0x08U, 0x09U, 
0x0BU, 0x0DU, 0x0EU, 0x10U, 0x11U, 0x13U, 
0x15U, 0x16U, 0x18U, 0x19U, 0x1BU, 0x1CU, 
0x1EU, 0x1FU, 0x21U, 0x22U, 0x24U, 0x25U, 
0x27U, 0x28U, 0x2AU, 0x2BU, 0x2CU, 0x2DU, 
0x2FU, 0x30U, 0x31U, 0x33U, 0x34U, 0x35U, 
0x37U, 0x38U, 0x3AU, 0x3BU, 0x3DU, 0x3EU, 
0x3FU, 0x40U, 0x42U, 0x43U, 0x44U, 0x45U, 
0x47U, 0x48U, 0x49U, 0x4AU, 0x4BU, 0x4CU, 
0x4DU, 0x4EU, 0x4FU, 0x50U, 0x51U, 0x52U, 
0x53U, 0x53U, 0x54U, 0x55U, 0x56U, 0x57U, 
0x57U, 0x58U, 0x59U, 0x5AU, 0x5AU, 0x5BU, 
0x5BU, 0x5CU, 0x5CU, 0x5DU, 0x5DU, 0x5EU, 
0x5EU, 0x5EU, 0x5FU, 0x5FU, 0x60U, 0x60U, 
0x60U, 0x61U, 0x61U, 0x61U, 0x61U, 0x62U, 
0x62U, 0x62U, 0x62U, 0x62U, 0x63U, 0x63U, 
0x63U, 0x63U, 0x63U, 0x63U, 0x64U
};

_attribute_ram_code_ void adc_bat_init(void)
{
	adc_power_on_sar_adc(0);
	gpio_set_output_en(GPIO_PB5, 1);
	gpio_set_input_en(GPIO_PB5, 0);
	gpio_write(GPIO_PB5, 1);
	adc_set_sample_clk(5);
	adc_set_left_right_gain_bias(GAIN_STAGE_BIAS_PER100, GAIN_STAGE_BIAS_PER100);
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);
	adc_set_state_length(240, 0, 10);
	analog_write(anareg_adc_res_m, RES14 | FLD_ADC_EN_DIFF_CHN_M);
	adc_set_ain_chn_misc(B5P, GND);
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);
	adc_set_tsample_cycle_chn_misc(SAMPLING_CYCLES_6);
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
	adc_power_on_sar_adc(1);
}

_attribute_ram_code_ uint16_t get_battery_mv(void)
{
	uint16_t temp;
	int i,j;
	if(!adc_hw_initialized){
		adc_hw_initialized = 1;
		adc_bat_init();
	}
	adc_power_on_sar_adc(1);	// + 0.4 mA
	adc_reset_adc_module();
	u32 t0 = clock_time();

	uint16_t adc_sample[8] = {0};
	u32 adc_result;
	for(i=0;i<8;i++){
		adc_dat_buf[i] = 0;
	}
	while(!clock_time_exceed(t0, 25)); //wait at least 2 sample cycle(f = 96K, T = 10.4us)
	adc_config_misc_channel_buf((uint16_t *)adc_dat_buf, 8<<2);
	dfifo_enable_dfifo2();
	for(i=0;i<8;i++){
		while(!adc_dat_buf[i]);
		if(adc_dat_buf[i] & BIT(13)){
			adc_sample[i] = 0;
		}
		else{
			adc_sample[i] = ((uint16_t)adc_dat_buf[i] & 0x1FFF);
		}
		if(i){
			if(adc_sample[i] < adc_sample[i-1]){
				temp = adc_sample[i];
				adc_sample[i] = adc_sample[i-1];
				for(j=i-1;j>=0 && adc_sample[j] > temp;j--){
					adc_sample[j+1] = adc_sample[j];
				}
				adc_sample[j+1] = temp;
			}
		}
	}
	dfifo_disable_dfifo2();
	u32 adc_average = (adc_sample[2] + adc_sample[3] + adc_sample[4] + adc_sample[5])/4;
	adc_result = adc_average;
	batt_vol_mv	 = (adc_result * adc_vref_cfg.adc_vref) >> 10;
	adc_power_on_sar_adc(0); // - 0.4 mA
	
	
return batt_vol_mv;
}

_attribute_ram_code_ uint8_t get_battery_level(uint16_t battery_mv){
	uint8_t battery_level = (battery_mv-1900)/(36-19);
	if(battery_level>100)battery_level=100;
	if(battery_mv<1900)battery_level=0;
	return batt_lut[battery_level];
}

