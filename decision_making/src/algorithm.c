#include "algorithm.h"

#include <stdlib.h>
#include <string.h>

static const float FREQUENCIES[5] = {0.2, 0.35, 0.5, 0.65, 0.7}; // Hz
static const int AMPLITUDES[5] = {20, 40, 60, 80, 100};          //%, dimensionless

static char relativity_order[] = {RELATIVITY_RIGHT, RELATIVITY_TOP, RELATIVITY_BOTTOM, RELATIVITY_LEFT};

static matrix algorithm_matrix = {
    .data = {0, 0, 0, 0, 0}, // dummy data overwritten  by initialization of data
    .label_cols = (float *)&(FREQUENCIES[0]),
    .label_rows = (int *)&(AMPLITUDES[0])};

tile *determine_next_tile(tile *curr_tile)
{

    size_t arr_size = 4;

    int highest_score = 0; // if highest score is 0; -1, 1 or 2 take over. -1 is never written as long as highest_score is 0 -1 is updated at alt tile
    tile *alt_tile = NULL; // updated to -1 tile if highest score is 0. score 1 supercedes -1 which supercedes 0

    for (int i = 0; i < arr_size; i++)
    {
        int scores[] = {-1, -1, -1, -1};

        size_t score_index = 3 - i; // represents the relation of the next tile with the current tile
        switch (relativity_order[i])
        {
        case RELATIVITY_RIGHT:
        {
            if (curr_tile->location[0] == MATRIX_SIZE - 1)
            { // break if rightmost tile in the matrix
                break;
            }
            tile *right_tile = algorithm_matrix.data[curr_tile->location[0] + 1][curr_tile->location[1]];
            if (right_tile != 0)
            {
                int tile_data = right_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    return right_tile; // return first higher score tile
                }
                if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                {
                    alt_tile = right_tile;
                    highest_score = 1;
                    break;
                }
                if (tile_data == -1 && highest_score == 0 && !alt_tile)
                {
                    alt_tile = right_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0] + 1, curr_tile->location[1], scores);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (highest_score == 0 && !alt_tile) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    alt_tile = newly_generated_tile;
                }

                break;
            }
        }
        case RELATIVITY_TOP:
        {
            if (curr_tile->location[1] == 0)
            { // break if topmost tile in the matrix
                break;
            }
            tile *top_tile = algorithm_matrix.data[curr_tile->location[0]][curr_tile->location[1] - 1];
            if (top_tile != 0)
            {
                int tile_data = top_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    return top_tile; // return first higher score tile
                }
                if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                {
                    alt_tile = top_tile;
                    highest_score = 1;
                    break;
                }
                if (tile_data == -1 && highest_score == 0 && !alt_tile)
                {
                    alt_tile = top_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0], curr_tile->location[1] - 1, scores);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (highest_score == 0 && !alt_tile) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    alt_tile = newly_generated_tile;
                }

                break;
            }
        }
        case RELATIVITY_LEFT:
        {
            if (curr_tile->location[0] == 0)
            { // break if leftmost tile in the matrix
                break;
            }
            tile *left_tile = algorithm_matrix.data[curr_tile->location[0] - 1][curr_tile->location[1]];
            if (left_tile != 0)
            {
                int tile_data = left_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    return left_tile; // return first higher score tile
                }
                if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                {
                    alt_tile = left_tile;
                    highest_score = 1;
                    break;
                }
                if (tile_data == -1 && highest_score == 0 && !alt_tile)
                {
                    alt_tile = left_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0] - 1, curr_tile->location[1], scores);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (highest_score == 0 && !alt_tile) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    alt_tile = newly_generated_tile;
                }

                break;
            }
        }
        case RELATIVITY_BOTTOM:
        {
            if (curr_tile->location[1] == MATRIX_SIZE - 1)
            { // break if bottommost tile in the matrix
                break;
            }
            tile *bottom_tile = algorithm_matrix.data[curr_tile->location[0]][curr_tile->location[1] + 1];
            if (bottom_tile != 0)
            {
                int tile_data = bottom_tile->scores[score_index]->data;
                if (tile_data == 0)
                {
                    break; // case where the indexed tile has a less score
                }
                if (tile_data == 2)
                {
                    return bottom_tile; // return first higher score tile
                }
                if (tile_data == 1 && highest_score < 1) // if same score tile is found update the alt tile if only 0s or -1s are found so far
                {
                    alt_tile = bottom_tile;
                    highest_score = 1;
                    break;
                }
                if (tile_data == -1 && highest_score == 0 && !alt_tile)
                {
                    alt_tile = bottom_tile; // update alternative tile to the unknown tile if only lower score tiles are found and no alternative tile exists
                    break;
                }
            }
            else
            {
                // initialize new tile with -1 relative score to the left and update alt_tile do not touch the high_score
                tile *newly_generated_tile = new_tile(curr_tile->location[0], curr_tile->location[1] + 1, scores);
                insert_tile_into_matrix(newly_generated_tile, false);
                if (highest_score == 0 && !alt_tile) // update alternative tile to the unknown tile if no higher score or alternative tile exists
                {
                    alt_tile = newly_generated_tile;
                }

                break;
            }
        }
        default:
            break;
        }
    }
}

void get_tile_output_values(tile* curr_tile, float* out_arr){
    out_arr[0] = FREQUENCIES[ curr_tile->location[0]];
    out_arr[1] = FREQUENCIES[curr_tile->location[1]];
    return;
}

tile *new_tile(size_t loc0, size_t loc1, int *scores)
{
    size_t *loc_arr = malloc(sizeof(size_t) * 2);
    loc_arr[0] = loc0;
    loc_arr[1] = loc1;

    score **score_arr = malloc(sizeof(score *) * 4);

    for (int i = 0; i < 4; i++)
    {
        score *new_score_obj = malloc(sizeof(score));
        new_score_obj->data = scores[i];
        new_score_obj->relativity = relativity_order[i];
        score_arr[i] = new_score_obj;
    }

    tile *new_tile = malloc(sizeof(tile));
    new_tile->scores = score_arr;
    new_tile->location = loc_arr;

    return new_tile;
}

void initialize_matrix_data(size_t size)
{
    for (int i = 0; i < size; i++)
    {
        uint8_t *zero_arr = malloc(sizeof(uint8_t) * size);
        memset(zero_arr, 0, size);

        algorithm_matrix.data[i] = (tile **)zero_arr;
    }
}

int insert_tile_into_matrix(tile *curr_tile, bool overwrite)
{
    if (!overwrite && algorithm_matrix.data[curr_tile->location[0]][curr_tile->location[1]]) // default empty matrix values are 0 by definition
    {
        return -1;
    }

    algorithm_matrix.data[curr_tile->location[0]][curr_tile->location[1]] = curr_tile;

    return 0;
}

void free_matrix()
{
    // free tiles inside the data arrays
    // free data arrays
    // free tile location arrays
    // free scores
}
