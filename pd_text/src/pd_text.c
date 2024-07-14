#include "pd_text.h"

#include <pd_api.h>

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

void pdText_DisplayString(uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...) {
    va_list v_list;
    va_start(v_list, fmt);
    char *out;
    size_t len = s_pd->system->vaFormatString(&out, fmt, v_list);
    va_end(v_list);
    s_pd->graphics->drawText(out, len, (PDStringEncoding) encoding, x, y);
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
