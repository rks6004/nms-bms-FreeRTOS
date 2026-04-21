/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adbms_Application.c
* @brief:   adbms application test cases
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! \addtogroup APPLICATION
*  @{
*/

/*! @addtogroup Application
*  @{
*/

#include "adBms6830Driver.h"
#include "math.h"

/**
*******************************************************************************
* @brief Setup Variables
* The following variables can be modified to configure the software.
*******************************************************************************
*/

/* Set Under Voltage and Over Voltage Thresholds */
const float OV_THRESHOLD = MAX_CELL_VOLTAGE;                 /* Volt */
const float UV_THRESHOLD = MIN_CELL_VOLTAGE;                 /* Volt */
const float OT_THRESHOLD = 60.0;                /* Temperature(Celsius) */
const float UT_THRESHOLD = -20.0;                /* Temperature(Celsius) */
const int OWC_Threshold = 2000;                 /* Cell Open wire threshold(mili volt) */
const int OWA_Threshold = 50000;                /* Aux Open wire threshold(mili volt) */


/**
 * @brief Configure ConfigA and ConfigB for standard operation
 */
void adBms6830_init_config(uint8_t tIC, cell_asic_6830 *ic)
{
  // Iterate through every ADBMS6830 in the chain to configure all of them
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    // Enable REFON bit to wake up the IC
    ic[cic].tx_cfga.refon = PWR_UP;
    // Set CTH register to 10.05 mV
    ic[cic].tx_cfga.cth = CVT_10_05mV;
    // All GPIO pull down off to perform ADC reads
    ic[cic].tx_cfga.gpo = 0X3FF; 

    /* Init config B */
    // Reset DTMEN register
    ic[cic].tx_cfgb.dtmen = DTMEN_OFF;
    // Set OV and UV thresholds to the global defines
    ic[cic].tx_cfgb.vov = SetOverVoltageThreshold(OV_THRESHOLD);
    ic[cic].tx_cfgb.vuv = SetUnderVoltageThreshold(UV_THRESHOLD);
    // Reset DCTO register
    SetConfigB_DischargeTimeOutValue(tIC, &ic[cic], RANG_0_TO_63_MIN, DCTO_TIMEOUT);
    // Set all DCC registers to 0
    ic[cic].tx_cfgb.dcc = (uint16_t)(0x0);
  }
}

/**
 * @brief Initialize standard operation cell voltage measurement
 */
void adBms6830_init_measurements(uint8_t tIC, cell_asic_6830 *ic)
{
  // Wake up IC
  adBmsWakeupIc(tIC);
  // Send ADCV command to initialize C-ADCs conversions (check datasheet for more details)
  adBms6830_Adcv(RD_OFF, CONTINUOUS, DCP_OFF, RSTF_OFF, OW_OFF_ALL_CH);
  Delay_ms(1); // ADCs are updated at their conversion rate is 1ms
  // Send ADCV command to initialize C-ADCs conversions (check datasheet for more details)
  adBms6830_Adcv(RD_ON, CONTINUOUS, DCP_OFF, RSTF_OFF, OW_OFF_ALL_CH);
  Delay_ms(1); // ADCs are updated at their conversion rate is 1ms
  // Send ADSV command to initialize S-ADCs conversions (check datasheet for more details)
  adBms6830_Adsv(CONTINUOUS, DCP_OFF, OW_OFF_ALL_CH);
  Delay_ms(8); // ADCs are updated at their conversion rate is 8ms
}

/**
 * @brief Initialize ConfigA and ConfigB for charging operation (need to balance cells)
 */
void adBms6830_init_charging_config(uint8_t tIC, cell_asic_6830 *ic)
{
  // Iterate through every ADBMS6830 in the chain to configure all of them
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    // Enable REFON bit to wake up the IC
    ic[cic].tx_cfga.refon = PWR_UP;
    // Set CTH register to 10.05 mV
    ic[cic].tx_cfga.cth = CVT_10_05mV;
    // All GPIO pull down off to perform ADC reads
    ic[cic].tx_cfga.gpo = 0X3FF; 

    /* Init config B */
    // Enable DTMEN to allow the ADBMS6830 to enter extended balancing state 
    // (check datasheet for more details)
    ic[cic].tx_cfgb.dtmen = DTMEN_ON;
    // Set OV and UV thresholds to the global defines
    ic[cic].tx_cfgb.vov = SetOverVoltageThreshold(OV_THRESHOLD);
    ic[cic].tx_cfgb.vuv = SetUnderVoltageThreshold(UV_THRESHOLD);
    // Set DCTO (Discharge Timeout) to 5 minutes to allow the chip to continue balancing
    // in the extended balancing state
    SetConfigB_DischargeTimeOutValue(tIC, &ic[cic], RANG_0_TO_63_MIN, TIME_5MIN_OR_1_33HR);
    // Set all DCC registers to 0
    ic[cic].tx_cfgb.dcc = (uint16_t)(0x0);
  }
}

/**
 * @brief Initialize cell voltage measurements for charging operation (need to balance cells)
 */
void adBms6830_init_charging_measurements(uint8_t tIC, cell_asic_6830 *ic)
{
  adBmsWakeupIc(tIC);
  // Send ADCV command to initialize C-ADCs conversions (check datasheet for more details)
  adBms6830_Adcv(RD_ON, CONTINUOUS, DCP_OFF, RSTF_OFF, OW_OFF_ALL_CH);
  Delay_ms(1); // ADCs are updated at their conversion rate is 1ms
}

/**
 * @brief Read average cell voltages registers
 */
void adBms6830_read_avgcell_voltages(uint8_t tIC, cell_asic_6830 *ic)
{
  // Read average cell voltage registers (check datasheet and adBms6830ReadData
  // implementation for more details)
  adBms6830ReadData(tIC, &ic[0], RDACA, AvgCell, A);
  adBms6830ReadData(tIC, &ic[0], RDACB, AvgCell, B);
  adBms6830ReadData(tIC, &ic[0], RDACC, AvgCell, C);
  adBms6830ReadData(tIC, &ic[0], RDACD, AvgCell, D);
  adBms6830ReadData(tIC, &ic[0], RDACE, AvgCell, E);
  adBms6830ReadData(tIC, &ic[0], RDACF, AvgCell, F);
}

/**
 * @brief Initialize auxiliary voltage conversions
 */
void adBms6830_start_aux_voltage_measurment(uint8_t tIC, cell_asic_6830 *ic)
{
  // Initialize Auxiliary ADC conversion (check the datasheet for more details) and poll the ADC
  adBms6830_Adax(AUX_OW_OFF, PUP_DOWN, AUX_ALL);
  adBmsPollAdc(PLAUX1);
}

/**
 * @brief Read auxiliary voltage registers
 */
void adBms6830_read_aux_voltages(uint8_t tIC, cell_asic_6830 *ic)
{
  // implementation for more details)
  // adBmsWakeupIc(tIC);
  adBms6830ReadData(tIC, &ic[0], RDAUXA, Aux, A);
  adBms6830ReadData(tIC, &ic[0], RDAUXB, Aux, B);
  adBms6830ReadData(tIC, &ic[0], RDAUXC, Aux, C);
  adBms6830ReadData(tIC, &ic[0], RDAUXD, Aux, D);
}

/**
 * @brief Read cell voltage registers for charging operation (during balancing)
 */
void adBms6830_charging_voltage_measurement(uint8_t tIC, cell_asic_6830* ic)
{
  // Enable Redundant Measurements
  adBms6830_Adsv(CONTINUOUS, DCP_OFF, OW_OFF_ALL_CH);
  Delay_ms(20); // ADCs are updated at their conversion rate is 8ms
  
  // read all Cell Voltage Registers
  adBmsWakeupIc(tIC);
  adBms6830ReadData(tIC, &ic[0], RDACA, AvgCell, A);
  adBms6830ReadData(tIC, &ic[0], RDACB, AvgCell, B);
  adBms6830ReadData(tIC, &ic[0], RDACC, AvgCell, C);
  adBms6830ReadData(tIC, &ic[0], RDACD, AvgCell, D);
  adBms6830ReadData(tIC, &ic[0], RDACE, AvgCell, E);
  adBms6830ReadData(tIC, &ic[0], RDACF, AvgCell, F);

  // Reset ADSV
  adBms6830_Adsv(SINGLE, DCP_ON, OW_OFF_ALL_CH);

}

/**
 * @brief Function to convert raw ADC voltage readings to a floating point voltage
 * 
 * @param adc_cell_voltage raw ADC value
 * @return float 
 */
static float get_voltage(int16_t adc_cell_voltage)
{
  return ((adc_cell_voltage + 10000) * 0.000150);
}

/**
 * @brief Balance cells
 */
void adBms6830_balance_cells(uint8_t tIC, uint8_t t2950IC, cell_asic_6830* ic)
{
  int16_t min_cell_volt = INT16_MAX;
  // Iterate for every ADBMS6830 in the chain
  for(int current_ic = t2950IC; current_ic < tIC; current_ic ++)
  {
    // Find min cell voltage by iterating through all the cells in the segment 
    // and updating min_cell_volt
    for(int current_cell = 0; current_cell < CELL; current_cell ++)
    {
      if (ic[current_ic].acell.ac_codes[current_cell] < min_cell_volt)
      {
        min_cell_volt = ic[current_ic].acell.ac_codes[current_cell];
      }
    }
  }

  // Iterate for every ADBMS6830 in the chain
  for(int current_ic = t2950IC; current_ic < tIC; current_ic ++)
  {
    // Enable PWM registers of cells that are not the min
    // Iterate through all the cells
    for(int current_cell = 0; current_cell < CELL; current_cell ++)
    {
      // Check if the current cell is greater than the min_cell_volt + 0.01V (10 mV)
      if(get_voltage(ic[current_ic].acell.ac_codes[current_cell]) > (get_voltage(min_cell_volt) + 0.01))
      {
        // Configure PWM registers to 100% duty cycle, so 1111
        // Check datasheet for more details
        if(current_cell < 12)
        {
          ic[current_ic].PwmA.pwma[current_cell] = 0xF;
        }
        else
        {
          ic[current_ic].PwmB.pwmb[current_cell - 12] = 0xF;
        }
      }
    }
  }

  // Send PWM message to all ICs in the chain
  adBms6830WriteData(tIC, &ic[0], WRPWMA, Pwm, A);
  adBms6830WriteData(tIC, &ic[0], WRPWMB, Pwm, B);

  // Reset all PWM registers to 0 to not affect next balancing operation
  for(int current_ic = t2950IC; current_ic < tIC; current_ic ++)
  {
    for(int i = 0; i < PWMA; i ++)
    {
      ic[current_ic].PwmA.pwma[i] = 0;
    }

    for(int i = 0; i < PWMB; i++)
    {
      ic[current_ic].PwmB.pwmb[i] = 0;
    }
  }
}

/**
 * @brief Method to convert AUX adc voltage into temperature in celsius based
 * on the thermistor's ratings. We use an NTC thermistors and ChatGPT kindly 
 * provided the formula to calculate the temperature given the voltage.
 * 
 * @param voltage voltage across the thermistor
 * @return float 
 */
static float get_temperature(float voltage)
{
  // Reference voltage
  float vref = 3.0;
  // Reference temperature 25C
  float T0 = 298.15;
  // Nominal resistance of the thermistor
  float R0 = 10000;
  // Resistance of the pull up resistor on the PCB
  float Rfixed = 10000;
  // Beta value of the thermistor
  float b = 3380;
  // Equation to calculate the temperature in Kkelvin
  float T = 1/(((log((Rfixed * (voltage/(vref - voltage)))/R0))/b) + (1/T0));
  // Return the temperature in celsius
  return T - 273;
}

/**
 * @brief Convert raw AUX ADC readings to temperatures in celsius
 */
void adBms6830_populate_cell_temps(uint8_t tIC, cell_asic_6830* ic)
{
  for(int current_ic = 0; current_ic < tIC; current_ic ++)
  {
    for (int current_aux = 0; current_aux < (AUX - 2); current_aux ++)
    {
      // Convert raw AUX ADC reading to floating-point voltage and then to temperature sensors
      ic[current_ic].cell_temperatures.cell_temps[current_aux] = get_temperature(get_voltage(ic[current_ic].aux.a_codes[current_aux]));
    }
  }
}
