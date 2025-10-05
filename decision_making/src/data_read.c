#include <stdio.h>
#include <stdlib.h>
#include "data_read.h"
#include <string.h>

int databuffer_push(float *val, DataBuffer *databuffer)
{
    if (databuffer->count == databuffer->buffer_capacity) // return if buffer is full
        return -1;

    if (sizeof(*val) / sizeof(float) != databuffer->array_len) // return if array sizes dont match
        return -2;

    size_t h = databuffer->head;

    memcpy(&databuffer->data[idx(databuffer, h, 0)], val, sizeof(*val));

    h++;
    if (h == databuffer->buffer_capacity)
        h = 0; // wrap head to 0 if buffer capacity to avoid getting sigsegv errors
    databuffer->head = h;

    databuffer->count++;
    return 0;
}

int databuffer_pop(DataBuffer *databuffer, float *out_array)
{
    if (databuffer->count == 0)
        return -1;

    for (int j = 0; j < databuffer->array_len; j++)
    {
        out_array[j] = databuffer->data[idx(databuffer, databuffer->tail, j)];
    }

    databuffer->tail = (databuffer->tail + 1) % databuffer->buffer_capacity;
    databuffer->count--;
    return 0;
}

static inline size_t idx(const DataBuffer *databuffer, size_t buffer_index, size_t array_index)
{
    return buffer_index * databuffer->array_len + array_index;
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
    db->data = malloc(sizeof(float) * array_len * buffer_capacity);
    return db;
}
