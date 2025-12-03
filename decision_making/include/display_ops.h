#ifndef DISPLAY_OPS_H

#define DISPLAY_OPS_H

#include <stdint.h>
#include <display.h>
#include "algorithm.h"

#define HEARTBEAT_DISPLAY_TEXT "IN1"
#define CRYING_DISPLAY_TEXT "IN2"

#define OUT_FREQ_TEXT "OUT1"
#define OUT_AMPL_TEXT "OUT2"

extern const char* FONT_PATH;

typedef struct
{
    uint16_t y_inner_offset;
    uint16_t x_inner_offset;
    uint16_t x_outer_offset;
    uint16_t y_outer_offset;
    uint16_t line_width;
    FontxFile *fx;
} stylistics;

extern display_t display;

int move_cursor(matrix* alg_matrix, size_t row_old, size_t col_old, size_t row, size_t col);
int initialize_cursor(matrix* alg_matrix);

int display_string_on_display( char *val1, char *val2, char *val3, char *val4,
                              stylistics *styling);
int display_draw_cartesian( stylistics *styling);

int display_draw_matrix(matrix* alg_matrix, FontxFile* font_file);

int display_update_matrix_at_location(matrix* alg_matrix, size_t row, size_t col);


#endif