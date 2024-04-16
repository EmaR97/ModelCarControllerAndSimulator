//
// Created by emanu on 27/05/2023.
//

#ifndef SIMULATORE_PER_DEBUG_MICRO_FUNCTIONS_H
#define SIMULATORE_PER_DEBUG_MICRO_FUNCTIONS_H

#include "main.h"

void motor_control_micro_part(direction *d, speed *s);

void stop_motor_micro_part();

void initialization_sequence(void);

int read_tof_micro_part();

void sendMessage(char *outputMessage);

#endif //SIMULATORE_PER_DEBUG_MICRO_FUNCTIONS_H
