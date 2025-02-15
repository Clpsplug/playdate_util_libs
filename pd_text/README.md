# Playdate Text Helper Module

Helper functions for text-related operations.

## Types

### Font (struct)

This struct is designed to hold more information in addition to `LCDFont` struct.

#### LCDFont* font

This is the same as `LCDFont` found in Playdate API.

#### uint8_t height

This is the 'height' of the font, but it can include some margin pixels
to the original height of the font.

## Functions

### pdText_Initialize

```c
void pdText_Initialize(void* ctx);
```

Initializes the text module.

### pdText_LoadFont

```c
bool pdText_LoadFont(const char* font_path, uint8_t height_margin, Font* font, const char *err);
```

Shorthand for `PlaydateAPI::graphics::loadFont` plus some useful info.

This API loads a font from given `font_path`,
and also stores the height of the font for later use.

#### Parameters

* [in]  `font_path`     Path to the font.
* [in]  `height_margin` This value will be added to the actual font height.
  Useful when you want to ensure the glyphs don't collide.
* [out] `font`          Specify the pointer to `Font` struct defined in this header.
  This API will write information of loaded font there.
* [out] `err`           Should an error occurs, this string will be populated.

### Returns

`true`(1) on success, `false`(0) on failure (`true`/`false` as in `stdbool.h`)

### pdText_GetWrappedText

```c
uint32_t pdText_GetWrappedText(
  char **out_str,
  const Font *font,
  uint32_t max_lines,
  uint16_t max_width,
  PDStringEncoding encoding,
  const char *fmt,
...
);
```

Formats a string then writes the 'wrap-around' text into the buffer.

Since `PlaydateAPI::Graphics::drawText` draws text even if it's off the screen,
using this function will avoid unwanted cut-off texts.

This function will only split at spaces (` `, character `0x20`).

#### Parameters
* [out] `out_str`   Out buffer, can be supplied from outside. 
                    If NULL, this function will perform a memory allocation,
                    in which case the user is responsible for freeing it.
* [in]  `font       `Font` object.
* [in]  `max_lines` Number of times to wrap at most.  
                    If the input text is so large that it needs more wraps than this value,
                    the function will attempt to wrap the text up to this number of times
                    while the rest of the text will be left untouched.
* [in]  `max_width` Maximum allowed width of the text.
* [in]  `encoding`  PDStringEncoding value.
* [in]  `fmt`       String format (as in printf)
* [in]  `...`       Variadic arguments, used to format @c fmt .

#### Returns
Number of valid lines returned from this function. Is always less than or equal to `max_lines`.

> [!WARNING]
> 
> This function is potentially computationally costly.
> Use it wherever there's a lot of CPU time to spare,
> or a performance hit itself is acceptable (such as when loading).  
> This function allocates some heap memory internally.

### pdText_DisplayString

```c
void pdText_DisplayString(uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...);
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

### pdText_DisplayStringWithFont

```c
void pdText_DisplayString(Font* font, uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...);
```

Shorthand for `Playdate::graphics::setFont` and `PlaydateAPI::graphics::drawText`.

Same as `pdText_DisplayString`, but this will set the font before displaying the string.

#### Parameters

* [in] `font`     `Font` object (as in this library's `Font`).
* [in] `encoding` PDStringEncoding value.
* [in] `x`        X-axis position.
* [in] `y`        Y-axis position.
* [in] `fmt`      Format string.
* [in] `...`      Variadic arguments, used to format @c fmt .
  Works identically to sprintf.

> [!WARNING]
>
> If you need to display multiple strings in the same font,
> use this function for the first one
> and use `pdText_DisplayString` for the rest,
> which will save a function call.

### pdText_GetStringWidth

```c
uint16_t pdText_GetStringWidth(const Font* font, uint32_t encoding, const char *text);
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

### pdText_FreeFont

```c
void pdText_FreeFont(Font *font);
```

Frees the font loaded by `pdText_LoadFont`.

#### Parameters

* [in] `font` `Font` struct (as in this library's `Font`) to free.

### pdText_Finalize

```c
void pdText_Finalize(void);
```

Finalizes the text module.
