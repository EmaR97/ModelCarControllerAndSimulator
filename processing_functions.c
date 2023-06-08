#include "main.h"
#include "math.h"
#include "processing_functions.h"

/**
 * @brief calcola l'angolo compreso tra cateto ed ipotenusa
 * @param start_distance
 * @param finish_distance
 * @return
 */
double get_approximated_angle(int start_distance, int finish_distance) {

    double approximated_start_distance = (start_distance + car_half_length);
    double approximated_finish_distance = (finish_distance + car_half_length);
    double approximated_angle = rad_to_deg(acos(approximated_start_distance / approximated_finish_distance));
    return approximated_angle;
}

double format_angle(double angle) {

    if (fabs(angle) > 180) {
        angle = angle > 0 ? (angle - 360) : (360 + angle);
    }
    return angle;
}

double deg_to_rad(double a) { return a / 180 * 3.14; }

double rad_to_deg(double a) { return a / 3.14 * 180; }

double ang_from_x_y(double x_medio, double y_medio) {

    double tan = y_medio / x_medio;
    double angle = rad_to_deg(atan(tan));
    if (x_medio < 0) { if (angle < 0) { angle += 180; } else { angle -= 180; }}
    return angle;
}