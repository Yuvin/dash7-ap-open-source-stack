/*
 *  Created on: Jan 11, 2013
 *  Authors:
 *  	alexanderhoet@gmail.com
 */

#ifndef FEC_H_
#define FEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint16_t interleave_deinterleave(uint16_t data);


#ifdef __cplusplus
}
#endif

#endif /* FEC_H_ */
