/**
 * @file scene.h
 *
 * @brief Playdate simple scene switcher engine
 *
 * Manages the game state as 'scenes',
 * each scene representing a game screen that is presented to the player.
 *
 * @par Scene:
 * A scene is defined with the Scene struct and has the following five elements:
 * @li Scene::sceneIdentifier An integer that is unique throughout the whole game, used to refer to a scene
 * @li Scene::initFunction A special function that receives Playdate API context object, which you should save during that function
 * @li Scene::updateFunction A function that is called every display update cycle
 * @li Scene::eventFunction A function that handles Playdate system events besides kEventInit and kEventTerminate
 * @li Scene::unloadFunction A function that handles unloading the scene (freeing stuff).
 *
 * Only the identifier is required; not all function pointers must be filled.
 * If there is no reason to assign functions, just assign NULL.
 *
 * @par Defining a scene:
 * Include this header file in the implementation (i.e., C file) to define a scene,
 * @code
 * static Scene scene_definition = {
 *   .sceneIdentifier = EXAMPLE_SCREEN, // Pretend this is a #define'd macro
 *   .initFunction = initFunc,
 *   .unloadFunction = unloadFunc,
 *   .updateFunction = updateFunc,
 *   .eventFunction = NULL,
 * };
 * @endcode
 * and in your header file, expose a function like this:
 * @code
 * void* get_example_scene(void);
 * @endcode
 * then implement the function like the following:
 * @code
 * void* get_example_scene(void) {
 *   return &scene_definition;
 * }
 * @endcode
 * All other functions (such as @c initFunc and @c updateFunc in this example)
 * do not need to be exposed in the header file,
 * and thus can be static functions in the C file.
 *
 * @par Registering a scene:
 * A scene must be registered to the system before it's usable.
 * Simply call the definition-returning function defined like above
 * and supply it to pdScene_Register(void*) API.
 * @code
 * pdScene_Initialize(playdate_api_object); // Don't forget to init the system!
 * pdScene_Register(get_example_scene());
 * @endcode
 *
 * @par Loading a scene:
 * To load (start) a scene, call pdScene_Load(SceneIdentifier, const void*) with the identifier you've specified
 * in the definition.
 * If you supply a pointer to the second argument,
 * it will be available in the Scene::initFunction of the corresponding scene.
 * @code
 * pdScene_Load(EXAMPLE_SCREEN, NULL);
 * @endcode
 *
 *
 * @author  Clpsplug \<clpsplug\@clpsplug.com>
 * @license MIT
 */

#ifndef SCENE_ENGINE_H
#define SCENE_ENGINE_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Value for invalid scene ID.
 *
 * This should NOT be used by user - doing so will crash the game.
 */
#define PD_SCENE_INVALID_SCENE_ID UINT32_MAX

/**
 * @brief An unsigned integer to identify given scene.
 *
 * This value must be unique throughout the game.
 * Having scenes with the same identifier value leads to undefined behavior.
 *
 * @remarks UINT32_MAX cannot be used because it is reserved as an 'invalid scene' identifier.
 */
typedef uint32_t SceneIdentifier;

/**
 * Signature for the init function.
 *
 * @param[in] pd   Playdate API context object. Store this value within the implementation as soon as possible.
 * @param[in] data Data passed from other scenes. Can be null and can be ignored if it's irrelevant.
 */
typedef void(*SceneInitFunction)(void *pd, const void *data);

/**
 * @brief Signature for the unload function.
 *
 * The scene must make sure to free all the resources claimed during its lifetime.
 */
typedef void(*SceneUnloadFunction)(void);

/**
 * Signature for the update function.
 *
 * @returns 1 if the display needs to be updated, 0 if not.
 */
typedef int32_t(*SceneUpdateFunction)(void);

/**
 * @brief Signature for the event handler function.
 *
 * This is to be called whenever eventHandler fires,
 * except when the event is kEventInit and kEventTerminate.
 *
 * @param[in] eventType PDSystemEvent value, except for kEventInit and kEventTerminate.
 * @param[in] arg       The third argument from eventHandler function.
 * @returns 0 unless something goes awry.
 */
typedef int32_t(*SceneEventFunction)(uint32_t eventType, uint32_t arg);

/**
 * @brief Scene definition struct
 */
typedef struct SceneTag {
    /**
     * @brief An integer value to identify this scene.
     * @warning UINT32_MAX is reserved for 'invalid scene'; registering a scene with such will cause an e1 crash.
     * @remarks It is recommended that you #define the value.
     */
    const SceneIdentifier sceneIdentifier;
    /**
     * @brief Function to be called when the scene is initialized. Can be null.
     */
    const SceneInitFunction initFunction;
    /**
     * @brief Function to be called when the scene is unloaded. Can be null.
     */
    const SceneUnloadFunction unloadFunction;
    /**
     * @brief Function to be called when an update cycle is triggered. Can be null.
     */
    const SceneUpdateFunction updateFunction;
    /**
     * @brief Function to be called when an event handler triggers. Can be null.
     *
     * @attention kEventInit and kEventTerminate are not meant to be handled here.
     */
    const SceneEventFunction eventFunction;
} Scene;

/**
 * @brief Initialize scene switcher engine.
 *
 * Prior to calling other APIs defined here,
 * you must call this API,
 * preferably at the kEventInit stage.
 *
 * @param[in] pd Playdate API context object
 */
void pdScene_Initialize(void *pd);

/**
 * @brief Registers a scene.
 *
 * Registered scenes can be referenced using Scene::sceneIdentifier later.
 *
 * @param[in] scene Scene object
 */
void pdScene_Register(void *scene);

/**
 * @brief Registers multiple scenes at once.
 *
 * Registered scenes can be referenced using @c Scene::sceneIdentifier later.
 *
 * @param[in] scene Scene object pointer list
 * @param[in] count Item count of @c scene.
 * @warning If you get @c count wrong or you don't pass @c Scene* to @c scene, you will encounter an undefined behavior.
 */
void pdScene_RegisterBulk(void **scene, size_t count);

/**
 * @brief Load a scene while passing a data.
 *
 * The scene MUST be registered using pdScene_Register(void*) beforehand.
 * If there is a scene loaded and it has an unloading function assigned,
 * it will be called before loading up the next scene.
 *
 * @param[in] sceneIdentifier identifier assigned to Scene registered using pdScene_Register(void*).
 * @param[in] data            data to pass to the scene.
 * @warning If the scene with sceneIdentifier isn't registered, this will trigger an e1 crash.
 * @see pdScene_Register
 */
void pdScene_Load(SceneIdentifier sceneIdentifier, const void *data);

/**
 * @brief (Explicitly) unloads a scene.
 *
 * This API rarely has a need to be called
 * as it will cause a softlock unless you call pdScene_Load immediately after,
 * but if a memory consumption can be an issue,
 * this can be called to trigger the unloading function of the current scene.
 */
void pdScene_Unload(void);

/**
 * @brief Calls the update function of the scene.
 *
 * Call this API within a function that you specify using
 * @c playdate->system->setUpdateCallback.
 *
 * @remarks Playdate API context object will not be available here.
 *          To reference it, you must have assigned it to a static variable
 *          during the initialization function of the scene.
 */
int32_t pdScene_Update(void);

/**
 * @brief Calls the event handler function of the scene.
 *
 * Call this API from the default branch of the @c PDSystemEvent switch-case
 * @b without the @c PlaydateAPI* variable (which should have been assigned at the initialization function).
 *
 * @param[in] eventType @c PDSystemEvent value.
 * @param[in] arg       arg value (the third @c uint32_t argument of the @c eventHandler).
 */
int32_t pdScene_EventHandler(uint32_t eventType, uint32_t arg);

/**
 * @brief Finalizes the scene switcher engine.
 *
 * @warning After calling this API, all other APIs (except for pdScene_Initialize(void*))
 *          will be unavailable, and will cause undefined behaviors if they are called.
 */
void pdScene_Finalize(void);

#endif /* SCENE_ENGINE_H */
