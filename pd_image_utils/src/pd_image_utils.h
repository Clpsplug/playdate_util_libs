#ifndef PD_IMAGE_UTILS_H
#define PD_IMAGE_UTILS_H

#include <pd_api.h>

typedef enum PDImageUtilsBayerTypeTag {
    PDImageUtilsBayer2 = 0,
    PDImageUtilsBayer4,
    PDImageUtilsBayer8,
    BayerTypeTagIs4Bytes = 0x7fffffff,
} PDImageUtilsBayerType;

void pdImageUtils_Initialize(void *pd);

void pdImageUtils_GetBayerDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    PDImageUtilsBayerType type,
    uint8_t threshold
);

void pdImageUtils_Finalize(void);

#endif
