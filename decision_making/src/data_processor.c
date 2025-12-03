#include <data_buffer.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <libpynq.h>
#include "data_processor.h"
#include "algorithm.h"
#include <stdbool.h>
#include "display_ops.h"
#include "data_read.h"

static volatile int keep_running = 1;

int initial_stress;

void stop_processing(void)
{
    keep_running = 0;
}

static void int_as_string(uint8_t num, char *buf)
{
    sprintf(buf, "%d", num);
}

void *call_data_process_fromargs(void *args)
{
    data_process_args *dp_args = (data_process_args *)args;

    return (void *)data_process(dp_args->mutex_in_buffer, dp_args->mutex_out_buffer, dp_args->db_in, dp_args->db_out, dp_args->msec_sleep, dp_args->font_file);
}

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out,
                 size_t msec_sleep, FontxFile *font_file)
{
    uint8_t *val = malloc(sizeof(uint8_t) * db_in->array_len);     // 0: heartbeat 1: crying
    uint8_t *val_old = malloc(sizeof(uint8_t) * db_in->array_len); // 0: heartbeat 1: crying

    int last_stress;

    float out_arr[] = {0.0f, 0.0f};
    uint8_t vals_out[2] = {0, 0};
    int stress = 0xFF;

    while (1) // initial stress calculation is crucial
    {
        pthread_mutex_lock(mutex_in_buffer);

        if (db_in->count != 0)
        {
            databuffer_pop(db_in, val);
            pthread_mutex_unlock(mutex_in_buffer);
            stress = get_stress_level(val[0], val[1]);
            if (stress == -1)
            {
                fprintf(stderr, "initial stress calculation failed, trying again\n");
                continue;
            }
            else
                break;
        }

        pthread_mutex_unlock(mutex_in_buffer);
        sleep_msec(msec_sleep);
    }
    int relativity;

    initial_stress = stress;

    initialize_matrix_data(MATRIX_SIZE);

    int scores[] = {-1, -1, -1, -1};
    tile *initial_tile = new_tile(4, 4, scores, initial_stress); // initialize the max stress tile
    insert_tile_into_matrix(initial_tile, false);

    tile *curr_tile = initial_tile;
    tile *last_tile = initial_tile;

    matrix *alg_matrix = get_matrix();

    display_draw_matrix(alg_matrix, font_file);
    initialize_cursor(alg_matrix);

    int temp_stress = -1;

    while (keep_running) // algorithm loop
    {

        if (db_in->count == 0)
        {
            sleep_msec(msec_sleep);
            continue;
        }

        pthread_mutex_lock(mutex_in_buffer);
        databuffer_pop(db_in, val);
        pthread_mutex_unlock(mutex_in_buffer);

        temp_stress = get_stress_level(val[0], val[1]); // this is the case if crying provided value(count > 0), but stress > 50 so it is invalid; wait for heartbeat data
        if (temp_stress == -1)
        {
            continue;
        }

        last_stress = stress;
        stress = temp_stress;

        curr_tile->stress = stress;
        fprintf(stdout, "r:%li c:%li heartbeat:%i crying:%i stress: %i\n", curr_tile->location[0], curr_tile->location[1], val[0], val[1], stress);

        if (stress == last_stress && stress == 20)
        {
            fprintf(stdout, "baby calmed down\n");
            stop_processing();
            return 0;
        }

        if (stress < last_stress)
        {
            last_tile->scores[relativity]->data = 0;
            curr_tile->scores[relativity_order_opposites[relativity]]->data = 2;
        }
        else if (stress == last_stress)
        {
            last_tile->scores[relativity]->data = 1;
            curr_tile->scores[relativity_order_opposites[relativity]]->data = 1;
        }
        else if (stress > last_stress) // panic jump
        {
            last_tile->scores[relativity]->data = 2;
            curr_tile->scores[relativity_order_opposites[relativity]]->data = 0;
            fprintf(stdout, "panic jump occured\n");
            last_tile = curr_tile;
            curr_tile = initial_tile;
            stress = initial_stress;
            move_cursor(alg_matrix, last_tile->location[0], last_tile->location[1], curr_tile->location[0], curr_tile->location[1]);
        }

        last_tile = curr_tile;
        curr_tile = determine_next_tile(curr_tile, &relativity);

        // move the cursor to the next tile, update the old tile
        move_cursor(alg_matrix, last_tile->location[0], last_tile->location[1], curr_tile->location[0], curr_tile->location[1]);

        get_tile_output_values(curr_tile, out_arr);

        vals_out[0] = (uint8_t)(out_arr[0] * 100);
        vals_out[1] = (uint8_t)(out_arr[1]);

        pthread_mutex_lock(mutex_out_buffer);
        databuffer_push(vals_out, db_out);
        pthread_mutex_unlock(mutex_out_buffer);
        sleep_msec(msec_sleep);
    }

    free(val);
    return 0;
}
