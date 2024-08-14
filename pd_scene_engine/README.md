# Playdate simple scene switcher engine

Manages the game state as 'scenes',
each scene representing a game screen that is presented to the player.

## Scene

A scene is defined with the `Scene` struct and consists of the following five elements:

* `sceneIdentifier` An integer that is unique throughout the whole game, used to refer to a scene
* `initFunction` A special function that receives Playdate API context object, which you should save during that
  function
* `updateFunction` A function that should be called every display update cycle
* `eventFunction` A function that handles Playdate system events *besides* kEventInit and kEventTerminate
* `unloadFunction` A function that handles unloading the scene (freeing stuff).

Only the identifier is required; not all function pointers must be filled.
If there is no reason to assign functions, assigning `NULL` will be enough.

## Defining a scene

Include `pd_scene.h` header file in the implementation (i.e., C file) to define a scene,

```c
#include <pd_scene.h>

static Scene scene_definition = {
  .sceneIdentifier = EXAMPLE_SCREEN, // Pretend this is a #define'd macro
  .initFunction = initFunc,
  .unloadFunction = unloadFunc,
  .updateFunction = updateFunc,
  .eventFunction = NULL,
};
```

and in your header file, expose a function like this:

```c
void* get_example_scene(void);
```

then implement the function like the following:

```c
void* get_example_scene(void) {
  return &scene_definition;
}
```

All other functions (such as `initFunc` and `updateFunc` in this example)
do *not* need to be exposed in the header file,
and thus can be static functions in the C file.

## Registering a scene

A scene must be registered to the system before it's usable.  
Call the definition-returning function defined like above
and supply it to `pdScene_Register(void*)` API.

```c
pdScene_Initialize(playdate_api_object); // Don't forget to init the system!
pdScene_Register(get_example_scene());
```

## Calling update & event handler

Your Playdate event handler and update callback MUST call the following functions:

```c
/* If the event isn't kEventInitialize or kEventTerminate: */
int pdScene_EventHandler(PDSystemEvent event, uint32_t arg);

/* In your update callback (you can directly return the value from this function) */
int pdScene_Update(void);
```

otherwise, the game will look like it froze.

## Loading a scene

To load (start) a scene, call pdScene_Load with the identifier you've specified
in the definition.  
If you supply a pointer to the second argument,
it will be available in the initFunction of the corresponding scene.  
If you don't need it, passing `NULL` will be enough.

```c
pdScene_Load(EXAMPLE_SCREEN, NULL);
```

If there is a scene loaded at this point, that scene will be unloaded,  
meaning that `unloadFunction` will be called.

## At the end of the game

When the user chooses to go back to the launcher, you should call the following function:

```c
pdScene_Finalize();
```

which will call the `unloadFunction` for that scene and free related resources.

## TL;DR

In summary, your `main.c` (the entrypoint code) would look something like this:

```c
#include <pd_api.h>
#include <stdint.h>

#include <pd_scene.h>
#include "scene_ids.h" /* Your Scene Identifier macro declarations */
#include "example/example.h" /* An example scene header, has `void* get_example_scene(void)` */

#define UNUSED(arg) (void)(arg)

static int update(void *pd);

/**
 * Playdate entrypoint
 */
int eventHandler(PlaydateAPI *pd, PDSystemEvent eventType, uint32_t arg) {
    switch (eventType) {
        case kEventInit:
            pdScene_Initialize(pd);
            pdScene_Register(get_example_scene());
            pdScene_Load(EXAMPLE_SCREEN, NULL);
            pd->system->setUpdateCallback(update, NULL);
            return 0;
        case kEventTerminate:
            pdScene_Finalize();
            return 0;
        default:
            return pdScene_EventHandler(eventType, arg);
    }
}

/**
 * Playdate update function
 */
static int update(void *pd) {
    UNUSED(pd);
    return pdScene_Update();
}
```
