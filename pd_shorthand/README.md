# Playdate Shorthand Library

This library provides wrapper functions that may be confusing at first glance.

## Initialization

This library requires initialization.  
The initialization SHOULD be done as soon as your game starts.

You can optionally finalize the library on exit.

```c
#include <pd_shorthand.h>

int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
    switch (event){
    case kEventInit:
        pd_Initialize(pd); /* Run this before anything */
        break;
    case kEventTerminate:
        pd_Finalize();
    /* ... */
    }
}

```

## Macros

### LCD_WIDTH / LCD_HEIGHT

Same as `LCD_COLUMNS` and `LCD_HEIGHT` respectively,
they are there so that we can use terms that may be easier to understand.

## Types

### PDContextLoader (union)

This union helps you with type-punning between `void*` and `PlaydateAPI*`.

If other modules or functions passes what you know is `PlaydateAPI*` as `void*`,
use this union to perform a safe cast (type-punning) to `PlaydateAPI*`.

```c
void example(void* void_ptr) {
    /* You know void_ptr is actually PlaydateAPI* */
    PDContextLoader ld = { void_ptr };
    PlaydateAPI* pd = ld.pd;
}
```

## Memory management

In addition to `PlaydateAPI::system::realloc`, those functions are implemented:

### pd_Malloc

```c
void *pd_Malloc(size_t size);
```
API that replicates malloc(3).

#### Parameters
* [in] `size` Memory allocation size to request.

#### Returns
Pointer to the allocated memory, or `NULL` if allocation fails.


### pd_Realloc

```c
void *pd_Realloc(void* ptr, size_t size);
```

API that replicates realloc(3).

Same as `PlaydateAPI::system::realloc`, but exists for completeness's sake.

#### Parameters
* [in] `ptr`  Pointer to the original memory location.
* [in] `size` New requested size. 

#### Returns

Pointer to the re-allocated memory, or NULL if allocation fails.

> [!WARNING]
> 
> It is **NOT** guaranteed that this API returns the same pointer as `ptr`.
> If the new allocation is impossible at the given pointer,
> the system will assign a new location in RAM and return that as the pointer.
> If that fails, then this function will return `NULL`.  
> Therefore, it is NOT advisable to directly assign the return value of this API,
> because if you do, your program may dereference pointers
> that shouldn't be dereferenced when the allocation fails.  
> **Always** check for `NULL` before assigning the new pointer.  
> Also, be advised that overuse of this API may lead to [RAM fragmentation](https://en.wikipedia.org/wiki/Fragmentation_(computing)).
> 
> ```c
> #include <pd_shorthand.h>
> 
> void safe_realloc(void) {
>     void* ptr = pd_Malloc(sizeof(char) * 42);
>     void* newPtr = pd_Realloc(ptr, sizeof(char) * 84);
>     if (newPtr == NULL) {
>       pd_Log("Memory allocation failure");
>       return;
>     }
> }
> ```

#### Remarks
When expanding the allocation using this API, the content of the memory
**IS** guaranteed to be intact, even if the returned pointer is not equal to ptr.
Also, if the system moves your allocation, the previously-allocated RAM
will be automatically freed.


### pd_Free
```c
void pd_Free(void *ptr);
```

API that replicates free(3).

#### Parameters
* [in] `ptr` Pointer to free.


> [!NOTE]
> 
> If you define `PD_SHORTHAND_DEBUG`, you can unlock an experimental feature
> where this library reports possible memory leaks upon finalization.


