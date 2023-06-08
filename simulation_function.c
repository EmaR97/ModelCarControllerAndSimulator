#include "main.h"
#include "math.h"
#include "stdio.h"
#include "simulation_function.h"
#include "processing_functions.h"

/** ostacoli sotto forma di segmenti */
segmento segmenti[40];

/** segmenti che rappresentano la macchinina, usate per rilevare le collisioni */
segmento seg_car[4];

int num_seg = 0;

long simulation_time = 0;

/** posizione estremi della macchina ipotizzando sia un rettangolo */
double car_extreme_angle = 0;

int n_contact = 0;

int skip_cycle = 1;

/**
 * @brief funzione usata per inserire gli ostacoli sotto forma di segmenti indicandone le coordinate x e y degli estremi
 */
void read_point_value() {

    int A_x, A_y, B_x, B_y;
    while (1) {
        printf("Inserisci valori ostacolo_%d\n", num_seg);
        printf("A_x,A_y,B_x,B_y=");
        scanf("%d,%d,%d,%d", &A_x, &A_y, &B_x, &B_y);
        if ((A_x == 0 && A_y == 0 && B_x == 0 && B_y == 0) || num_seg >= 40) { break; }
        segmenti[num_seg++] = segmento_tra_due_punti(A_x, A_y, B_x, B_y);
    }
}

/**
 * @brief
 * @param s1
 * @param s2
 * @return
 */
double intersezione_tra_segmenti(segmento s1, segmento s2) {

    double x = (s2.c * s1.b - s1.c * s2.b) / (s1.a * s2.b - s2.a * s1.b);
    double x1min, x1max, x2min, x2max;
    x1max = s1.x1 > s1.x2 ? s1.x1 : s1.x2;
    x1min = s1.x1 > s1.x2 ? s1.x2 : s1.x1;
    x2max = s2.x1 > s2.x2 ? s2.x1 : s2.x2;
    x2min = s2.x1 > s2.x2 ? s2.x2 : s2.x1;
    if (x > x1min && x < x1max && x > x2min && x < x2max) {
        return x;
    }
    else { return 0; }
}

/**
 * @brief
 * @param x0
 * @param y0
 * @param a
 * @param d
 * @return
 */
segmento segmento_dal_sensore(double x0, double y0, double a, double d) {

    return segmento_tra_due_punti(x0 + cos(deg_to_rad(a)) * car_half_length, y0 + sin(deg_to_rad(a)) * car_half_length, x0 + cos(deg_to_rad(a)) * d, y0 + sin(deg_to_rad(a)) * d);
}

/**
 * @brief simula in modo grossolano lo scorrere del tempo nella simulazione
 */
uint32_t osalThreadGetMilliseconds() {

    return ++simulation_time;
}

void osalThreadDelayMilliseconds(int i) {

    simulation_time += i;
}

/**
 * @brief controllo se, in base alla posizione attuale, i segmenti che rappresentano gli estremi della macchina
 * sono sovrapposti a quelli che rappresentano gli ostacoli. In caso di sovrapposizione è avvenuta una collisione
 */
void estremi_macchina() {

    double cos_angle = cos(deg_to_rad(actual.facing_angle + car_extreme_angle)), sin_angle = sin(deg_to_rad(actual.facing_angle + car_extreme_angle));
    double cos_d = cos_angle * car_half_diagonal, sin_d = sin_angle * car_half_diagonal;
    double anteriore_sinistra_x = actual.x + cos_d;
    double anteriore_sinistra_y = actual.y + sin_d;
    double anteriore_destra_x = actual.x + cos_d;
    double anteriore_destra_y = actual.y - sin_d;
    double posteriore_sinistra_x = actual.x - cos_d;
    double posteriore_sinistra_y = actual.y + sin_d;
    double posteriore_destra_x = actual.x - cos_d;
    double posteriore_destra_y = actual.y - sin_d;
    seg_car[0] = segmento_tra_due_punti(anteriore_destra_x, anteriore_destra_y, posteriore_destra_x, posteriore_destra_y);
    seg_car[1] = segmento_tra_due_punti(anteriore_sinistra_x, anteriore_sinistra_y, posteriore_sinistra_x, posteriore_sinistra_y);
    seg_car[2] = segmento_tra_due_punti(anteriore_destra_x, anteriore_destra_y, anteriore_destra_x, anteriore_destra_y);
    seg_car[3] = segmento_tra_due_punti(posteriore_sinistra_x, posteriore_sinistra_y, posteriore_destra_x, posteriore_destra_y);
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < num_seg; i++) {
            double x = intersezione_tra_segmenti(segmenti[i], seg_car[j]);
            if (x != 0) {
                printf("ostacolo x1:%f,x2:%f,y1:%f,y2:%f\n", segmenti[i].x1, segmenti[i].x2, segmenti[i].y1, segmenti[i].y2);
                printf("lato x1:%f,x2:%f,y1:%f,y2:%f\n", seg_car[j].x1, seg_car[j].x2, seg_car[j].y1, seg_car[j].y2);
                printf("Contatto per j: %d\n", j);
                n_contact++;
            }
        }
    }
}

/**
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return
 */
segmento segmento_tra_due_punti(double x1, double y1, double x2, double y2) {

    if (car_extreme_angle == 0) {
        car_extreme_angle = rad_to_deg(atan2(car_half_width, car_half_length));
    }
    segmento s;
    if (x1 == x2) { x1++; }
    if (y1 == y2) { y1++; }
    s.x1 = x1;
    s.x2 = x2;
    s.y1 = y1;
    s.y2 = y2;
    s.a = 1 / (x2 - x1);
    s.b = 1 / (y2 - y1);
    s.c = y1 * s.b - x1 * s.a;
    //   printf("x1:%f,x2:%f,y1:%f,y2:%f\n", s.x1, s.x2, s.y1, s.y2);
    return s;
}

/**
 * @brief nella simulazione il sensore TOF rileva gli ostacoli come il punto di sovrapposizione
 * tra un segmento che parte dalla macchina estendendosi difronte a questa e gli ostacoli rappresentati come segmenti.
 * @return
 */
int read_tof_simulation_part() {

    estremi_macchina();
    osalThreadDelayMilliseconds(timingBudget);
    double distance = 2000;
    /* creo il segmento sello spazio in base alla posizione attuale della macchina */
    segmento segmento_sensore = segmento_dal_sensore(actual.x, actual.y, actual.facing_angle, range_TOF + car_half_length);

    /* controllo la sovrapposizione tra il segmento che rappresenta il vettore visivo del sensore e
     * tutti i segmenti rappresentanti gli ostacoli */
    for (int i = 0; i < num_seg; i++) {
        double x = intersezione_tra_segmenti(segmenti[i], segmento_sensore);
        if (x != 0) {
            double res = fabs((x - actual.x) / cos(deg_to_rad(actual.facing_angle)));
            if (debug_print) {
                printf("x1:%f,x2:%f,y1:%f,y2:%f\n", segmenti[i].x1, segmenti[i].x2, segmenti[i].y1, segmenti[i].y2);
                printf("Intersezione per x: %f\n", x);
                printf("TOF distanza rilevata: %f\n", res);
            }
            distance = res != 0 && res < distance ? fabs(res) - car_half_length : distance;
        }
    }
    if (distance < 2000 && debug_print) {
        printf("TOF distanza compensata: %f\n\n", distance);
    }
    return (int) distance;
}

/**
 * @brief procede per il numero indicato di cicli di movimento la simulazione
 * @return
 */
int movement_control_simulation_part() {

    skip_cycle--;
    if (!skip_cycle) {
        printf("Totale contatti: %d\n", n_contact);
        printf("Continua n_cycle else Completa 0: ");
        scanf("%d", &skip_cycle);
    }
    return skip_cycle;
}