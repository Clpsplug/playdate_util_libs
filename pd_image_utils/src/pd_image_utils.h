/**
 * @brief Playdate image manipulation module
 */
#ifndef PD_IMAGE_UTILS_H
#define PD_IMAGE_UTILS_H

#include <pd_api.h>

/**
 * @brief Size of the Bayer matrix to use.
 *
 * Bigger size will result in less grainy but coarser image.
 */
typedef enum PDImageUtilsBayerTypeTag {
    /**
     * @brief Uses 2x2 Bayer matrix.
     */
    PDImageUtilsBayer2 = 0,
    /**
     * @brief Uses 4x4 Bayer matrix.
     */
    PDImageUtilsBayer4,
    /**
     * @brief Uses 8x8 Bayer matrix.
     */
    PDImageUtilsBayer8,
    /**
     * @brief Unused. Exists to force this enum to be 4 bytes.
     */
    BayerTypeTagIs4Bytes = 0x7fffffff,
} PDImageUtilsBayerType;

/**
 * @brief Initializes the image manipulation module.
 *
 * @param[in] pd PlaydateAPI context object
 */
void pdImageUtils_Initialize(void *pd);

/**
 * @brief Calculate the vertical dithering mask and write it to a given buffer.
 *
 * @param[out] buffer    Output buffer. MUST NOT be NULL.
 * @param[in]  row       Row count for the buffer.
 * @param[in]  rb        Row byte count. Get it from @c playdate->graphics->getBitmapData .
 * @param[in]  threshold Threshold to compare against the bayer matrix. Must be 0 <= threshold <= max_value.
 * @param[in]  max_value Max value for threshold.
 */
void pdImageUtils_GetVerticalDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    uint8_t threshold,
    uint8_t max_value
);

/**
 * @brief Calculate the horizontal dithering mask and write it to a given buffer.
 *
 * @param[out] buffer    Output buffer. MUST NOT be NULL.
 * @param[in]  row       Row count for the buffer.
 * @param[in]  rb        Row byte count. Get it from @c playdate->graphics->getBitmapData .
 * @param[in]  threshold Threshold to compare against the bayer matrix. Must be 0 <= threshold <= max_value.
 * @param[in]  max_value Max value for threshold.
 */
void pdImageUtils_GetHorizontalDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    uint8_t threshold,
    uint8_t max_value
);

/**
 * @brief Calculate the Bayer matrix dithering mask and write it to a given buffer.
 *
 * @param[out] buffer    Output buffer. MUST NOT be NULL.
 * @param[in]  row       Row count for the buffer.
 * @param[in]  rb        Row byte count. Get it from @c playdate->graphics->getBitmapData .
 * @param[in]  type      Bayer matrix type.
 * @param[in]  threshold Threshold to compare against the bayer matrix. Must be 0 <= threshold < 2^(bayer matrix size).
 */
void pdImageUtils_GetBayerDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    PDImageUtilsBayerType type,
    uint8_t threshold
);

/**
 * @brief Finalizes the image manipulation module.
 */
void pdImageUtils_Finalize(void);

#endif
