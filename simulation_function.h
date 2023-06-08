//
// Created by emanu on 27/05/2023.
//

#ifndef SIMULATORE_PER_DEBUG_SIMULATION_FUNCTION_H
#define SIMULATORE_PER_DEBUG_SIMULATION_FUNCTION_H

#include <stdint.h>

typedef struct {
    double a;
    double b;
    double c;
    double x1;
    double x2;
    double y1;
    double y2;
} segmento;

void read_point_value();

segmento segmento_tra_due_punti(double x1, double y1, double x2, double y2);

double intersezione_tra_segmenti(segmento s1, segmento s2);

segmento segmento_dal_sensore(double x0, double y0, double a, double d);

void osalThreadDelayMilliseconds(int i);

uint32_t osalThreadGetMilliseconds();

void estremi_macchina();

int read_tof_simulation_part();

int movement_control_simulation_part();

#endif //SIMULATORE_PER_DEBUG_SIMULATION_FUNCTION_H
