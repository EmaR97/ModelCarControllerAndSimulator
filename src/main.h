#ifndef SIMULATORE_PER_DEBUG_MAIN_H
#define SIMULATORE_PER_DEBUG_MAIN_H

#include "processing_functions.h"
#include "simulation_function.h"
#include "stdarg.h"
#include "math.h"
#include "stdio.h"
#include <stdint.h>

typedef struct {
    double x;
    double y;
    double d;
    double a;
} point;

typedef enum {
    right = 0, left = 1, forward = 2, backward = 3
} direction;

typedef enum {
    slow = 15, normal = 50
} speed;

typedef struct {
    double velocity;
    double init_time;
    double init_value;
} speed_info;

typedef struct {
    double x;
    double y;
    double facing_angle;
} car_position;

extern const int m[5], time_to_full_power, car_half_length, car_half_width, range_TOF, car_half_diagonal, space_needed_to_turn,debug_print;

/* il valore é cosi alto per fare la simulazione di base mi pare fosse 20 o 40 */
extern const uint16_t timingBudget;

extern car_position actual;


/* serial functions declaration */

/* MICRO ONLY START */
//int stringLen(const char *str);
//
//void sendMessage(char *outputMessage);
//
//void *sbrk(size_t incr);
/* MICRO ONLY END */

void trace(const char *format, ...);

/* MICRO ONLY START */

/* initialization function */
//void initializeTOF(void);
//
//void initialization_sequence(void);

/* MICRO ONLY END */
/* tof function */
int read_approximated_tof(int num_samples);

int read_tof(void);

/* calibrate velocity */
void calibrate_velocity(int init_time, int numSamples, int tipo);

__attribute__((unused)) __attribute__((unused)) void complete_calibrate();

/* control movement */
int rileva_ostacoli(int num_samples, int num_p);

point punto_rispetto_al_centro(double distanza_1, double angolo_1);

double corridoio_tra_due_punti(point ostacolo_A, point ostacolo_B);

int memorizza_ostacoli(double dis_ostacolo_n, double ang_ostacolo_n, int *classe_ostacolo_o, double *ang_ostacolo_o, double *ang_ostacolo_last, int *dis_ostacolo_last);

void move(int quantity, direction dir);

void evaluate_car_position(direction dir, uint32_t total_time, int print);

void reset_car_position(void);

 void movement_control(int num_samples);

/* motor function */
void motor_control(direction d, speed s);

void stop_motor(void);

direction opposite(direction dir);

int controlla_passaggio(int num_corridoi, int passaggio_da_controllare);

#endif //SIMULATORE_PER_DEBUG_MAIN_H
