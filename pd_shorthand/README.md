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
        pd_Init(pd); /* Run this before anything */
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

### Font (struct)

This struct is designed to hold more information in addition to `LCDFont` struct.

#### LCDFont* font

This is the same as `LCDFont` found in Playdate API.

#### uint8_t height

This is the 'height' of the font, but it can include some margin pixels
to the original height of the font.

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
* [in] ptr Pointer to free.


> [!NOTE]
> 
> If you define `PD_SHORTHAND_DEBUG`, you can unlock an experimental feature
> where this library reports possible memory leaks upon finalization.


## Fonts

### pd_LoadFont

```c
bool pd_LoadFont(const char* font_path, uint8_t height_margin, Font* font, const char *err);
```

Shorthand for `PlaydateAPI::graphics::loadFont` plus some useful info.

This API loads a font from given `font_path`,
and also stores the height of the font for later use.

#### Parameters
* [in]  `font_path`     Path to the font.
* [in]  `height_margin` This value will be added to the actual font height.
                        Useful when you want to ensure the glyphs don't collide.
* [out] `font`          Specify the pointer to @c Font struct defined in this header.
                        This API will write information of loaded font there.
* [out] `err`           Should an error occurs, this string will be populated.


### pd_DisplayString

```c
void pd_DisplayString(uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...);
```

Shorthand for `PlaydateAPI::graphics::drawText`.

Because it takes 4 lines to display a formatted string on the screen,
and it could potentially create memory leak,  
this API takes all necessary parameters at once
and display the text at the given coordinates  
while also making sure that the memory is properly freed.

#### Parameters

* [in] `encoding` PDStringEncoding value.
* [in] `x`        X-axis position.
* [in] `y`        Y-axis position.
* [in] `fmt`      Format string.
* [in] `...`      Variadic arguments, used to format @c fmt .
                  Works identically to sprintf.

### pd_GetStringWidth
```c
uint16_t pd_GetStringWidth(const Font* font, uint32_t encoding, const char *text);
```
Calculates the string width of given text using the current tracking value.

There's a potential performance impact; avoid calling this API like every frame.

#### Parameters
* [in] `font`     `Font` object (as in this library's `Font`).
* [in] `encoding` `PDStringEncoding` value.
* [in] `text`     string to display.

> [!WARNING]
> 
> This API will use the current Text Tracking value.


### pd_FreeFont
```c
void pd_FreeFont(Font *font);
```

Frees the font loaded by `pd_LoadFont`.

#### Parameters
* [in] `font` `Font` struct (as in this library's `Font`) to free.
