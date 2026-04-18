#include "adBmsCommonDriver.h"

/**
 * @brief Write commands to all ICs in the BMS chain
 * 
 */
void adBmsCommonWriteData(uint8_t t6830IC, uint8_t t2950IC, cell_asic_6830 *ic_6830, cell_asic_2950 *ic_2950, uint8_t cmd_arg[2], TYPE type, GRP group){
    /**
     * @note Consider there are 5 6830 and 2 2950, t2950IC = 2, t6830IC = 5 + t2950IC = 5 + 2 = 7 
     */
    
    // set data_len to tx data size and write_size to the total amount of data that needs
    // to be written. t6830IC contains the total number of ICs because we need to account 
    // for dummies ADBMS6830 ICs that are really ADBMS2950 ICs in the chain. For example,
    // for a chain of 2 ADBMS2950 and 5 ADBMS6830, t6830IC = 7
    uint8_t data_len = TX_DATA, write_size = (TX_DATA * (t6830IC));
    // allocate memory to write data
    uint8_t *write_buffer = (uint8_t *)calloc(write_size, sizeof(uint8_t));
    if (write_buffer == NULL)
    {
        printf(" Failed to allocate write_buffer array memory \n");
        exit(0);
    }
    else
    {
        switch (type)
        {
        // configuration register	   
        case Config:	
            switch (group)
            {
            case A:
                // create configuration A for both chips
                adBms2950CreateConfiga(t2950IC, &ic_2950[0]);
                adBms6830CreateConfiga(t6830IC, &ic_6830[0]);
                // add configuration A data to the write buffer for ADBMS2950
                for (uint8_t cic = 0; cic < t2950IC; cic++)
                {
                    for (uint8_t data = 0; data < data_len; data++)
                    {
                        write_buffer[(cic * data_len) + data] = ic_2950[cic].configa.tx_data[data];
                    }
                }
                // add configuration A data to the write buffer for ADBMS6830 starting at t2950IC
                // as t6830IC = (# of ADBMS2950) + (# of ADBMS6830)
                for (uint8_t cic = t2950IC; cic < t6830IC; cic++)
                {
                    for (uint8_t data = 0; data < data_len; data++)
                    {
                        write_buffer[(cic * data_len) + data] = ic_6830[cic].configa.tx_data[data];
                    }
                }
                break;
            case B:
                // create configuration B for both chips
                adBms2950CreateConfigb(t2950IC, &ic_2950[0]);
                adBms6830CreateConfigb(t6830IC, &ic_6830[0]);
                // add configuration B data to the write buffer for ADBMS2950
                for (uint8_t cic = 0; cic < t2950IC; cic++)
                {
                    for (uint8_t data = 0; data < data_len; data++)
                    {
                        write_buffer[(cic * data_len) + data] = ic_2950[cic].configb.tx_data[data];
                    }
                }
                // add configuration B data to the write buffer for ADBMS6830 starting at t2950IC
                // as t6830IC = (# of ADBMS2950) + (# of ADBMS6830)
                for (uint8_t cic = t2950IC; cic < t6830IC; cic++)
                {
                    for (uint8_t data = 0; data < data_len; data++)
                    {
                        write_buffer[(cic * data_len) + data] = ic_6830[cic].configb.tx_data[data];
                    }
                }
                break;
            }
            break;

        // comm register
        case Comm:
            // create comm configuration for both chips
            adBms2950CreateComm(t2950IC, &ic_2950[0]);
            adBms6830CreateComm(t6830IC, &ic_6830[0]);
            // add com data to the write buffer for ADBMS2950
            for (uint8_t cic = 0; cic < t2950IC; cic++)
            {
                for (uint8_t data = 0; data < data_len; data++)
                {
                    write_buffer[(cic * data_len) + data] = ic_2950[cic].com.tx_data[data];
                }
            }
            // add comm data to the write buffer for ADBMS6830 starting at t2950IC
            // as t6830IC = (# of ADBMS2950) + (# of ADBMS6830)
            for (uint8_t cic = t2950IC; cic < t6830IC; cic++)
            {
                for (uint8_t data = 0; data < data_len; data++)
                {
                    write_buffer[(cic * data_len) + data] = ic_6830[cic].com.tx_data[data];
                }
            }
            break;
        
        // clrflag register
        case Clrflag:
            // create clrflag configuration for both chips	
            adBms2950CreateClrflagData(t2950IC, &ic_2950[0]);
            adBms6830CreateClrflagData(t6830IC, &ic_6830[0]);
            // add clrflag data to the write buffer for ADBMS2950
            for (uint8_t cic = 0; cic < t2950IC; cic++)
            {
                for (uint8_t data = 0; data < data_len; data++)
                {
                    write_buffer[(cic * data_len) + data] = ic_2950[cic].clrflag.tx_data[data];
                }
            }
            // add clrflag data to the write buffer for ADBMS6830 starting at t2950IC
            // as t6830IC = (# of ADBMS2950) + (# of ADBMS6830)
            for (uint8_t cic = t2950IC; cic < t6830IC; cic++)
            {
                for (uint8_t data = 0; data < data_len; data++)
                {
                    write_buffer[(cic * data_len) + data] = ic_6830[cic].clrflag.tx_data[data];
                }
            }
            break;
    
        default:
            break;
        }
    }

    // write data to the BMS chain over spi (through isoSPI transciever)
    spiWriteData(t6830IC, cmd_arg, &write_buffer[0]);	
    // deallocate memory from write buffer
    free(write_buffer);
}

/**
 * @brief Write configuration registers A and B to all ICs in the BMS chain
 */
void adBmsCommonWriteConfig(uint8_t t6830IC, uint8_t t2950IC, cell_asic_6830 *ic_6830, cell_asic_2950 *ic_2950)
{
    adBmsWakeupIc(t6830IC);
    // write configuration A to all ICs in the BMS chain
    adBmsCommonWriteData(t6830IC, t2950IC, &ic_6830[0], &ic_2950[0], WRCFGA, Config, A);
    // write configuration B to all ICs in the BMS chain
    adBmsCommonWriteData(t6830IC, t2950IC, &ic_6830[0], &ic_2950[0], WRCFGB, Config, B);
}