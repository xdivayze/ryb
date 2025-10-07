#ifndef DATA_WRITER_H
#define DATA_WRITER_H

#include <stdint.h>
#include <stdlib.h>
#include <data_buffer.h>
#include <pthread.h>

// PYNQ LIBRARY DATA WRITER FROM BUFFER USING PWM

// Duty encoded byte per frame
// Duty Cycle(%) * 255(0b11111111) = data
int pwm_multiplex_writer(size_t msec_signal_duration, uint32_t ticks_period, uint8_t sync_signal, DataBuffer *buffer,
                         pthread_mutex_t *mutex, int pwm_chan);

#endif