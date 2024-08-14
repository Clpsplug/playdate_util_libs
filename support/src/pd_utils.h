/**
 * @brief The Playdate Utility Library
 *
 * This header includes all headers necessary
 * and a function performs required initialization
 * to use this library.
 *
 * If you find yourself using more than two modules from this library,
 * you should use this header instead for shortcut.
 *
 * @file pd_utils.h
 *
 */

#ifndef PD_UTILS_ALL_H
#define PD_UTILS_ALL_H

#include <pd_api.h>
#include <pd_shorthand.h>
#include <pd_text.h>
#include <pd_scene.h>

/**
 * @brief Initializes all the modules in this library.
 *
 * @param[in] playdate_context Playdate context object.
 */
inline void pdUtil_InitializeAll(void* playdate_context);

/**
 * @brief Finalizes all the modules in this library.
 *
 * @warning If you attempt to use this function to finalize the library
 *          when you have not used pdUtil_InitializeAll(void*) to initialize the library
 *          (i.e., some modules are uninitialized),
 *          the behavior of this function is undefined
 *          (most likely leads to a crash).
 */
inline void pdUtil_FinalizeAll(void);

#endif
