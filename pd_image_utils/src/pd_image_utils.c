#include "pd_image_utils.h"

#include <pd_api.h>
#include <stdint.h>
#include <stdbool.h>

static PlaydateAPI *s_pd;


/* Hardcoded Bayer matrices */
static const uint8_t bayer2x2[2][2] = {{0, 2},
                                       {3, 1}};

static const uint8_t bayer4x4[4][4] = {{0,  8,  2,  10},
                                       {12, 4,  14, 6},
                                       {3,  11, 1,  9},
                                       {15, 7,  13, 5}};

static const uint8_t bayer8x8[8][8] = {{0,  32, 8,  40, 2,  34, 10, 42},
                                       {48, 16, 56, 24, 50, 18, 58, 26},
                                       {12, 44, 4,  36, 14, 46, 6,  38},
                                       {60, 28, 52, 20, 62, 30, 54, 22},
                                       {3,  35, 11, 43, 1,  33, 9,  41},
                                       {51, 19, 59, 27, 49, 17, 57, 25},
                                       {15, 47, 7,  39, 13, 45, 5,  37},
                                       {63, 31, 55, 23, 61, 29, 53, 21}};

/**
 * @brief bayer2x2 but mapped in 8x8.
 *
 * To be calculated on initialization.
 */
static uint8_t bayer2x2in8[8][8];
/**
 * @brief bayer4x4 but mapped in 8x8
 *
 * To be calculated on initialization.
 */
static uint8_t bayer4x4in8[8][8];


/** @brief Initializes the image manipulation module. */
void pdImageUtils_Initialize(void *pd) {
    s_pd = pd;
    /* Populate the 8x8 version of bayer kernels
     * (I know this is ugly, but it's confined here, so... we'll keep it like this)
     */
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 2; k++) {
                for (int l = 0; l < 2; l++) {
                    bayer2x2in8[2 * i + k][2 * j + l] = bayer2x2[k][l];
                }
            }
        }
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                for (int l = 0; l < 4; l++) {
                    bayer4x4in8[2 * i + k][2 * j + l] = bayer4x4[k][l];
                }
            }
        }
    }
}

/** @brief Calculate the vertical dithering mask and write it to a given buffer. */
void pdImageUtils_GetVerticalDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    uint8_t threshold,
    uint8_t max_value
) {
    /* Check parameter inconsistencies */
    if (buffer == NULL) {
        s_pd->system->error("Invalid reference to buffer (NULL) was passed.");
        return;
    }

    if (rb == 0) {
        s_pd->system->error("Invalid row count was passed.");
        return;
    }

    if (threshold > max_value) {
        s_pd->system->error(
            "Threshold for horizontal dithering filter must be less than %d.",
            max_value
        );
        return;
    }
    /* TODO */
}

/** @brief Calculate the horizontal dithering mask and write it to a given buffer. */
void pdImageUtils_GetHorizontalDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    uint8_t threshold,
    uint8_t max_value
) {
    /* Check parameter inconsistencies */
    if (buffer == NULL) {
        s_pd->system->error("Invalid reference to buffer (NULL) was passed.");
        return;
    }

    if (rb == 0) {
        s_pd->system->error("Invalid row count was passed.");
        return;
    }

    if (threshold > max_value) {
        s_pd->system->error(
            "Threshold for horizontal dithering filter must be less than %d.",
            max_value
        );
        return;
    }
    /* TODO */
}

/** @brief Calculate the Bayer matrix dithering mask and write it to a given buffer. */
void pdImageUtils_GetBayerDither(
    uint8_t **buffer,
    uint8_t row,
    uint8_t rb,
    PDImageUtilsBayerType type,
    uint8_t threshold
) {
    /* Check parameter inconsistencies */
    if (buffer == NULL) {
        s_pd->system->error("Invalid reference to buffer (NULL) was passed.");
        return;
    }

    if (rb == 0) {
        s_pd->system->error("Invalid row count was passed.");
        return;
    }

    bool is_threshold_valid;
    switch (type) {
        case PDImageUtilsBayer2:
            is_threshold_valid = (threshold < 4);
            break;
        case PDImageUtilsBayer4:
            is_threshold_valid = (threshold < 16);
            break;
        case PDImageUtilsBayer8:
            is_threshold_valid = (threshold < 64);
            break;
        default:
            is_threshold_valid = false;
            break;
    }

    if (!is_threshold_valid) {
        s_pd->system->error(
            "Threshold of %d is not valid for bayer filter type %d.",
            threshold,
            type
        );
        return;
    }

    for (int r = 0; r < row; r++) {
        for (int c = 0; c < rb; c++) {
            for (int bc = 0; bc < 8; bc++) {
                switch (type) {
                    case PDImageUtilsBayer2:
                        buffer[r * rb + c] += (
                            (threshold > bayer2x2in8[r % 8][bc] ? 1 : 0)
                                << (8 - bc));
                        break;
                    case PDImageUtilsBayer4:
                        buffer[r * rb + c] += (
                            (threshold > bayer4x4in8[r % 8][bc] ? 1 : 0)
                                << (8 - bc));
                        break;
                    case PDImageUtilsBayer8:
                        buffer[r * rb + c] += (
                            (threshold > bayer8x8[r % 8][bc] ? 1 : 0)
                                << (8 - bc));
                        break;
                    default:
                        /* This should not occur at this point; this is to shut Clang-tidy up */
                        break;
                }
            }
        }
    }

}

/** @brief Finalizes the image manipulation module. */
void pdImageUtils_Finalize(void) {
    s_pd = NULL;
}
