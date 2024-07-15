#include "pd_text.h"

#include <pd_api.h>
#include <string.h>
#include <math.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

typedef union PDContextLoaderTag {
    void *raw_ptr;
    PlaydateAPI *pd;
} PDContextLoader;

static PlaydateAPI *s_pd;

void pdText_Initialize(void *pd) {
    PDContextLoader loader = {pd};
    s_pd = loader.pd;
}

bool pdText_LoadFont(const char *font_path, uint8_t height_margin, Font *font, const char **err) {
    LCDFont *ft = s_pd->graphics->loadFont(font_path, err);
    if ((*err != NULL) && (strlen(*err) != 0)) {
        font->font = NULL;
        font->height = 0;
        return false;
    }

    font->font = ft;
    font->height = s_pd->graphics->getFontHeight(ft) + height_margin;
    return true;
}

uint32_t pdText_GetWrappedText(
    char **out_str,
    const Font *font,
    uint32_t max_lines,
    uint16_t max_width,
    PDStringEncoding encoding,
    const char *fmt,
    ...
) {
    if (max_lines == 0) {
        s_pd->system->error("PDText Error: Invalid number of lines has been passed.");
        return 0;
    }

    va_list v_list;
    va_start(v_list, fmt);
    char *out;
    size_t len = s_pd->system->vaFormatString(&out, fmt, v_list);

    /* If max_lines == 1, then there's no way we can wrap this text. */
    if (max_lines == 1) {
        s_pd->system->logToConsole(
            "PDText Warning: tried to generate wrapped text but only one line of text is allowed by parameter."
        );
        *out_str = out;
        return 1;
    }

    /* If there are no space characters, we cannot wrap this text. */
    if (strchr(out, ' ') == NULL) {
        *out_str = out;
        return 1;
    }

    /* Split by word (or space) */
    uint32_t space_count = 0;
    for (int i = 0; i < len; ++i) {
        space_count += out[i] == ' ' ? 1 : 0;
    }

    uint32_t *split_points = s_pd->system->realloc(NULL, sizeof(uint32_t) * space_count);
    uint32_t split_count = 0;
    for (int i = 0; i < len; ++i) {
        if (out[i] == ' ') {
            split_points[split_count] = i;
            split_count++;
        }
    }

    char *buf = s_pd->system->realloc(NULL, sizeof(char) * len);
    int line_count = 0;
    uint32_t str_offset = 0;
    int32_t split_point_index = 0;
    while (line_count < max_lines - 1) {
        while (split_point_index < space_count) {
            /* Expand this line until we go over the maximum width allowed */
            uint32_t split_point = split_points[split_point_index];
            memcpy(buf, out + str_offset, (split_point - str_offset) + 1);
            buf[(int) (split_point - str_offset)] = '\0';
            uint32_t text_width = s_pd->graphics->getTextWidth(
                font->font,
                buf,
                strlen(buf),
                encoding,
                s_pd->graphics->getTextTracking()
            );
            if (text_width > max_width) break;
            split_point_index++;
        }
        /* If the very first word is super long, perform a wrap at first space character. */
        uint32_t split_point = split_points[max(0, split_point_index - 1)];
        out[split_point] = '\n';
        /* The new string offset will be the split point + 1 */
        str_offset = split_point + 1;
        line_count++;
        split_point_index++;
    }

    *out_str = out;

    s_pd->system->realloc(buf, 0);
    s_pd->system->realloc(split_points, 0);

    return line_count + 1;
}

void pdText_DisplayString(PDStringEncoding encoding, int32_t x, int32_t y, const char *fmt, ...) {
    va_list v_list;
    va_start(v_list, fmt);
    char *out;
    size_t len = s_pd->system->vaFormatString(&out, fmt, v_list);
    va_end(v_list);
    s_pd->graphics->drawText(out, len, encoding, x, y);
    s_pd->system->realloc(out, 0);
}

void pdText_DisplayStringWithFont(Font *font, uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...) {
    if (font == NULL) {
        s_pd->system->error("Invalid font (NULL font passed).");
        return;
    }
    s_pd->graphics->setFont(font->font);
    va_list v_list;
    va_start(v_list, fmt);
    char *out;
    size_t len = s_pd->system->vaFormatString(&out, fmt, v_list);
    va_end(v_list);
    s_pd->graphics->drawText(out, len, (PDStringEncoding) encoding, x, y);
    s_pd->system->realloc(out, 0);
}

uint16_t pdText_GetStringWidth(const Font *font, const uint32_t encoding, const char *text) {
    return s_pd->graphics->getTextWidth(font->font, text, strlen(text), encoding, s_pd->graphics->getTextTracking());
}

void pdText_FreeFont(Font *font) {
    if (font == NULL) {
        return;
    }
    if (font->font == NULL) {
        return;
    }
    s_pd->system->realloc(font->font, 0);
    font->font = NULL;
    font->height = 0;
}

void pdText_Finalize(void) {
    s_pd = NULL;
}
