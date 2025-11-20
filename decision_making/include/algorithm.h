#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <stdbool.h>
#include <stdlib.h>
#include "data_buffer.h"

#define MATRIX_SIZE 5

// relativity definitions for score struct
#define RELATIVITY_TOP 't'
#define RELATIVITY_BOTTOM 'b'
#define RELATIVITY_LEFT 'l'
#define RELATIVITY_RIGHT 'r'

typedef struct
{
    char relativity; // descrive to which tile the data is relative to, uses relativity definitions above
    bool data;       // 0 less score, -1 undefined, 2 higher score, 1 same score
} score;

typedef struct
{
    size_t *location; // row, column array position, has size 2
    score **scores;    // has size 4 for all x and y relations following r t l b as 0-3 indices
} tile;

// data has array of tile arrays which are the columns access done via matrix.data[col][row]
typedef struct
{
    tile ***data;
    int *label_rows; // values of each row
    float *label_cols; // values of each column
} matrix;

// determines the next tile to use
// if all tiles are known and no tiles with higher or equal score is found returns NULL
// if there are unknown tiles in the matrix neighbouring the current tile returns one of them in the order r, t, l, b
// if exists returns a pointer to the tile in the matrix
// if doesnt exist returns a new one but doesnt insert into the matrix since has -1 unknown score
tile *determine_next_tile(tile *curr_tile);

// returns 0 on success
// returns -1 if overwrite is false and a tile already exists in that location
int insert_tile_into_matrix(tile *curr_tile, bool overwrite);


// overwrites all matrix data with zero arrays
void initialize_matrix_data(size_t size);

// score array parameter is only used as a reference and not pointed to by the location field in the tile object
tile *new_tile(size_t loc0, size_t loc1, int *scores);

void free_matrix();

#endif