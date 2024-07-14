#include "pd_shorthand.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pd_api.h>

#define UNUSED(arg) (void)(arg)

#if defined(PD_SHORTHAND_DEBUG)
typedef struct AllocInfoTag {
    void *ptr;
    size_t size;
} AllocInfo;

typedef struct AllocInfoArrayTag {
    AllocInfo **info;
    uint32_t count;
    uint32_t capacity;
} AllocInfoArray;

static AllocInfoArray allocInfo = {0};
#endif

static PlaydateAPI *s_pd;
static size_t s_total_allocation;

static void add_alloc_info(void *ptr, size_t size);

static size_t edit_alloc_info(void *ptr, void *newPtr, size_t size);

static size_t free_alloc_info(void *ptr);

#if defined(PD_SHORTHAND_DEBUG)
static void setup_alloc_info(void);

static void assert_memory_leak(void);

static int find_empty_entry_and_add(void *ptr, size_t size);
#endif

void pd_Initialize(void *ctx) {
    PDContextLoader ld = {ctx};
    s_pd = ld.pd;
}

void pd_Finalize(void) {
    s_pd = NULL;
}

void *pd_Malloc(size_t size) {
    void *ptr = s_pd->system->realloc(NULL, size);
    if (ptr != NULL) {
        s_total_allocation += size;
        add_alloc_info(ptr, size);
    }
    return ptr;
}

void *pd_Realloc(void *ptr, size_t size) {
    void *newPtr = s_pd->system->realloc(ptr, size);

    if (newPtr != NULL) {
        s_total_allocation += edit_alloc_info(ptr, newPtr, size);
    }

    return newPtr;
}

void pd_Free(void *ptr) {
    s_pd->system->realloc(ptr, 0);
    s_total_allocation -= free_alloc_info(ptr);
}

void pd_Log(const char *msg) {
    s_pd->system->logToConsole(msg);
}

void pd_Error(const char *msg) {
    s_pd->system->error(msg);
}

void pd_DisplayString(uint32_t encoding, int32_t x, int32_t y, const char *fmt, ...) {
    va_list v_list;
    va_start(v_list, fmt);
    char *out;
    size_t len = s_pd->system->vaFormatString(&out, fmt, v_list);
    va_end(v_list);
    s_pd->graphics->drawText(out, len, (PDStringEncoding) encoding, x, y);
    s_pd->system->realloc(out, 0);
}

uint16_t pd_GetStringWidth(const Font *font, const uint32_t encoding, const char *text) {
    return s_pd->graphics->getTextWidth(font->font, text, strlen(text), encoding, s_pd->graphics->getTextTracking());
}

bool pd_LoadFont(const char *font_path, uint8_t height_margin, Font *font, const char **err) {
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

void pd_FreeFont(Font *font) {
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

PlaydateAPI *pd_getPd(void) {
    return s_pd;
}

#if defined(PD_SHORTHAND_DEBUG)
// I really would like to believe this part works, but I don't have 100% certainity
static void assert_memory_leak(void) {
    if (total_allocation != 0) {
        pd->system->logToConsole(
                "[PD Shorthand Lib WARNING] Memory leak of %d bytes detected.",
                total_allocation
        );
        for (int i = 0; i < allocInfo.capacity; i++) {
            if (allocInfo.info[i]->ptr == NULL) continue;
            pd->system->logToConsole(
                    "[PD Shorthand Lib WARNING] Memory addr %p with %d bytes appears left allocated",
                    allocInfo.info[i]->ptr,
                    allocInfo.info[i]->size
            );
        }
    } else {
        pd->system->logToConsole("[PD Shorthand Lib INFO] Allocated memory cleanly freed!");
    }

    for (int i = 0; i < allocInfo.capacity; i++) {
        pd->system->realloc(allocInfo.info[i], 0);
    }
    pd->system->realloc(allocInfo.info, 0);
}

static void setup_alloc_info(void)
{
    total_allocation = 0;
    allocInfo.info = pd->system->realloc(NULL, sizeof(AllocInfo *) * 128);
    if (allocInfo.info == NULL) {
        pd->system->error("PD shorthand library startup failed");
    }
    for (int i = 0; i < 128; i++) {
        allocInfo.info[i] = pd->system->realloc(NULL, sizeof(AllocInfo));
    }
    allocInfo.count = 0;
    allocInfo.capacity = 128;
}

static int find_empty_entry_and_add(void *ptr, size_t size) {
    for (int i = 0; i < allocInfo.capacity; i++) {
        if (allocInfo.info[i]->ptr != NULL) continue;
        allocInfo.info[i]->ptr = ptr;
        allocInfo.info[i]->size = size;
        allocInfo.count++;
        return 1;
    }

    return 0;
}

static void add_alloc_info(void *ptr, size_t size) {
    if (find_empty_entry_and_add(ptr, size)) return;
    if (allocInfo.count == allocInfo.capacity) {
        allocInfo.capacity *= 2;
        void *newPtr = pd->system->realloc(allocInfo.info, sizeof(AllocInfo *) * allocInfo.capacity);
        if (newPtr == NULL) {
            pd->system->logToConsole(
                    "[PD Shorthand Lib WARNING] Failed to allocate memory for allocation debug info, info might be inaccurate");
            return;
        }
        for (int i = allocInfo.capacity / 2; i < allocInfo.capacity; i++) {
            allocInfo.info[i] = pd->system->realloc(NULL, sizeof(AllocInfo));
        }
    }
    find_empty_entry_and_add(ptr, size);
}

static ssize_t edit_alloc_info(void *ptr, void *newPtr, size_t size) {
    for (int i = 0; i < allocInfo.capacity; i++) {
        if (allocInfo.info[i]->ptr != ptr) continue;
        allocInfo.info[i]->ptr = newPtr;
        ssize_t prevSize = (ssize_t) allocInfo.info[i]->size;
        allocInfo.info[i]->size = size;
        return (ssize_t) size - prevSize;
    }
    return 0;
}

static size_t free_alloc_info(void *ptr) {
    for (int i = 0; i < allocInfo.capacity; i++) {
        if (allocInfo.info[i]->ptr != ptr) continue;
        allocInfo.info[i]->ptr = NULL;
        size_t size = allocInfo.info[i]->size;
        allocInfo.info[i]->size = 0;
        allocInfo.count--;
        return size;
    }
    return 0;
}
#else

static void add_alloc_info(void *ptr, size_t size) {
    UNUSED(ptr);
    UNUSED(size);
}

static size_t edit_alloc_info(void *ptr, void *newPtr, size_t size) {
    UNUSED(ptr);
    UNUSED(newPtr);
    UNUSED(size);
    return 0;
}

static size_t free_alloc_info(void *ptr) {
    UNUSED(ptr);
    return 0;
}

#endif