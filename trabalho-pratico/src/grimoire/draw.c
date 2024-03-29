#include "grimoire/draw.h"
#include "grimoire/attr.h"
#include "grimoire/grimoire_priv.h"
#include "grimoire/terminal.h"

#include "common.h"
#include "util/error.h"
#include "util/string.h"

#ifdef GM_WIDECHAR
    #include <wchar.h>
    #define _gm_vsnprintf vswprintf
#else
    #define _gm_vsnprintf vsnprintf    
#endif

size_t gm_str_visible_len(const char* str) {
    size_t length = 0;

    for (size_t i = 0; str[i] != '\0';) {
        // Check for ANSI escape codes
        if (str[i] == '\x1B' && str[i + 1] == '[') {
            // Skip ANSI escape code
            while (str[i] != 'm') {
                i++;
            }

            i++; // Move past 'm'
        } else {
            // Handle UTF-8 multi-byte characters
            uint8_t byte = (uint8_t)str[i];
            if ((byte & 0xC0) != 0x80) {
                // This is the start of a character
                length++;
            }

            i++;
        }
    }

    return length;
}

int _gm_is_area_inbounds(GM_Term term, int row_start, int col_start, int row_end, int col_end) {
    if (row_start < 0 || row_start >= term->size.rows) return 0;
    if (col_start < 0 || col_start >= term->size.cols) return 0;
    if (row_end < 0 || row_end >= term->size.rows) return 0;
    if (col_end < 0 || col_end >= term->size.cols) return 0;

    if (row_start > row_end) return 0;
    if (col_start > col_end) return 0;

    return 1;
}

// ======= DRAWING FUNCTIONS =======
void gm_printf(GM_Term term, int row, int col, const char *format, ...) {
    #define SCOPE "gm_printf"
    rt_assert(
        _gm_is_area_inbounds(term, row, col, row, col),
        trace_msg(SCOPE, "Initial position is out of bounds.")
    );

    va_list args;
    va_start(args, format);

    // TODO: Trace?

    // int truepos = gm_term_buf_truepos(term, row, col);

    // Determine the length of the formatted string
    int length = _gm_vsnprintf(NULL, 0, format, args);
    va_end(args);

    // Row and col can no longer be calculated preemptively, due to multiline support.
    if (length <= 0) return;
    // if (row > term->size.rows) {
    //     return;
    // }
    // if (col + length > term->size.cols) {
    //     return;
    // }

    va_start(args, format);

    GM_Char* result = (GM_Char*)malloc(length + 1);
    if (result == NULL) {
        va_end(args);
        return;
    }

    // Format the string into the allocated memory
    _gm_vsnprintf(result, length + 1, format, args);
    va_end(args);
    
    // int vislen = gm_str_visible_len(result);

    Tokens lines = get_lines(result, length);

    int max_col = 0;
    
    // Break lines too big for current terminal.
    if (gm_has_attribute(term, GM_PRINT_OVERFLOW_BREAK)) {
        GM_TERM_SIZE size = gm_term_get_size(term);
        Tokens _lines = break_lines(lines, size.cols - col, NULL);
        destroy_tokens(lines);
        lines = _lines;

        int len = strlen(lines->data[0]);

        rt_assert(
            _gm_is_area_inbounds(term, row, col, row + lines->len - 1, col + len),
            trace_msg(SCOPE, "Area is out of bounds.")
        );

        max_col = len;

        for (int i = 1; i < lines->len; i++) {
            int _len = strlen(lines->data[i]);
            if (max_col < _len) max_col = _len; 

            rt_assert(
                _gm_is_area_inbounds(term, row, 0, row + lines->len - 1, col + _len),
                trace_msg(SCOPE, "Area is out of bounds.")
            );
        }
    } else {
        // int max_col = 0;
        for (int i = 0; i < lines->len; i++) {
            int len = strlen(lines->data[i]);
            if (max_col < len) max_col = len; 
        }

        rt_assert(
            _gm_is_area_inbounds(term, row, col, row + lines->len - 1, col + max_col),
            trace_msg(SCOPE, "Area is out of bounds.")
        );
    }
    
    GM_Attr attr = gm_attr_make(
        term, 
        GM_ATTR_TYPE_LINE, 
        row, 
        col, 
        row + lines->len,
        col + max_col, 
        &gm_attr_resolve_line,
        // strdup(result)
        lines
    );
    gm_term_attr_add(term, attr);

    free(result);
    #undef SCOPE
}

void gm_box(GM_Term term, int row_start, int col_start, int row_end, int col_end) {
    // #define SCOPE "gm_box"
    // rt_assert(
    //     _gm_is_area_inbounds(term, row_start, col_start, row_end, col_end),
    //     trace_msg(SCOPE, "Area is out of bounds.")
    // );

    // rt_assert(
    //     (row_end - row_start > 2 && col_end - col_start > 2),
    //     trace_msg(SCOPE, "Box is not big enough.")
    // );

    // // term->buf->data[row_start][col_start] = term->box_chars.tlc;
    // // term->buf->data[row_start][col_end] = term->box_chars.trc;
    // // term->buf->data[row_end][col_start] = term->box_chars.blc;
    // // term->buf->data[row_end][col_end] = term->box_chars.brc;
    // memcpy(term->buf->data[row_start] + col_start, term->box_chars.tlc, MAX_UTF8_SEQ_CORE);
    // memcpy(term->buf->data[row_start] + col_end, term->box_chars.trc, MAX_UTF8_SEQ_CORE);
    // memcpy(term->buf->data[row_end] + col_start, term->box_chars.blc, MAX_UTF8_SEQ_CORE);
    // memcpy(term->buf->data[row_end] + col_end, term->box_chars.brc, MAX_UTF8_SEQ_CORE);

    // // TODO: Fill vertical and horizontal lines
    // for (int i = col_start + 1; i < col_end; i++) {
    //     memcpy(term->buf->data[row_start] + i, term->box_chars.hl, MAX_UTF8_SEQ_CORE);
    //     memcpy(term->buf->data[row_end] + i, term->box_chars.hl, MAX_UTF8_SEQ_CORE);
    // }

    // for (int i = row_start + 1; i < row_end; i++) {
    //     memcpy(term->buf->data[i] + row_start, term->box_chars.vl, MAX_UTF8_SEQ_CORE);
    //     memcpy(term->buf->data[i] + row_end, term->box_chars.vl, MAX_UTF8_SEQ_CORE);
    // }

    // // TODO: Add line resolver
    // GM_Attr attr = gm_attr_make(
    //     term, 
    //     GM_ATTR_TYPE_BOX, 
    //     row_start, 
    //     col_start, 
    //     row_end,
    //     col_end, 
    //     &gm_attr_resolve_box
    // );
    // gm_term_attr_add(term, attr);
    
    // #undef SCOPE

    #define SCOPE "gm_box"
    rt_assert(
        _gm_is_area_inbounds(term, row_start, col_start, row_end, col_end),
        trace_msg(SCOPE, "Area is out of bounds.")
    );

    rt_assert(
        (row_end - row_start >= 2 && col_end - col_start > 2),
        trace_msg(SCOPE, "Box is not big enough.")
    );

    GM_Attr attr = gm_attr_make(
        term, 
        GM_ATTR_TYPE_BOX, 
        row_start, 
        col_start, 
        row_end,
        col_end, 
        &gm_attr_resolve_box,
        NULL
    );
    gm_term_attr_add(term, attr);

    #undef SCOPE
}
// ======= END DRAWING FUNCTIONS =======
