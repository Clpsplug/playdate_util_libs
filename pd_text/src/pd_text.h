#ifndef PD_TEXT_SHORTHAND_H
#define PD_TEXT_SHORTHAND_H

#include <pd_api.h>
#include <stdbool.h>

/**
 * @brief Font information.
 *
 * This struct provides additional info to LCDFont from Playdate API.
 */
typedef struct FontTag {
    /**
     * @brief The Playdate's LCDFont struct.
     *
     * Can be passed into @c playdate->graphics->setFont
     */
    LCDFont *font;

    /**
     * @brief Height of a given font.
     *
     * @remarks Depending on how this struct was populated,
     *          This might include some margin pixels
     *          to the original height of the font.
     */
    uint8_t height;
} Font;

/**
 * @brief Initializes the text module.
 *
 * @param[in] pd Playdate context object
 */
void pdText_Initialize(void *pd);

/**
 * @brief Shorthand for @c playdate->graphics->loadFont plus some useful info.
 *
 * This API loads a font from given @c font_path,
 * and also stores the height of the font for later use.
 *
 * @param[in]  font_path     Path to the font.
 * @param[in]  height_margin This value will be added to the actual font height.
 *                           Useful when you want to ensure the glyphs don't collide.
 * @param[out] font          Specify the pointer to @c Font struct defined in this header.
 *                           This API will write information of loaded font there.
 * @param[out] err           Should an error occurs, this string will be populated.
 * @returns true(1) on success, false(0) on failure (true/false as in stdbool.h)
 */
bool pdText_LoadFont(const char *font_path, uint8_t height_margin, Font *font, const char **err);

/**
 * @brief Formats a string then writes the 'wrap-around' text into the buffer.
 *
 * Since @c playdate->graphics->drawText draws text even if it's off the screen,
 * using this function will avoid unwanted cut-off texts.
 *
 * This function will only split at spaces (' ', character 0x20).
 *
 * @param[out] out_str   Out buffer, can be supplied from outside.
 *                       If NULL, this function will perform a memory allocation,
 *                       in which case the user is responsible for freeing it.
 * @param[in]  font      @c Font object.
 * @param[in]  max_lines Number of times to wrap at most.
 *                       If the input text is so large that it needs more wraps than this value,
 *                       the function will attempt to wrap the text up to this number of times
 *                       while the rest of the text will be left untouched.
 * @param[in]  max_width Maximum allowed width of the text.
 * @param[in]  encoding  PDStringEncoding value.
 * @param[in]  fmt       String format (as in printf)
 * @param[in]  ...       Variadic arguments, used to format @c fmt .
 * @returns Number of valid lines returned from this function.
 *          Is always less than or equal to @c max_lines .
 *
 * @warning This function is potentially computationally costly.
 *          Use it wherever there's a lot of CPU time to spare,
 *          or a performance hit itself is acceptable (such as when loading).
 *          This function allocates some heap memory internally.
 */
uint32_t pdText_GetWrappedText(
    char **out_str,
    const Font *font,
    uint32_t max_lines,
    uint16_t max_width,
    PDStringEncoding encoding,
    const char *fmt,
    ...
);

/**
 * @brief Shorthand for @c playdate->graphics->drawText .
 *
 * Because it takes 4 lines to display a formatted string on the screen
 * and it could potentially create memory leak,
 * this API takes all necessary parameters at once
 * and display the text at the given coordinates
 * while also making sure that the memory is properly freed.
 *
 * @param[in] encoding PDStringEncoding value.
 * @param[in] x        X-axis position.
 * @param[in] y        Y-axis position.
 * @param[in] fmt      Format string.
 * @param[in] ...      Variadic arguments, used to format @c fmt .
 *                     Works identically to sprintf.
 */
void pdText_DisplayString(PDStringEncoding encoding, int32_t x, int32_t y, const char *fmt, ...);

/**
 * @brief Shorthand for @c playdate->graphics->setFont and @c playdate->graphics->drawText .
 *
 * Same as @c pdText_DisplayString , but this will set the font before displaying the string.
 *
 * @param[in] font     @c Font object.
 * @param[in] encoding PDStringEncoding value.
 * @param[in] x        X-axis position.
 * @param[in] y        Y-axis position.
 * @param[in] fmt      Format string.
 * @param[in] ...      Variadic arguments, used to format @c fmt .
 *                     Works identically to sprintf.
 *
 * @warning If you need to display multiple strings in the same font,
 * use this function for the first one and use @c pdText_DisplayString for the rest,
 * which will save a function call.
 */
void pdText_DisplayStringWithFont(Font *font, uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...);

/**
 * @brief Calculates the string width of given text using the current tracking value.
 *
 * There's a potential performance impact;
 * avoid calling this API like every frame.
 *
 * @param[in] font     @c Font object.
 * @param[in] encoding @c PDStringEncoding value.
 * @param[in] text     string to display.
 *
 * @warning This API will use the current Text Tracking value.
 */
uint16_t pdText_GetStringWidth(const Font *font, uint32_t encoding, const char *text);

/**
 * @brief Frees the font loaded by @c pd_LoadFont.
 *
 * @param[in] font @c Font struct to free.
 */
void pdText_FreeFont(Font *font);

/**
 * @brief Finalizes the text module.
 */
void pdText_Finalize(void);

#endif
