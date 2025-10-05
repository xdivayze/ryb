#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <stddef.h>


//FIFO N dimensional float buffer
typedef struct {
        size_t buffer_capacity; // capacity of the buffer, in number of arrays
    size_t array_len;       // length of each array in the buffer
    size_t head;            // index to write, in number of arrays
    size_t tail;            // index to pop, in number of arrays
    size_t count;           // buffer current length, it represents the nth array in the buffer not the actual float start in the buffer
    float *data;            // 2D float array spread into a 1D array where index of any element is found by (index * array_len + array_index)
} DataBuffer;

DataBuffer* databuffer_create(const size_t buffer_capacity, const size_t array_len) ;

//push data in a circular pattern move head and tail forward when capacity is full
// no method to check if array lengths are matching so non-matching arrays that are passed will result in unexpected behaavior
int databuffer_push(float *val, DataBuffer* databuffer ); 

//returns current length in terms of the nth array in the buffer not the nth float
size_t databuffer_get_current_length(const DataBuffer* databuffer); 

//remove element at tail and write to out_array array, -1 on empty buffer
int databuffer_pop(DataBuffer *databuffer, float* out_array); 

//flush buffer, returns 0 on success -1 on error, fast flush -resets indices-
int databuffer_flush(DataBuffer *databuffer); 

//destroy buffer, free memory, returns 0 on success -1 on error
int databuffer_destroy(DataBuffer *databuffer); 




#endif //DATA_BUFFER_H