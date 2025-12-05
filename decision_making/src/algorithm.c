#include "algorithm.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
static const float FREQUENCIES_NON_FLOATING[5] = {20, 35, 50, 65, 70}; // Hz
static const float FREQUENCIES_FLOATING[5] = {0.2f, 0.35f, 0.5f, 0.65f, 0.7f}; // Hz

static const int AMPLITUDES[5] = {20, 40, 60, 80, 100};          //%, dimensionless

char relativity_order_opposites[] = {2, 3, 0, 1};
char relativity_order_actual[] = {RELATIVITY_RIGHT, RELATIVITY_TOP, RELATIVITY_LEFT, RELATIVITY_BOTTOM};

static matrix algorithm_matrix = {
    .data = {0, 0, 0, 0, 0}, // dummy data overwritten  by initialization of data
    .label_cols = (float *)&(FREQUENCIES_FLOATING[0]),
    .label_rows = (int *)&(AMPLITUDES[0])};

matrix *get_matrix()
{
    return &algorithm_matrix;
}

tile *determine_next_tile(tile *curr_tile, int *relativity) // relativity set by program
{

    if ((curr_tile->location[0] == curr_tile->location[1]) && (curr_tile->location[0] == 0))
    {
        return curr_tile;
    }

    size_t arr_size = 4;

    int highest_score = 0; // if highest score is 0; -1, 1 or 2 take over. -1 is never written as long as highest_score is 0 -1 is updated at alt tile
    tile *alt_tile = NULL; // updated to -1 tile if highest score is 0. score 1 supercedes -1 which supercedes 0

    for (int i = 0; i < arr_size; i++)
    {
        int scores[] = {-1, -1, -1, -1};
        int score_index = relativity_order_opposites[i];

        switch (relativity_order_actual[i])
        {
        case RELATIVITY_RIGHT:
        {
            if (curr_tile->location[1] == MATRIX_SIZE - 1)
            { // break if rightmost tile in the matrix
                break;
            }

            tile *right_tile = algorithm_matrix.data[curr_tile->location[1] + 1][curr_tile->location[0]];
            if (right_tile)
            {
                int tile_data = right_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    *relativity = 0;
                    return right_tile; // return first higher score tile
                }
                if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                {
                    *relativity = 0;
                    alt_tile = right_tile;
                    highest_score = 1;
                    break;
                }
                if (tile_data == -1 && (!alt_tile || highest_score == 1))
                {
                    *relativity = 0;
                    alt_tile = right_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0], curr_tile->location[1] + 1, scores, -1);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (!alt_tile || highest_score == 1) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    *relativity = 0;
                    alt_tile = newly_generated_tile;
                }

                break;
            }
            break;
        }
        case RELATIVITY_TOP:
        {
            if (curr_tile->location[0] == 0)
            { // break if topmost tile in the matrix
                break;
            }
            tile *top_tile = algorithm_matrix.data[curr_tile->location[1]][curr_tile->location[0] - 1];
            if (top_tile)
            {
                int tile_data = top_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    *relativity = 1;
                    return top_tile; // return first higher score tile
                }
                if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                { 
                    *relativity = 1;
                    alt_tile = top_tile;
                    highest_score = 1;
                    break;
                }
                if (tile_data == -1 && (!alt_tile || highest_score == 1))
                {
                    *relativity = 1;
                    alt_tile = top_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
                break;
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0] - 1, curr_tile->location[1], scores, -1);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (!alt_tile || highest_score == 1) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    *relativity = 1;

                    alt_tile = newly_generated_tile;
                }

                break;
            }
        }
        case RELATIVITY_LEFT:
        {
            if (curr_tile->location[1] == 0)
            { // break if leftmost tile in the matrix
                break;
            }
            tile *left_tile = algorithm_matrix.data[curr_tile->location[1] - 1][curr_tile->location[0]];
            if (left_tile)
            {
                int tile_data = left_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    *relativity = 2;

                    return left_tile; // return first higher score tile
                }
                if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                {
                    *relativity = 2;

                    alt_tile = left_tile;
                    highest_score = 1;
                    break;
                }
                if (tile_data == -1 && (!alt_tile || highest_score == 1))
                {
                    *relativity = 2;

                    alt_tile = left_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0], curr_tile->location[1] - 1, scores, -1);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (!alt_tile || highest_score == 1) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    *relativity = 2;

                    alt_tile = newly_generated_tile;
                }

                break;
            }
            break;
        }
        case RELATIVITY_BOTTOM:
        {
            if (curr_tile->location[0] == MATRIX_SIZE - 1)
            { // break if bottommost tile in the matrix
                break;
            }
            tile *bottom_tile = algorithm_matrix.data[curr_tile->location[1]][curr_tile->location[0] + 1];
            if (bottom_tile)
            {
                int tile_data = bottom_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    *relativity = 3;

                    return bottom_tile; // return first higher score tile
                }
                // if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                // { //! this condition is causing it to favor bottom tile if bottom is a 1 and only -1s are found. the last 1 index isnt able to assign a -1 to the tile...
                //     *relativity = 3;

                //     alt_tile = bottom_tile;
                //     highest_score = 1;
                //     break;
                // }
                if (tile_data == -1 && (!alt_tile || highest_score == 1))
                {
                    *relativity = 3;

                    alt_tile = bottom_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0] + 1, curr_tile->location[1], scores, -1);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (!alt_tile || highest_score == 1) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    *relativity = 3;

                    alt_tile = newly_generated_tile;
                }

                break;
            }
            break;
        }
        default:
            break;
        }
    }

    return alt_tile;
}

void get_tile_output_values(tile *curr_tile, float *out_arr)
{
    out_arr[0] = FREQUENCIES_FLOATING[curr_tile->location[1]];
    out_arr[1] = AMPLITUDES[curr_tile->location[0]];
    return;
}
// loc0 row; loc1 col
tile *new_tile(size_t loc0, size_t loc1, int *scores, int stress)
{
    size_t *loc_arr = malloc(sizeof(size_t) * 2);
    loc_arr[0] = loc0;
    loc_arr[1] = loc1;

    score **score_arr = calloc(4, sizeof(score *));

    for (int i = 0; i < 4; i++)
    {
        score *new_score_obj = malloc(sizeof(score));
        new_score_obj->data = scores[i];
        new_score_obj->relativity = relativity_order_actual[i];
        score_arr[i] = new_score_obj;
    }

    tile *new_tile = malloc(sizeof(tile));
    new_tile->scores = score_arr;
    new_tile->location = loc_arr;
    new_tile->stress = stress;

    return new_tile;
}

void initialize_matrix_data(size_t size)
{
    algorithm_matrix.data = malloc(size * sizeof(tile **));

    for (int i = 0; i < size; i++)
    {
        algorithm_matrix.data[i] = calloc(size, sizeof(tile *));
    }
}

int insert_tile_into_matrix(tile *curr_tile, bool overwrite)
{
    if (!overwrite && algorithm_matrix.data[curr_tile->location[1]][curr_tile->location[0]]) // default empty matrix values are 0 by definition
    {
        return -1;
    }

    algorithm_matrix.data[curr_tile->location[1]][curr_tile->location[0]] = curr_tile;

    return 0;
}

void free_matrix()
{
    // free tiles inside the data arrays
    // free data arrays
    // free tile location arrays
    // free scores
}

// checks for only crying data if provided valid range.
// -1 if invalid range
int get_stress_level(int heartbeat, int crying)
{
    if (crying < 100 && crying >= 0)
    { // crying data only valid in interval (100,0] where stres is [10,50)
        float m = (50.0f - 10.0f) / 100.0f;
        float c = 10.0f;
        return (int)(m * crying) + c; // linear interpolation
    }

    if (heartbeat < 60 || heartbeat > 240)
        return -1; //-1 if no data is in valid range

    float m = (100.0f - 10.0f) / (240.0f - 60.0f);
    float c = -20.0f;

    return (int)((m * heartbeat) + c); // linear interpolation
}
