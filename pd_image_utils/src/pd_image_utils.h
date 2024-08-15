/**
 * @file pd_image_utils.h
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
 * @brief Direction of fill when using vertical or horizontal dithering.
 *
 * When using either
 * pdImageUtils_GetVerticalDither(uint8_t**, uint8_t, uint8_t, uint8_t, uint8_t, PDImageUtilsDitheringDirection)
 * or pdImageUtils_GetHorizontalDither(uint8_t**, uint8_t, uint8_t, uint8_t, uint8_t, PDImageUtilsDitheringDirection),
 * this enum will indicate which side of the mask will be unmasked first.
 */
typedef enum PDImageUtilsDitheringDirectionTag {
    /**
     * @brief Left side or Upper side.
     */
    PDImageUtilsDitheringDirectionPositive = 0,
    /**
     * @brief Right side or Lower side.
     */
    PDImageUtilsDitheringDirectionNegative,
    /**
     * @brief Unused. Exists to force this enum to be 4 bytes.
     */
    DitheringDirectionTagIs4Bytes = 0x7fffffff,
} PDImageUtilsDitheringDirection;

/**
 * @brief Initializes the image manipulation module.
 *
 * @param[in] pd PlaydateAPI context object
 */
void pdImageUtils_Initialize(void *pd);

/**
 * @brief Calculate the vertical dithering mask and write it to a given buffer.
 *
 * The mask will be a repeating max_value pixels wide mask
 * and the threshold pixels wide section will be unmasked.
 *
 * @param[out] buffer    Output buffer. MUST NOT be NULL.
 * @param[in]  row       Row count for the buffer.
 * @param[in]  rb        Row byte count. Get it from @c playdate->graphics->getBitmapData .
 * @param[in]  threshold Threshold to compare against the bayer matrix. Must be 0 <= threshold <= max_value.
 * @param[in]  max_value Max value for threshold. Must be 2 or greater.
 * @param[in]  direction Direction of dithering.
 *                       Positive will unmask left side first while Negative will unmask right side first.
 */
void pdImageUtils_GetVerticalDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    uint8_t threshold,
    uint8_t max_value,
    PDImageUtilsDitheringDirection direction
);

/**
 * @brief Calculate the horizontal dithering mask and write it to a given buffer.
 *
 * The mask will be a repeating max_value tall mask
 * and threshold pixel tall section will be unmasked.
 *
 * @param[out] buffer    Output buffer. MUST NOT be NULL.
 * @param[in]  row       Row count for the buffer.
 * @param[in]  rb        Row byte count. Get it from @c playdate->graphics->getBitmapData .
 * @param[in]  threshold Threshold to compare against the bayer matrix. Must be 0 <= threshold <= max_value.
 * @param[in]  max_value Max value for threshold.
 * @param[in]  direction Direction of dithering.
 *                       Positive will unmask upper side first while Negative will unmask lower side first.
 */
void pdImageUtils_GetHorizontalDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    uint8_t threshold,
    uint8_t max_value,
    PDImageUtilsDitheringDirection direction
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
