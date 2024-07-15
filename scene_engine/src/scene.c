#include "scene.h"

#include <pd_api.h>
#include <pd_shorthand.h>


/**
 * @brief Definition of an invalid scene.
 *
 * Reference this if there is no scene loaded.
 */
static Scene invalid_scene = {PD_SCENE_INVALID_SCENE_ID, NULL, NULL, NULL, NULL};

/**
 * @brief Scene registration struct
 *
 * Implemented as an appendable array
 */
typedef struct SceneRegistrationTag {
    Scene **scenes;
    int32_t count;
    int32_t capacity;
} SceneRegistration;

/**
 * @brief Type punning from void* to Scene*
 */
typedef union SceneLoaderTag {
    void *rawPtr;
    Scene *scene;
} SceneLoader;

static PlaydateAPI *s_pd;
static Scene *s_currentScene = &invalid_scene;
static SceneRegistration s_registrations = {0};

void pdScene_Initialize(void *pd) {
    s_pd = pd;
    s_registrations.scenes = pd_Malloc(sizeof(Scene *));
    s_registrations.count = 0;
    s_registrations.capacity = 1;
}

void pdScene_RegisterBulk(void **scenes, size_t count) {
    if (count <= 0) {
        s_pd->system->error("Invalid scene count passed.");
        return;
    }

    for (int i = 0; i < count; i++) {
        pdScene_Register(scenes[i]);
    }
}

void pdScene_Register(void *rawScene) {
    if (s_registrations.capacity == 0) {
        /* NOTE: This error message wouldn't be shown if the user forgets to initialize us, because s_pd is NULL */
        s_pd->system->error(
            "Registration capacity is set to 0 - did you initialize this library or accidentally finalize it?"
        );
        return;
    }

    SceneLoader loader = {rawScene};
    Scene *scene = loader.scene;
    if (scene->sceneIdentifier == PD_SCENE_INVALID_SCENE_ID) {
        s_pd->system->error("%d is reserved as invalid scene ID. Please don't use it.", PD_SCENE_INVALID_SCENE_ID);
        return;
    }
    if (s_registrations.count == s_registrations.capacity) {
        s_registrations.capacity *= 2;
        void *newPtr = pd_Realloc(s_registrations.scenes, sizeof(Scene *) * s_registrations.capacity);
        if (newPtr == NULL) {
            /* Honestly, if you trigger this, you REALLY need to consider how to manager RAM usage... */
            s_pd->system->error(
                "Allocation failure during scene registration... (tried to register ID %d, registration capacity %d)",
                scene->sceneIdentifier,
                s_registrations.capacity
            );
            return;
        }
        s_registrations.scenes = newPtr;
    }
    s_registrations.scenes[s_registrations.count] = scene;
    s_registrations.count++;
}


void pdScene_Load(const SceneIdentifier sceneIdentifier, const void *data) {
    pdScene_Unload();

    if (s_registrations.count == 0) {
        pd_Error("No scene in registration. Have you run pdScene_Register / pdScene_RegisterBulk?");
        return;
    }

    for (int i = 0; i < s_registrations.count; i++) {
        Scene *scene = s_registrations.scenes[i];
        if (sceneIdentifier == scene->sceneIdentifier) {
            s_currentScene = scene;
            if (s_currentScene->initFunction != NULL) {
                s_currentScene->initFunction(s_pd, data);
            }
            return;
        }
    }

    s_pd->system->error("Scene with identifier %d not found...", sceneIdentifier);
    s_currentScene = &invalid_scene;
}

void pdScene_Unload(void) {
    if (s_currentScene->unloadFunction != NULL) {
        s_currentScene->unloadFunction();
    }
    s_currentScene = &invalid_scene;
}

int32_t pdScene_Update(void) {
    if (s_currentScene->updateFunction == NULL) return 0;
    return s_currentScene->updateFunction();
}

int32_t pdScene_EventHandler(uint32_t eventType, uint32_t arg) {
    if (s_currentScene->eventFunction == NULL) return 0;
    return s_currentScene->eventFunction(eventType, arg);
}

void pdScene_Finalize(void) {
    pdScene_Unload();
    pd_Free(s_registrations.scenes);
    s_registrations.count = 0;
    s_registrations.capacity = 0;
}
