#include <stdio.h>
#include <stdlib.h>
#include "data_buffer.h"
#include <string.h>
#include <stdint.h>

// inline func to switch between array indexing to buffer indexing
static inline size_t idx(const DataBuffer *databuffer, size_t buffer_index, size_t array_index)
{
    return buffer_index * databuffer->array_len + array_index;
}

uint8_t *databuffer_consume(DataBuffer *databuffer)
{
    // implement
    return 0;
}

int databuffer_push(uint8_t *val, DataBuffer *databuffer)
{
    size_t h = databuffer->head;

    // write at current head slot
    memcpy(&databuffer->data[idx(databuffer, h, 0)],
           val,
           databuffer->array_len * sizeof(uint8_t));

    // advance head to next slot
    h = (h + 1 == databuffer->buffer_capacity) ? 0 : h + 1;

    if (databuffer->count == databuffer->buffer_capacity) {
        // buffer full: drop the oldest element by advancing tail
        databuffer->tail = (databuffer->tail + 1 == databuffer->buffer_capacity)
                               ? 0
                               : databuffer->tail + 1;
    } else {
        databuffer->count++;
    }

    databuffer->head = h;

    return 0;
}

int databuffer_pop(DataBuffer *databuffer, uint8_t *out_array)
{
    if (databuffer->count == 0)
        return -1; // empty

    // slot of last pushed element (head points to next free slot)
    size_t slot = (databuffer->head == 0)
                      ? databuffer->buffer_capacity - 1
                      : databuffer->head - 1;

    // copy out the full array from that slot
    memcpy(out_array,
           &databuffer->data[idx(databuffer, slot, 0)],
           databuffer->array_len * sizeof(uint8_t));

    // move head back to this slot (now becomes next free position)
    databuffer->head = slot;
    databuffer->count--;

    // optional: keep tail sane when we become empty
    if (databuffer->count == 0)
        databuffer->tail = databuffer->head;

    return 0;
}

int databuffer_destroy(DataBuffer *databuffer)
{
    free(databuffer->data);
    databuffer->data = NULL;
    databuffer->buffer_capacity = databuffer->array_len = databuffer->head = databuffer->tail = databuffer->count = 0;
    return 0;
}

int databuffer_flush(DataBuffer *databuffer)
{
    if (databuffer == NULL)
        return -1;
    databuffer->head = databuffer->tail = databuffer->count = 0;
    return 0;
}

size_t databuffer_get_current_length(const DataBuffer *databuffer)
{
    if (databuffer == NULL)
        return -1;
    return databuffer->count;
}

DataBuffer *databuffer_create(const size_t buffer_capacity, const size_t array_len)
{
    DataBuffer *db = malloc(sizeof(DataBuffer));
    if (!db)
        return NULL;
    db->buffer_capacity = buffer_capacity;
    db->array_len = array_len;
    db->head = db->tail = db->count = 0;
    db->data = malloc(sizeof(uint8_t) * array_len * buffer_capacity);
    return db;
}