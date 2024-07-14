#include "pd_text.h"

#include <pd_api.h>
#include <string.h>

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

uint32_t pdText_GetWrappedText(const Font *font, const char **out_str, uint32_t max_lines, uint16_t max_width,
                               PDStringEncoding encoding, const char *fmt, ...) {
    if (out_str == NULL) {
        s_pd->system->error("Invalid pointer to buffer has been passed.");
        return 0;
    }

    if (max_lines == 0) {
        s_pd->system->error("Invalid number of lines has been passed.");
        return 0;
    }

    va_list v_list;
    va_start(v_list, fmt);
    char *out;
    size_t len = s_pd->system->vaFormatString(&out, fmt, v_list);

    /* If max_lines == 1, then there's no way we can wrap this text. */
    if (max_lines == 1) {
        s_pd->system->logToConsole(
                "Warning: tried to generate wrapped text but only one line of text is allowed by parameter.");
        out_str[0] = out;
        return 1;
    }

    /* If there are no space characters, we cannot wrap this text. */
    if (strchr(out, ' ') == NULL) {
        out_str[0] = out;
        return 1;
    }

    /* Split by word (or space) */
    uint32_t space_count = 0;
    for (int i = 0; i < len; ++i) {
        space_count += out[i] == ' ' ? 1 : 0;
    }

    uint32_t *split_points = s_pd->system->realloc(NULL, sizeof(uint32_t) * space_count);
    uint32_t count = 0;
    for (int i = 0; i < len; ++i) {
        if (out[i] == ' ') {
            split_points[count] = i;
            count++;
        }
    }

    char *buf = s_pd->system->realloc(NULL, sizeof(char) * len);
    int line_index = 0;
    uint32_t str_offset = 0;
    uint32_t split_point_index = 0;
    while (line_index < max_lines - 1) {
        uint32_t text_width = 0;
        while (split_point_index < space_count) {
            memcpy(buf, out + str_offset, split_points[split_point_index] - str_offset);
            buf[(int) (split_points[split_point_index] - str_offset)] = '\0';
            text_width = s_pd->graphics->getTextWidth(
                    font->font,
                    buf,
                    split_points[split_point_index] - str_offset,
                    encoding,
                    s_pd->graphics->getTextTracking()
            );
            split_point_index++;
            if (text_width > max_width) break;
        }

        if (text_width == 0) break;

        /* User must free this memory */
        char *out_buf = s_pd->system->realloc(NULL, sizeof(char) * (strlen(buf) + 1));
        strcpy(buf, out_buf);
        out_str[line_index] = out_buf;
        str_offset += strlen(buf);
        line_index++;
    }

    /* If at this point there's any text left to be wrapped, shove all of them at the last element */
    if (str_offset != strlen(out))
    {
        char *out_buf = s_pd->system->realloc(NULL, sizeof (char) * (strlen(out) - str_offset));
        strcpy(out + str_offset, out_buf);
        out_str[max_lines - 1] = out_buf;
    }

    s_pd->system->realloc(buf, 0);
    s_pd->system->realloc(split_points, 0);

    return line_index;
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
