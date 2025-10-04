#include <stdio.h>
#include <stdlib.h>
#include "data_read.h"

struct DataBuffer
{
    size_t buffer_capacity; // capacity of the buffer, in number of arrays
    size_t array_len;       // length of each array in the buffer
    size_t head;            // index to write
    size_t tail;            // index to read
    size_t count;           // buffer current length, it represents the nth array in the buffer not the actual float start in the buffer
    float *data;            // 2D float array spread into a 1D array where index of any element is found by (index * array_len + array_index)
};

int databuffer_push(DataBuffer *databuffer, float *val)
{
    //TODO implement push
    if (databuffer -> count == databuffer -> buffer_capacity) return -1;

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
