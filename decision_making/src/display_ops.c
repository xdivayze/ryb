#include "display_ops.h"
#include <string.h>

static const int rw = DISPLAY_WIDTH / 5;
static const int rh = DISPLAY_HEIGHT / 5;

static uint16_t score_color_converter(int stress)
{
    if (stress == -1)
        return RGB_WHITE;
    if ((stress < 1) || (stress > max_stress))
    {
        return -1;
    }

    float alpha = (1.0f / (float)max_stress) * (9 - stress);
    uint8_t g = 0x3F;
    uint16_t r = 0x00;
    uint8_t b = 0x00;

    r = r * (1.0f - alpha) + 0x1F * alpha;
    g = g * (1.0f - alpha) + 0x3F * alpha;
    b = b * (1.0f - alpha) + 0x1F * alpha;

    return ((r << 11) | (g << 5) | b);
}

int display_draw_cartesian(stylistics *styling)
{
    displayDrawLine(&display, DISPLAY_WIDTH / 2, styling->y_outer_offset, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - (styling->y_outer_offset), RGB_BLACK);
    displayDrawLine(&display, styling->x_outer_offset, DISPLAY_HEIGHT / 2, DISPLAY_WIDTH - (styling->x_outer_offset), DISPLAY_HEIGHT / 2, RGB_BLACK);

    uint16_t canvas_width = DISPLAY_WIDTH - (styling->x_outer_offset * 2);

    displayDrawString(&display, styling->fx, styling->x_outer_offset, styling->y_outer_offset, HEARTBEAT_DISPLAY_TEXT, RGB_BLACK);
    displayDrawString(&display, styling->fx, styling->x_inner_offset + DISPLAY_WIDTH / 2, styling->y_outer_offset,
                      CRYING_DISPLAY_TEXT, RGB_BLACK);

    displayDrawString(&display, styling->fx, styling->x_outer_offset, DISPLAY_HEIGHT / 2 + styling->y_outer_offset,
                      OUT_AMPL_TEXT, RGB_BLACK);
    displayDrawString(&display, styling->fx, styling->x_inner_offset + DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + styling->y_outer_offset,
                      OUT_FREQ_TEXT, RGB_BLACK);

    return 0;
}

int display_update_matrix_at_location(matrix *alg_matrix, size_t row, size_t col)
{
    uint16_t color = RGB_GRAY;
    tile *curr_tile = alg_matrix->data[col][row];
    if (curr_tile)
    {

        uint16_t curr_color = score_color_converter(curr_tile->stress);
        if (curr_color == -1)
        {
            fprintf(stderr, "location row %li col %li has invalid score\n", row, col);
            curr_color = RGB_RED;
        }

        color = curr_color;
    }

    displayDrawFillRect(&display, col * rw, row * rh, ((col + 1) * rw) - 1, ((row + 1) * rh) - 1, color);
    displayDrawRect(&display, col * rw, row * rh, ((col + 1) * rw) - 1, ((row + 1) * rh) - 1, RGB_RED);
}

int display_draw_matrix(matrix *alg_matrix)
{

    for (int i = 0; i < MATRIX_SIZE; i++)
    { // col
        for (int j = 0; j < MATRIX_SIZE; j++)
        { // row
            display_update_matrix_at_location(alg_matrix, j, i);
        }
    }
}

int display_string_on_display(char *val1, char *val2, char *val3, char *val4,
                              stylistics *styling)
{
    displayDrawString(&display, styling->fx, styling->x_outer_offset + styling->x_inner_offset, styling->y_inner_offset + styling->y_outer_offset, val1, RGB_BLACK);
    displayDrawString(&display, styling->fx, styling->x_outer_offset + styling->x_inner_offset + DISPLAY_WIDTH / 2, styling->y_inner_offset + styling->y_outer_offset, val2, RGB_BLACK);
    displayDrawString(&display, styling->fx, styling->x_outer_offset + styling->x_inner_offset + DISPLAY_WIDTH / 2, styling->y_inner_offset + styling->y_outer_offset + DISPLAY_HEIGHT / 2, val3, RGB_BLACK);
    displayDrawString(&display, styling->fx, styling->x_outer_offset + styling->x_inner_offset, styling->y_inner_offset + styling->y_outer_offset + DISPLAY_HEIGHT / 2, val4, RGB_BLACK);

    return 0;
}