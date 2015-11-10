#include <stdint.h>
#include <stdbool.h>

#ifndef __BIGINT_HELPER_H_
#define __BIGINT_HELPER_H_


#define BIGINT_DATA_SIZE    sizeof(uint16_t)    // bytes
#define BIGINT_DATA_TYPE    uint16_t
#define BIGINT_DATA_MAX     0xFFFFu
#define BIGINT_DATA_TYPE_2    uint32_t

void     BigIntMod(BIGINT_DATA *b, BIGINT_DATA *a);
BIGINT_DATA_TYPE* BigIntMSB(BIGINT_DATA *n);
uint32_t BigIntMagnitude(BIGINT_DATA *n);
int BigIntMagnitudeDifference(BIGINT_DATA *a, BIGINT_DATA *b);
int BIGINT_Compare(BIGINT_DATA *a, BIGINT_DATA *b);

uint8_t     _addBI(BIGINT_DATA * , BIGINT_DATA *);
uint8_t     _subBI(BIGINT_DATA * , BIGINT_DATA *);
void     _masBI(BIGINT_DATA_TYPE *,BIGINT_DATA_TYPE *,BIGINT_DATA_TYPE,BIGINT_DATA_TYPE *);
BIGINT_DATA_TYPE *     _msbBI(BIGINT_DATA *);
void     _mulBI(BIGINT_DATA *,BIGINT_DATA *,BIGINT_DATA *);
void     _copyBI(BIGINT_DATA *,BIGINT_DATA *);
void     _setBI(BIGINT_DATA *, BIGINT_DATA_TYPE *);


#endif  // __BIGINT_HELPER_H_

