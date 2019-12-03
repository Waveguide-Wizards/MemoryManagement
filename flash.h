/*
 * flash.h
 *
 *  Created on: Dec 1, 2019
 *      Author: hadji
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>

extern void FLASHSendCommand(uint32_t * data, uint32_t size);
extern void FLASHReadResponse(uint32_t * data, uint32_t size);
extern void FLASHInit();

#endif /* FLASH_H_ */
