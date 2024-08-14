/**
 * @file pd_shorthand.h
 *
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
 * @def LCD_WIDTH
 * @brief Playdate screen width, per spec
 */
#define LCD_WIDTH  (LCD_COLUMNS)

/**
 * @def LCD_HEIGHT
 * @brief Playdate screen height, per spec
 */
#define LCD_HEIGHT (LCD_ROWS)

/**
 * @brief Use this union to perform alignment-safe type punning
 * between @c void* and @c PlaydateAPI*.
 *
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
 * @brief API that replicates @c malloc(3).
 *
 * @param[in] size Memory allocation size to request.
 * @returns Pointer to the allocated memory, or NULL if allocation fails.
 */
void *pd_Malloc(size_t size);

/**
 * @brief API that replicates @c realloc(3).
 *
 * @param[in] ptr  Pointer to the original memory location.
 * @param[in] size New requested size.
 * @returns Pointer to the re-allocated memory, or NULL if allocation fails.
 * @warning It is @b NOT guaranteed that this API returns the same pointer as @c ptr.
 *          If the new allocation is impossible at the given pointer,
 *          the system will assign a new location in RAM and return that as the pointer.
 *          If that fails, then this function will return NULL.
 *          Therefore, it is NOT advisable to directly assign the return value of this API,
 *          because if you do, your program may dereference pointers
 *          that shouldn't be dereferenced when the allocation fails.
 *          Always check for NULL before assigning the new pointer.
 *          Also, be advised that overuse of this API may lead to RAM fragmentation.
 *
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
 * @brief API that replicates @c free(3).
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
void pd_Log(const char *msg);

/**
 * @brief Shorthand for @c playdate->system->logToConsole.
 *
 * This API can format the log.
 *
 * @param[in] fmt Format string
 * @param[in] ... Format variables
 */
void pd_LogF(const char *fmt, ...);

/**
 * @brief Equivalent of @c playdate->system->error
 * but without an ability to format.
 *
 * @param[in] msg Log message
 * @warning Using this function @b will @b crash the game -
 *          on simulator, the execution is paused,
 *          and it will crash a real Playdate with e1 error
 *          with the error message (revealed by pressing B)
 *          being @c msg .
 * @sa pd_ErrorF
 */
void pd_Error(const char *msg);

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
void pd_ErrorF(const char *fmt, ...);

#endif /* PD_SHORTHAND_H */
