#include "baby_mock.h"
#include "data_buffer.h"

#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>

static int test_matrix[5][5] = {

    {1, 5, 6, 9, 9},
    {2, 4, 5, 8, 9},
    {3, 4, 5, 7, 8},
    {7, 6, 6, 7, 8},
    {9, 9, 9, 9, 9}};

static const float FREQUENCIES_NON_FLOATING[5] = {20, 35, 50, 65, 70}; // Hz
static const float FREQUENCIES_FLOATING[5] = {0.2f, 0.35f, 0.5f, 0.65f, 0.7f}; // Hz
static const int AMPLITUDES[5] = {20, 40, 60, 80, 100};   //%, dimensionless

int crying_from_stress(int stress)
{
    if (stress < 0 || stress > 100)
        return -1;
    if (stress <= 10)
        return 0;
    if (stress >= 50)
        return 100;
    return (int)((10.0f / 4.0f) * (stress - 10));
}

int heartbeat_from_stress(int stress)
{
    if (stress < 0 || stress > 100)
        return -1;
    if (stress <= 10)
        return 60;
    return (2 * stress + 40);
}

void start_baby_loop(DataBuffer *db_in, pthread_mutex_t *mutex_in, DataBuffer *db_out, pthread_mutex_t *mutex_out, int initial_stress)
{
    struct timespec ts;
    ts.tv_nsec = BABY_REACTION_DELAY_MSEC * 1000 * 1000;
    ts.tv_sec = BABY_REACTION_DELAY_MSEC / 1000;

    uint8_t vals_in[2] = {70,100};  // freq, amplitude values
    uint8_t vals_out[2] = {0}; // heartbeat, crying values

    int stress = initial_stress;
    int last_stress = stress;
    int row = 0;
    int column = 0;

    int dstress = 0;

    vals_out[0] = 240;
    vals_out[1] = 100;

    pthread_mutex_lock(mutex_out);
    databuffer_push(vals_out, db_out);
    pthread_mutex_unlock(mutex_out);

    nanosleep(&ts, NULL);

    while (keep_baby_mock_running)
    {
        if (db_in->count != 0)
        {
            pthread_mutex_lock(mutex_in);
            databuffer_pop(db_in, vals_in);
            pthread_mutex_unlock(mutex_in);
        }

        column = -1;
        row = -1;

        for (int i = 0; i < 5; i++)
        {
            if (FREQUENCIES_NON_FLOATING[i] == vals_in[0])
            {
                column = i;
            }
            if (AMPLITUDES[i] == vals_in[1])
            {
                row = i;
            }
        }

        if ((column == -1) || (row == -1))
        {
            fprintf(stderr, "incompatible frequency or amplitude value\n");
        }

        last_stress = stress;
        stress = test_matrix[column][row] * 10 + 10;

        dstress = (last_stress - stress) / 10;

        if ((dstress == 0) && (stress == 20))
        {
            fprintf(stdout, "i am calmed down\n");
        }

        if (dstress < 0 || dstress > 1)
        { // panic jump if too low stress tile or higher stress tile
            vals_out[0] = 240;
            vals_out[1] = 100;
        }
        else
        {
            vals_out[0] = heartbeat_from_stress(stress);
            vals_out[1] = crying_from_stress(stress);
        }

        pthread_mutex_lock(mutex_out);
        databuffer_push(vals_out, db_out);
        pthread_mutex_unlock(mutex_out);

        nanosleep(&ts, NULL);
    }
}
