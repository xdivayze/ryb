#include <unity.h>
#include <algorithm.h>
#include <time.h>
#include <display_ops.h>
#include "utils.h"

#include <libpynq.h>

static int test_matrix[5][5] = {

    {1, 5, 6, 9, 9},
    {2, 4, 5, 8, 9},
    {3, 4, 5, 7, 8},
    {7, 6, 6, 7, 8},
    {9, 9, 9, 9, 9}};

static int check_tile(int row, int col, int nrow, int ncol)
{

    if (test_matrix[col][row] > test_matrix[ncol][nrow])
    {
        return 0; // panic jump
    }
    if (test_matrix[col][row] < test_matrix[ncol][nrow])
    {
        return 2; // less stress zone
    }
    if (test_matrix[col][row] == test_matrix[ncol][nrow])
    {
        return 1; // same stress
    }
}

int max_stress = 9;

void test_stress_calculation(void)
{
    int crying = 80;

    TEST_ASSERT_EQUAL_INT_MESSAGE(42, get_stress_level(2000, crying), "error calculating from crying");

    crying = 100;
    int heartbeat = 200;

    TEST_ASSERT_EQUAL_INT_MESSAGE(80, get_stress_level(heartbeat, crying), "error calculating from heartbeat");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, get_stress_level(0xFF, 0xFF), "stress calculation did not fail for OOB values");
}

void test_tile_detection(void)
{
    initialize_matrix_data(5);

    int scores[] = {-1, -1, -1, -1};
    tile *initial_tile = new_tile(4, 4, &(scores[0]), max_stress); // initialize the max stress tile
    insert_tile_into_matrix(initial_tile, false);

    tile *curr_tile = initial_tile;
    int last_stress = 9;
    int current_stress = 9;

    tile *last_tile;

    initial_stress = current_stress;

    int keep_running = 1;
    int relativity = -1;

    struct timespec ts;
    ts.tv_nsec = 50 * 1000 * 1000;
    ts.tv_sec = 50 / 1000;

    while (keep_running)
    {
        if (current_stress > last_stress)
        {
            printf("panic jump\n");
            curr_tile = initial_tile; // panic jump
        }

        last_tile = curr_tile;

        curr_tile = determine_next_tile(curr_tile, &relativity); // causes sigsegv by returning null if at 0,0 so no suitable conditions

        last_tile->scores[relativity]->data = check_tile(last_tile->location[0], last_tile->location[1], curr_tile->location[0], curr_tile->location[1]);

        curr_tile->scores[relativity_order_opposites[relativity]]->data = check_tile(curr_tile->location[0], curr_tile->location[1], last_tile->location[0], last_tile->location[1]);

        last_stress = current_stress;
        current_stress = test_matrix[curr_tile->location[1]][curr_tile->location[0]];

        printf("last stress: %i; current stress: %i;\n current tile location: r:%li c:%li; last tile location: r:%li c:%li;\n", last_stress, current_stress, curr_tile->location[0], curr_tile->location[1], last_tile->location[0], last_tile->location[1]);
        if ((current_stress == last_stress) && (last_stress == 1))
        {
            TEST_PASS();
            break;
        }
        nanosleep(&ts, NULL);
    }
}

void test_tile_detection_with_display(void)
{
    pynq_init();
    initialize_display();

    initialize_matrix_data(5);

    int scores[] = {-1, -1, -1, -1};
    tile *initial_tile = new_tile(4, 4, &(scores[0]), max_stress); // initialize the max stress tile
    insert_tile_into_matrix(initial_tile, false);

    tile *curr_tile = initial_tile;
    int last_stress = 9;
    int current_stress = 9;

    curr_tile->stress = current_stress;

    tile *last_tile;

    initial_stress = current_stress;

    int keep_running = 1;
    int relativity = -1;

    struct timespec ts;
    ts.tv_nsec = 200 * 1000 * 1000;
    ts.tv_sec = 200 / 1000;

    displayFillScreen(&display, RGB_WHITE);

    matrix *alg_matrix = get_matrix();

    display_draw_matrix(alg_matrix);
    initialize_cursor(alg_matrix);

    while (keep_running)
    {
        if (current_stress > last_stress)
        {
            printf("panic jump\n");
            last_tile = curr_tile;
            curr_tile = initial_tile; // panic jump
            move_cursor(alg_matrix, last_tile->location[0], last_tile->location[1], curr_tile->location[0], curr_tile->location[1]);
        }

        last_tile = curr_tile;

        curr_tile = determine_next_tile(curr_tile, &relativity); // causes sigsegv by returning null if at 0,0 so no suitable conditions

        last_tile->scores[relativity]->data = check_tile(last_tile->location[0], last_tile->location[1], curr_tile->location[0], curr_tile->location[1]);

        curr_tile->scores[relativity_order_opposites[relativity]]->data = check_tile(curr_tile->location[0], curr_tile->location[1], last_tile->location[0], last_tile->location[1]);

        last_stress = current_stress;
        current_stress = test_matrix[curr_tile->location[1]][curr_tile->location[0]];
        curr_tile->stress = current_stress;

        display_update_matrix_at_location(alg_matrix, curr_tile->location[0], curr_tile->location[1]);
        move_cursor(alg_matrix, last_tile->location[0], last_tile->location[1], curr_tile->location[0], curr_tile->location[1]);

        printf("last stress: %i; current stress: %i;\n current tile location: r:%li c:%li; last tile location: r:%li c:%li;\n", last_stress, current_stress, curr_tile->location[0], curr_tile->location[1], last_tile->location[0], last_tile->location[1]);

        if ((current_stress == last_stress) && (last_stress == 1))
        {
            // displayFillScreen(&display, RGB_WHITE);
            TEST_PASS();
            break;
        }

        nanosleep(&ts, NULL);
    }
    pynq_destroy();
}