#include<iostream>
#include<unistd.h> //for usleep
#include<string.h>
#include"bme280.h"
#include"US2066.h"
#include"I2CDevice.h"
#include"GPIO.h"

using namespace exploringBB;
using namespace std;


I2CDevice i2c(2,BME280_I2C_ADDRESS1);//0x3c

struct bme280_t BME280;

/*
\
Brief: The function is used as I2C bus read
Return : Status of the I2C read
param dev_addr : The device address of the sensor
param reg_addr : Address of the first register, will data is going to be read
param reg_data : This data read from the sensor, which is hold in an array
param cnt : The no of byte of data to be read
*/

int8_t BME280_I2C_bus_read(uint8_t dev_addr,uint8_t reg_addr,uint8_t *reg_data, uint8_t cnt){

	unsigned char* data = i2c.readRegisters(cnt,reg_addr);
	if(data == NULL){
		return 1;
	}

	memcpy(reg_data , data,cnt);

	return 0;

}

/*
\
Brief: The function is used as I2C bus write
Return :
Status of the I2C write
param dev_addr : The device address of the sensor
param reg_addr : Address of the first register, will data is going to be written
param reg_data : It is a value hold in the array,will be used for write the value into the register
paramcnt: The no of byte of data to be write
*/

int8_t BME280_I2C_bus_write(uint8_t dev_addr,uint8_t reg_addr, uint8_t *reg_data,uint8_t cnt){

	return i2c.writeRegisters(reg_addr,reg_data,cnt);


}

/**
 *\Brief met le programme en attente pour aumoins le nombre spécifique de milisecondes
 *\param: ms Le nombre de milisecondes a attendre
 */
void wait_ms(uint32_t ms){

	usleep(ms*1000);

}

/**
 * \BRief:initialiser le capteur BME280 avec
 * le parametre desirés pour la station meteo
 * elle est écrite en se basant si l'initialisation
 * faite dans la fonction BME280_data_readout_template
 * du fichier bme280_sppot.h
 */
void BME280_Initialiser(void){
	//int com_rslt;
	//I2C_routine();// a faire

	BME280.bus_write = BME280_I2C_bus_write;
	BME280.bus_read = BME280_I2C_bus_read;
	BME280.delay_msec = wait_ms;
	BME280.dev_addr = BME280_I2C_ADDRESS1;
	/*--------------------------------------------------------------------------*
	 *  This function used to assign the value/reference of
	 *	the following parameters
	 *	I2C address
	 *	Bus Write
	 *	Bus read
	 *	Chip id
	*-------------------------------------------------------------------------*/
		//com_rslt =
				bme280_init(&BME280);

		/*	For initialization it is required to set the mode of
		 *	the sensor as "NORMAL"
		 *	data acquisition/read/write is possible in this mode
		 *	by using the below API able to set the power mode as NORMAL*/
		/* Set the power mode as NORMAL*/
		//com_rslt +=
				bme280_set_power_mode(BME280_SLEEP_MODE);
		/*	For reading the pressure, humidity and temperature data it is required to
		 *	set the OSS setting of humidity, pressure and temperature
		 * The "BME280_CTRLHUM_REG_OSRSH" register sets the humidity
		 * data acquisition options of the device.
		 * changes to this registers only become effective after a write operation to
		 * "BME280_CTRLMEAS_REG" register.
		 * In the code automated reading and writing of "BME280_CTRLHUM_REG_OSRSH"
		 * register first set the "BME280_CTRLHUM_REG_OSRSH" and then read and write
		 * the "BME280_CTRLMEAS_REG" register in the function*/
		//com_rslt +=
				bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);

		/* set the pressure oversampling*/
		//com_rslt +=
				bme280_set_oversamp_pressure(BME280_OVERSAMP_1X);
		/* set the temperature oversampling*/
		//com_rslt +=
				bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);
}
