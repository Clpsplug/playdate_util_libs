/**
 * @brief Playdate shorthand library
 *
 * Wrapper APIs for common Playdate operations
 *        that may be confusing at first glance.
 *
 * @remarks If you define PD_SHORTHAND_DEBUG,
 *          this library will report memory leak when finalizing.
 *
 * @author  Clpsplug \<clpsplug\@clpsplug.com>
 * @license MIT
 */
#ifndef PD_SHORTHAND_H
#define PD_SHORTHAND_H

#include <stdlib.h>
#include <stdbool.h>
#include <pd_api.h>

/**
 * @brief Playdate screen width, per spec
 */
#define LCD_WIDTH  (LCD_COLUMNS)

/**
 * @brief Playdate screen height, per spec
 */
#define LCD_HEIGHT (LCD_ROWS)

/**
 * @brief Shorthand for @c playdate->system->logToConsole.
 *
 * This API can format the log.
 *
 * @param[in] fmt Format string
 * @param[in] ... Format variables
 */
#define pd_LogF(fmt, ...) pd_getPd()->system->logToConsole(fmt, __VA_ARGS__)

/**
 * @brief Shorthand for @c playdate->system->error.
 *
 * This API can format the error.
 *
 * @param[in] fmt Format string
 * @param[in] ... Format variables
 * @warning Using this function @b will @b crash the game -
 *          on simulator, the execution is paused,
 *          and it will crash a real Playdate with e1 error
 *          with the error message (revealed by pressing B)
 *          being the formatted message.
 */
#define pd_ErrorF(fmt, ...) pd_getPd()->system->error(fmt, __VA_ARGS__)

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
 * Use this union to perform alignment-safe type punning
 * between void* and PlaydateAPI*.
 *
 * @example
 * @code
 * void function(void* playdateApi) {
 *     PDContextLoader loader = { playdateApi };
 *     PlaydateAPI* pd = loader.pd;
 * }
 * @endcode
 */
typedef union PDContextLoaderTag {
    /**
     * @brief Raw void* pointer.
     *
     * You probably won't use this, but you can use this
     * to revert PlaydateAPI* to void*.
     */
    void *rawPtr;

    /**
     * @brief Use this member to get void* as PlaydateAPI*.
     *
     * Make sure that whatever you assign to rawPtr
     * to actually be PlaydateAPI*.
     */
    PlaydateAPI *pd;
} PDContextLoader;

/**
 * @brief Initializes the shorthand library.
 *
 * Preferably, this should be the #1 thing to be called on kEventInit.
 *
 * @param[in] pd PlaydateAPI* object.
 */
void pd_Initialize(void *pd);

/**
 * @brief Finalizes the PD shorthand library.
 */
void pd_Finalize(void);

/**
 * API that replicates malloc(3).
 *
 * @param[in] size Memory allocation size to request.
 * @returns Pointer to the allocated memory, or NULL if allocation fails.
 */
void *pd_Malloc(size_t size);

/**
 * API that replicates realloc(3).
 *
 * @param[in] ptr  Pointer to the original memory location.
 * @param[in] size New requested size.
 * @returns Pointer to the re-allocated memory, or NULL if allocation fails.
 * @warning It is NOT guaranteed that this API returns the same pointer as ptr.
 *          If the new allocation is impossible at the given pointer,
 *          the system will assign a new location in RAM and return that as the pointer.
 *          If that fails, then this function will return NULL.
 *          Therefore, it is NOT advisable to directly assign the return value of this API,
 *          because if you do, your program may dereference pointers
 *          that shouldn't be dereferenced when the allocation fails.
 *          Always check for NULL before assigning the new pointer.
 *          Also, be advised that overuse of this API may lead to RAM fragmentation.
 *
 * @example
 * @code
 * #include "pd_shorthand.h"
 *
 * void safe_realloc(void) {
 *     void* ptr = pd_Malloc(sizeof(char) * 42);
 *     void* newPtr = pd_Realloc(ptr, sizeof(char) * 84);
 *     if (newPtr == NULL) {
 *       pd_Log("Memory allocation failure");
 *       return;
 *     }
 *
 * }
 * @endcode
 *
 * @remarks When expanding the allocation using this API, the content of the memory
 *          IS guaranteed to be intact, even if the returned pointer is not equal to ptr.
 *          Also, if the system moves your allocation, the previously-allocated RAM
 *          will be automatically freed.
 */
void *pd_Realloc(void *ptr, size_t size);

/**
 * API that replicates free(3).
 *
 * @param[in] ptr Pointer to free.
 */
void pd_Free(void *ptr);

/**
 * @brief Equivalent of @c playdate->system->logToConsole
 * but without an ability to format.
 *
 * @param[in] msg Log message
 * @sa pd_LogF
 */
void pd_Log(const char* msg);

/**
 * @brief Equivalent of @c playdate->system->error
 * but without an ability to format.
 *
 * @paramn[in] msg Log message
 * @warning Using this function @b will @b crash the game -
 *          on simulator, the execution is paused,
 *          and it will crash a real Playdate with e1 error
 *          with the error message (revealed by pressing B)
 *          being @c msg .
 * @sa pd_ErrorF
 */
void pd_Error(const char* msg);

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
bool pd_LoadFont(const char *font_path, uint8_t height_margin, Font *font, const char **err);

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
void pd_DisplayString(uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...);

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
uint16_t pd_GetStringWidth(const Font* font, uint32_t encoding, const char *text);

/**
 * @brief Frees the font loaded by @c pd_LoadFont.
 *
 * @param[in] font @c Font struct to free.
 */
void pd_FreeFont(Font *font);


/**
 * @defgroup Private APIs
 *
 * These are not intended for regular use.
 */

/** @brief Get PlaydateAPI in this module.
 *
 * Not recommended for outside use, use PDContextLoader instead.
 *
 * @see PDContextLoader
 */
PlaydateAPI* pd_getPd(void);

#endif /* PD_SHORTHAND_H */
