/*
 * IncFile1.h
 *
 * Created: 07/07/2015 22:56:31
 *  Author: steve
 */ 

#include <stdlib.h>
#include <stdint.h>

#ifndef IIC_DRIVER_H_
#define IIC_DRIVER_H_

/*! 
 * Something more about I2C master driver.
 */
class IICDriverMaster {
private:
  static uint8_t sda;
  static uint8_t scl;
  int speed;
  uint8_t address;
  static bool notInitialised;
public:
  inline static void setPins(uint8_t sda, uint8_t scl) {
     IICDriverMaster::sda = sda;
     IICDriverMaster::scl = scl;     
  }
  IICDriverMaster(uint8_t addr, unsigned int kilohertz);
  
private:
  static IICDriverMaster *owner;
private:
  IICDriverMaster();
  void setClock(unsigned int kilohertz);
public:
  /** initiates a read operation 
   *  @param[in]  address 7 bit address of device to be read from
   *  @param[out] result  pointer to byte for read operation progress. Will be updated with bitwise OR of ResultCode
   */
/*  inline void startRead() {
    readResult = QUEUED;
    start(1);
  }*/
  /** initiates a write operation 
   *  @param[in]  address 7 bit address of device to be read from
   *  @param[out] result  pointer to byte for read operation progress. Will be updated with bitwise OR of ResultCode
   */
  inline void startWrite() {
	  //writeResult = QUEUED;
    start(0);
  }
  /** indicates the end of a message and unless already busy, start sending the message */
  void end();
  /** add one byte to the buffer to be written
   *  @param[in] byte byte to add to message
   */
  void write(uint8_t byte);
  /** add a number of bytes to the bugger to be written
   *  @param[in] bytes Pointer to buffer of bytes to be sent.
   *  @param[in] count Number of bytes in buffer
   */
  void write(const void *bytes, size_t count);
  /** read a number of bytes from the device specified by address
    * @param[in] address 7 bit address of device to be read from
	* @param[out] result Pointer to byte for read status. Can be NULL if status isn't required
	* @param[out] bytes  Pointer to buffer to fill with read data
	* @param[in] count   Number of bytes to read
	*/
  void read(void *bytes, size_t count);
  //void read(size_t count);
  /** Performs a write operation of a number of register byte followed by a string of values
    * @param[in] address 7 bit address of device to be read from
	* @param[out] result Pointer to byte for read status. Can be NULL if status isn't required
	* @param[in] reg Register to be set
	* @param[in] val Pointer to values to be sent
	* @param[in] count Number of bytes to be sent
	*/
  void setRegs(unsigned char reg, void *val, size_t count);
  /** Performs a write operation of a number of register byte followed by a string of values
    * @param[in] address 7 bit address of device to be read from
	* @param[out] result Pointer to byte for read status. Can be NULL if status isn't required
	* @param[in] reg Register to be set
	* @param[in] val Value to be sent
	*/
  void setReg(unsigned char reg, uint8_t val) {
    setRegs(reg, &val, 1);
  }
  /** Asynchronously retreive a set of registers from the addressed device.
    * @param[in] address 7 bit address of device to be read from
	* @param[out] status Pointer to two byte value for read request and read operation status. Can be NULL if status isn't required.
	* @param[in] regAddress Register address to be read from
	* @param[out] buf Pointer to buffer for read data
	* @param[in] count number of bytes to be read into buf
	*/
  void asyncGetRegs(uint8_t regAddress, void *buf, size_t count);
  /** Retreive a set of registers from the addressed device. The method will waiting until successful 
    * results are received from both sent request and data read.
    * @param[in] address 7 bit address of device to be read from
	* @param[in] regAddress Register address to be read from
	* @param[out] buf Pointer to buffer for read data
	* @param[in] count number of bytes to be read into buf
	* @return result codes of request and read operations.
	*/
  int getRegs(uint8_t regAddress, void *buf, size_t count);
  /*inline static void sync() {
	while (txHead != txTail);
  }*/

  //friend void TWI_vect(void);
private:
  //static void startSending();
  void start(uint8_t rw);
};

#endif /* IIC_DRIVER_H_ */
