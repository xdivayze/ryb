#ifndef DATA_READ_H
#define DATA_READ_H

#include <stddef.h>

typedef struct DataBuffer DataBuffer;

DataBuffer* databuffer_create(const size_t buffer_capacity, const size_t array_len) ;

//pushes value array into the buffer, returns status code: 
// -1 for buffer full
int databuffer_push(float *val, DataBuffer* databuffer ); 

//returns current length in terms of the nth array in the buffer not the nth float
size_t databuffer_get_current_length(const DataBuffer* databuffer); 

//remove element at tail and write to out_array array, -1 on empty buffer
int databuffer_pop(DataBuffer *databuffer, float* out_array); 

//flush buffer, returns 0 on success -1 on error, fast flush -resets indices-
int databuffer_flush(DataBuffer *databuffer); 

//destroy buffer, free memory, returns 0 on success -1 on error
int databuffer_destroy(DataBuffer *databuffer); 

//inline func to switch between array indexing to buffer indexing
size_t idx(const DataBuffer *databuffer, size_t buffer_index, size_t array_index); 


#endif DATA_READ_H