#include "stdio.h"
#include "math.h"
#include "stdarg.h"
#include "simulation_function.h"
#include "processing_functions.h"
#include "main.h"
//#include "micro_functions.h"

/* constant */
const int time_to_full_power = 200;

const int car_half_length = 240;

const int car_half_width = 150;

const int car_half_diagonal = 283;

const int space_needed_to_turn = 350;

const int range_TOF = 1000;

/**
 * intervalli a cui salvare gli angoli in base alla distanza
 */
const int m[] = {45, 30, 20, 15, 10};

const int debug_print = 0;

/* il valore é cosi alto per fare la simulazione di base mi pare fosse 20 o 40 */
const uint16_t timingBudget = 50;

/*global variable*/

car_position actual = {0, 0, 0};

car_position precedent_position;

/**
 * valori di velocità rilevati durante la fase di calibrazione, cambiano in base alla terreno e alla carica della batteria
 */
speed_info frontal = {0.246, 1000, 188}, angular = {0.0372, 1000, 22};

car_position vicoli_cechi[100];

point first_obstacle, precedent_obstacle, last_obstacle;

int passaggi[20];

/**
 * numero di vicoli cechi
*/
int num_v_c = 0;

/**
 * variabile usata per invertire la direzione di rotazione usata durante la fase di scansione degli ostacoli,
 * usata per compensare l'accumolo dell'errore dovuto all'imprecisione della calibrazione
 */
int rotazione_inversa = 0;

/**
 * Application entry point.
 */
int main(void) {
/* START SIMULATION PART */
    read_point_value();
/* END SIMULATION PART */

/* MICRO ONLY START */
//    initialization_sequence();
//    complete_calibrate();
/* MICRO ONLY END */

    /* completata la fase di calibrazione, eseguiamo la funzione per il controllo del movimento */
    movement_control(1);
}

/**
 * @brief sequenza di funzione usate per individuare la velocità e la velocità angolare, la correttezza dei valori ottenuti viene verificata manualmente facendo compiere alla macchina dei movimenti noti
 */
__attribute__((unused)) void complete_calibrate() {
    /* provare valori diversi (150,20,5) (40,75,1) (40,40,1) */
    /* nella speranza che vada dritto appoggialo al muro, dovrebbe tornare un pò indietro, ma restare perpendicolare */
    move(100, backward);
    /* calcolo velocity lineare */
    calibrate_velocity(1000, 5, 1);
    /* controlliamo la calibrazione */
    move(300, backward);
    move(300, forward);
    /* calcolo velocità angolare */
    calibrate_velocity(1000, 5, 0);
    /* controlliamo se la calibrazione ha funzionato facendola girare di 90 gradi e poi tornando al punto di partenza */
    move(90, left);
    move(90, right);
    move(180, left);
    move(180, right);
    move(90, right);
    /* completata la calibrazione azzeriamo la posizione per poi tenere traccia degli spostamenti */
    reset_car_position();
}

/**
 * @brief return opposite direction
 * @param dir
 * @return
 */
direction opposite(direction dir) {

    switch (dir) {
        case right:
            dir = left;
            break;
        case left:
            dir = right;
            break;
        case forward:
            dir = backward;
            break;
        case backward:
            dir = forward;
            break;
    }
    return dir;
}

/**
 * @brief aspetta il numero indicato di rilevazioni del sensore e ne restituisce una media
 * @param num_samples
 * @return
 */
int read_approximated_tof(int num_samples) {

    int sum_results = 0;
    for (int i = 0; i < num_samples; i++) {
        sum_results += read_tof();
    }
    return sum_results / num_samples;
}

/**
 * @brief aspetta che il sensore completi una rilevazione e ne legge il risultato
 * @return
 */
int read_tof() {
/* MICRO ONLY START */
//    return read_tof_micro_part();
/* MICRO ONLY END */

/* START SIMULATION PART */
    return read_tof_simulation_part();
/* END SIMULATION PART */
}

/**
 * @brief riordina array con valori assoluti crescenti
 * @param arr
 * @param n
 */
void selectionSort(int arr[], int n) {

    int i, j, min_idx, temp;
    for (i = 0; i < n - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < n; j++) {
            if (abs(arr[j]) < abs(arr[min_idx])) {
                min_idx = j;
            }
        }
        temp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = temp;
    }
}

/**
 * @brief controlla la direzione di movimento e la velocità
 * @param d
 * @param s
 */
void motor_control(direction d, speed s) {

/* MICRO ONLY START */
//    motor_control_micro_part(&d, &s);
/* MICRO ONLY END */

    trace("Velocità motori: %d", s);
    trace("Direzione motori: %d", d);

}

void stop_motor() {

/* MICRO ONLY START */
//    stop_motor_micro_part();
/* MICRO ONLY END */

    trace("Spegnimento motori");
}

/**
 * @brief La calibrazione avviene tramite una serie di passaggi successivi:
 - si accendono i motori per un tempo, che si pensa sufficiente, affinché si raggiunga una velocità costante,
 - si spengono i motori e si rivela la distanza percorsa, questa sara la distanza minima che si può percorrere, con un unico passaggio.
 - si torna al punto di partenza, accendendo i motori nella direzione opposta per lo stesso tempo del caso precedente.
 - si riparte della posizione di partenza, mantenendo  i motori accesi per un tempo maggiore.
 - si spengono i motori e si rivela la distanza percorsa finale
 Ottengo la velocità come (distanza finale - distanza minima)/(tempo seconda iterazione - tempo prima iterazione).
 La stima ottenuta usando la macchine con terreni ruvidi, che non causano scivolamento è abbastanza  accurata.
 *
 * @param init_time
 * @param numSamples
 * @param tipo
 */
__attribute__((unused)) void calibrate_velocity(int init_time, int numSamples, int tipo) {

    trace("\nCalibrate_velocity");
    trace("\nCalibrate velocity tipo: %d", tipo);
    /* prendo distanza frontale iniziale */
    int initial_distance = read_approximated_tof(numSamples);
    trace("initial distance: %d", initial_distance);
    /* setto i valori iniziali */
    int set_speed;
    uint32_t starting_time, last_read_time, total;
    direction dir;
    if (tipo == 0) {
        dir = right;
    }
    else {
        dir = backward;
    }
    /* per i = 0 --> dx oppure indietro */
    /* per i = 1 --> sx oppure avanti */
    angular.init_time = init_time;
    frontal.init_time = init_time;
    for (int i = 0; i < 4; i++) {
        set_speed = 0;
        trace("\nCiclo %d", i);
        starting_time = osalThreadGetMilliseconds();
        motor_control(dir, slow); //sterzo a dx lentamente
        if (i < 2) {
            total = init_time;
        }
        else {
            total = 1500;
        }
        while (1) {
            last_read_time = osalThreadGetMilliseconds();
            /* campiono ogni interval_millis (solo in andata) */
            /* da velocità slow a normal */
            if (set_speed == 0 && last_read_time >= starting_time + time_to_full_power) {
                motor_control(dir, normal);
                set_speed = 1;
            }
            /*completo e spengo il motore*/
            if (last_read_time >= (starting_time + total)) {
                stop_motor();
                break;
            }
        }
        osalThreadDelayMilliseconds(500);
        /* confronta i valori iniziali con quelli finali per poi calcolare la velocità media. */
        if (i == 0) {
            if (tipo == 0) {
                trace("angular.init_value");
                angular.init_value = get_approximated_angle(initial_distance, read_approximated_tof(numSamples));
            }
            else {
                trace("frontal.init_value");
                frontal.init_value = read_approximated_tof(numSamples) - initial_distance;
            }
        }
        else if (i == 2) {
            if (tipo == 0) {
                trace("angular.final_value");
                double angolo_after_1500 = get_approximated_angle(initial_distance, read_approximated_tof(numSamples));
                trace("\nangolo_after_1500= %f", angolo_after_1500);
                angular.velocity = (double) (angolo_after_1500 - angular.init_value) / (double) (total - angular.init_time);
                trace("\nVelocità   costante finale= %f", angular.velocity);
                trace("A partire da millisecond finale= %f", angular.init_time);
                trace("A partire da valore finale= %f", angular.init_value);
            }
            else {
                trace("frontal.final_value");
                frontal.velocity = (double) (read_approximated_tof(numSamples) - initial_distance - frontal.init_value) / (double) (total - frontal.init_time);
                trace("\nVelocità   costante finale= %f", frontal.velocity);
                trace("A partire da millisecond finale= %f", frontal.init_time);
                trace("A partire da valore finale= %f", frontal.init_value);
            }
        }
        dir = opposite(dir);
    }
}

/**
 * @brief se la con direzione forward e backward si muove in avanti o indietro della distanza indicata in millimetri,
 * con left e right ruota su se stessa dell'angolo indicato
 * per valori minori al valore di soglia, deve compiere due spostamenti per portarsi al valore indicato
 * @param quantity
 * @param dir
 */
void move(int quantity, direction dir) {

    trace("\nMove");
    speed_info to_use = frontal;
    int too_small = 0;
    /* se la direzione é laterale imposta i valori relativi alla velocità angolare */
    if (dir == left || dir == right) {
        to_use = angular;
        quantity = quantity % 360;
    }
    trace("quantity= %d", quantity);
    if (quantity == 0) {
        return;
    }
    /* per valori minori al valore di soglia, deve compiere due spostamenti per portarsi nel punto giusto */
    if (quantity < to_use.init_value) {
        too_small = 1;
        quantity += (int) to_use.init_value;
    }
    int necessary_rotation_time = (int) (((quantity - to_use.init_value) / to_use.velocity) + to_use.init_time);
    for (int i = 0; i <= too_small; i++) {
        uint32_t starting_time = osalThreadGetMilliseconds(), actual_time;
        motor_control(dir, slow);
        trace("starting_time= %lu", starting_time);
        trace("total_time= %lu", starting_time + necessary_rotation_time);
        trace("time_to_full_power= %lu", starting_time + necessary_rotation_time);
        int full_power = 0;
        car_position old = {actual.x, actual.y, actual.facing_angle};
        trace("necessary_rotation_time= %d", necessary_rotation_time);
        while (1) {
            actual_time = osalThreadGetMilliseconds();
            /* se non si sta muovendo piano ed é passato l'intervallo iniziale aumenta la PWM dei motori*/
            if (actual_time > starting_time + time_to_full_power) {
                if (!full_power) {
                    motor_control(dir, normal);
                    full_power = 1;
                }

                    /* START SIMULATION PART */
                else {
                    actual.x = old.x;
                    actual.y = old.y;
                    actual.facing_angle = old.facing_angle;
                    evaluate_car_position(dir, actual_time - starting_time, 0);
                }
                /* END SIMULATION PART */

            }
            /* se sta muovendo in avanti ed é presente un ostacolo a meno di 300 millimetri si ferma */
            /* se ha superato il tempo calcolato si ferma*/
            if ((dir == forward && read_tof() < (300 + frontal.velocity * timingBudget)) || actual_time > starting_time + necessary_rotation_time) {

                /* START SIMULATION PART */
                actual.x = old.x;
                actual.y = old.y;
                actual.facing_angle = old.facing_angle;
                /* END SIMULATION PART */

                stop_motor();
                break;
            }
        }
        /* salva il tempo finale per poter calcolare la posizione anche in caso si sia fermato in presenza di un ostacolo*/
        uint32_t total_time = osalThreadGetMilliseconds() - starting_time;
        evaluate_car_position(dir, total_time, 1);
        /* se il valore era troppo piccolo reimposta i nuovi parametri per il secondo ciclo di spostamento. */
        if (too_small == 1) {
            dir = opposite(dir);
            necessary_rotation_time = (int) angular.init_time;
        }
        osalThreadDelayMilliseconds(500);
    }
}

/**
 * @brief calcola una stima della nuova posizione in base allo spostamento effettuato, alla posizione iniziale
 * @param dir
 * @param total_time
 */
void evaluate_car_position(direction dir, uint32_t total_time, int print) {

    if (print) {
        trace("\nEvaluate_car_position");
        trace("total_time: %lu", total_time);
        trace("last.facing_angle: %f", actual.facing_angle);
        trace("last.x: %f", actual.x);
        trace("last.y: %f", actual.y);
    }
    speed_info to_use = frontal;
    if (total_time) {
        if (dir == left || dir == right) {
            to_use = angular;
            double angle = to_use.init_value + (total_time - to_use.init_time) * to_use.velocity;
            if (print) {
                trace("angle: %f", angle);
            }
            if (dir == right) {
                actual.facing_angle += angle;
            }
            else {
                actual.facing_angle -= angle;
            }
        }
        else {
            double distance = to_use.init_value + (total_time - to_use.init_time) * to_use.velocity;
            if (dir == forward) {
                actual.x += cos(deg_to_rad(actual.facing_angle)) * distance;
                actual.y += sin(deg_to_rad(actual.facing_angle)) * distance;
            }
            else {
                actual.x -= cos(deg_to_rad(actual.facing_angle)) * distance;
                actual.y -= sin(deg_to_rad(actual.facing_angle)) * distance;
            }
        }
        actual.facing_angle = format_angle(actual.facing_angle);
        if (print) {
            trace("actual.facing_angle: %f", actual.facing_angle);
            trace("actual.x: %f", actual.x);
            trace("actual.y: %f", actual.y);
        }
    }
}

__attribute__((unused)) void reset_car_position(void) {

    actual.facing_angle = 0;
    actual.x = 0;
    actual.y = 0;
}

/**
 *  @brief      Print to terminal the location and type of error
 *
 *  @param[in]  char *str - string
 *
 *  @param[in]  i2c_result_t errorType - error type value (possible value in enum i2c_result_t)
 *
 *  @api
 */
void trace(const char *format, ...) {

    char msg[100];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

/* START SIMULATION PART */
    printf("%s\n", msg);
/* END SIMULATION PART */

/* MICRO ONLY START */
//    sendMessage(msg);
/* MICRO ONLY END */

}

/* parte per il controllo e la scelta del percorso */

/**
 * @brief Controllo del movimento della macchina compresa la ricerca del percorso
 * @param num_samples
 */
void movement_control(int num_samples) {

    trace("\nMovement_control");
    trace("space_needed_to_turn: %d", space_needed_to_turn);
    int ciclo = 0;
    for (;;) {
        trace("movement_control ciclo: %d", ciclo++);
        /* la macchina si mette in posizione e ruota su se stessa di 360 gradi mentre il sensore rileva gli ostacoli e cerca i possibili corridoi tra questi */
        int num_corridoi = 0;
        num_corridoi = rileva_ostacoli(num_samples, num_corridoi);
        trace("corridoi trovati: %d", num_corridoi);
        int completo;
        /* riordino i passaggi in modo da scegliere quello con l'angolo minore rispetto alla direzione nella quale mi voglio muovere
         * la direzione preferenziale è quella che la macchina sta puntando all'inizio o a seguito della chiamata alla funzione reset position */
        selectionSort(passaggi, num_corridoi);
        /* si scorrono i corridoi individuati e i vicoli cechi per cercare il corridoi migliore che non é stato percorso e lo si percorre*/
        for (int j = 0; j < num_corridoi; j++) {
            int passaggio_da_controllare = passaggi[j];
            completo = controlla_passaggio(num_corridoi, passaggio_da_controllare);
            if (completo) {
                break;
            }
        }

        /* START SIMULATION PART */
        if (!movement_control_simulation_part()) { return; }
        /* END SIMULATION PART */
    }
}

/**
 * @brief Ruota su se stessa di 360 rilevando gli ostacoli, ad intervalli costanti,
 * e calcola i corridoi sufficientemente larghi che la macchina può percorrere
 * @param num_samples
 * @param num_p
 */
int rileva_ostacoli(int num_samples, int num_p) {
    /* la macchina ruota per posizionarsi a 90 gradi dalla direzione iniziale + il valore di angle per il quale non conosco la velocità angolare */
    trace("\nRileva_ostacoli");
    num_p = 0;
    evaluate_car_position(0, 0, 1);
    uint32_t starting_time = osalThreadGetMilliseconds();
    int classe_ostacolo_o = 4, full_power = 0, primo_ostacolo = 1, dis_ostacolo_last = 0;
    double ang_ostacolo_o = 0, ang_ostacolo_last = 0;
    /* calcolo il tempo necessario per fare 180 gradi + il valore di angle per il quale non conosco la velocità angolare */
    int necessary_rotation_time = (int) (angular.init_time + (360 / angular.velocity));
    /* comincia a ruotare in direzione opposta a quella iniziale */
    if (!rotazione_inversa) {
        motor_control(left, slow);
    }
    else {
        motor_control(right, slow);
    }
    double initial_facing_angle = actual.facing_angle;
    double res;
    while (1) {
        uint32_t time = osalThreadGetMilliseconds();
        /* prima si porta a regime  */
        if (time > starting_time + time_to_full_power && !full_power) {
            /* per compensare l'imprecisione nella rotazione la macchina ruota una volta in senso orario e la successiva in senso antiorario*/
            if (!rotazione_inversa) {
                motor_control(left, normal);
            }
            else {
                motor_control(right, normal);
            }
            full_power = 1;
        }
        /* raggiunti l'angolo desiderato si blocca */
        if (time > starting_time + necessary_rotation_time) {
            stop_motor();
            break;
        }
        /* se siamo nella fase a velocità costante si inizia a scansionare per la presenza di ostacoli */
        if (time > starting_time + angular.init_time) {
            /*la prima volta il valore non é presente le successive é gia stato calcolato*/
            if (actual.facing_angle == initial_facing_angle) {
                actual.facing_angle = format_angle(initial_facing_angle - (osalThreadGetMilliseconds() - (starting_time + angular.init_time)) * angular.velocity);
            }
            if (debug_print) { trace("rileva-ostacolo_angolo: %f", actual.facing_angle); }
            int dis_ostacolo_n = read_approximated_tof(num_samples);
            /* calcolo l'angolo in base al valore di tempo trascorso moltiplicandolo per la velocità angolare */
            actual.facing_angle = format_angle(initial_facing_angle - (osalThreadGetMilliseconds() - (starting_time + angular.init_time)) * angular.velocity);
            /* passo tutti i valori alla funzione che li salva come punti */
            if (memorizza_ostacoli(dis_ostacolo_n, format_angle(actual.facing_angle), &classe_ostacolo_o, &ang_ostacolo_o, &ang_ostacolo_last, &dis_ostacolo_last)) {
                if (primo_ostacolo == 1) {
                    primo_ostacolo = 0;
                    first_obstacle = last_obstacle;
                }
                else {
                    if (!rotazione_inversa) {
                        /* tra gli ostacoli rilevati si cercano dei corridoi abbastanza larghi da permettere il passaggio della macchina */
                        res = corridoio_tra_due_punti(precedent_obstacle, last_obstacle);
                    }
                    else {
                        /*quando il senso è inverso i punti vanno letti in ordine inverso*/
                        res = corridoio_tra_due_punti(last_obstacle, precedent_obstacle);
                    }
                    /* se trovo un passaggio lo salvo per poi controllarlo alla fine della scansione*/
                    if (res != 0) {
                        trace("Trovato Corridoio");
                        passaggi[num_p++] = (int) res;
                    }
                }
            }
        }
    }
    if (primo_ostacolo) {
        passaggi[num_p++] = 0;
    }
    if (!rotazione_inversa) {
        /* tra gli ostacoli rilevati si cercano dei corridoi abbastanza larghi da permettere il passaggio della macchina */
        res = corridoio_tra_due_punti(last_obstacle, first_obstacle);
    }
    else {
        res = corridoio_tra_due_punti(first_obstacle, last_obstacle);
    }
    if (res != 0) {
        passaggi[num_p++] = (int) res;
    }
    rotazione_inversa = !rotazione_inversa;
    return num_p;
}

/**
 * @brief in base alla distanza degli ostacoli sceglie quali valori salvare e quali non sono necessari
 * quando vi è una repentina variazione della distanza di due ostacoli successivi questi vengono salvati
 *
 * @param dis_ostacolo_n
 * @param ang_ostacolo_n
 * @param classe_ostacolo_o
 * @param ang_ostacolo_o
 * @return
 */
int memorizza_ostacoli(double dis_ostacolo_n, double ang_ostacolo_n, int *classe_ostacolo_o, double *ang_ostacolo_o, double *ang_ostacolo_last, int *dis_ostacolo_last) {

    if (debug_print) { trace("\nMemorizza_ostacoli"); }
    int res;
    int classe_ostacolo_n = (int) ceil(dis_ostacolo_n / 400) - 1;
    /* se gli ostacoli sono nello stesso range di distanza e si trovano a una distanza angolare inferiore a una certa sogli non vengono salvati*/
    if (*classe_ostacolo_o == classe_ostacolo_n && (classe_ostacolo_n == 4 || (int) fabs(ang_ostacolo_n - *ang_ostacolo_o) < m[classe_ostacolo_n])) {
        res = 0;
    }
    else {
        /*quando non si rilevano ostacoli si salva l'ultimo ostacolo rilevato, che rappresenta un estremo di un ostacolo reale */
        if (*classe_ostacolo_o != 4 && classe_ostacolo_n == 4) {
            dis_ostacolo_n = *dis_ostacolo_last;
            ang_ostacolo_n = *ang_ostacolo_last;
        }
        precedent_obstacle = last_obstacle;
        last_obstacle = punto_rispetto_al_centro(dis_ostacolo_n, ang_ostacolo_n);
        *ang_ostacolo_o = ang_ostacolo_n;
        *classe_ostacolo_o = classe_ostacolo_n;
        res = 1;
    }
    *dis_ostacolo_last = (int) dis_ostacolo_n;
    *ang_ostacolo_last = ang_ostacolo_n;
    return res;

}

/**
 * @brief calcola se c'é abbastanza spazio tra due punti rilevati perché la macchina possa passare
 * @param ostacolo_A
 * @param ostacolo_B
 * @return
 */
double corridoio_tra_due_punti(point ostacolo_A, point ostacolo_B) {

    double angolo, x_medio, y_medio, d_a_centro, d_b_centro, ang_a, ang_b;
    x_medio = (ostacolo_A.x + ostacolo_B.x) / 2;
    y_medio = (ostacolo_A.y + ostacolo_B.y) / 2;
    int angolo_da_sottrarre = 60, angolo_grande = 0;
    double angle_dif = ostacolo_B.a <= ostacolo_A.a ? fabs(ostacolo_A.a - ostacolo_B.a) : 360 - fabs(ostacolo_A.a - ostacolo_B.a);
    /* in caso di ostacoli che si rilevano ad una distanza angolare elevata si applicano condizioni particolari
     * affinché la macchina si muova radente all'ostacolo */
    if (angle_dif > (angolo_da_sottrarre * 2)) {
        angolo_grande = 1;
    }
    else {
        angolo_da_sottrarre = 45;
        if (ostacolo_A.x > 100 && ostacolo_B.x > 100 && angle_dif > (angolo_da_sottrarre * 2)) {
            angolo_grande = 1;
        }
    }
    if (angolo_grande) {
        /* se l'angolo é maggiore di 90 tutto va male */
        double op_angolo = format_angle(ostacolo_A.a - angle_dif / 2);
        angolo = format_angle(ostacolo_A.a - angolo_da_sottrarre);
        angolo = fabs(angolo) < fabs(op_angolo) ? angolo : op_angolo;
        d_a_centro = 400;
        d_b_centro = 400;
    }
    else {
        /* se l'angolo é minore di 90 tutto va bene */
        angolo = ang_from_x_y(x_medio, y_medio);
        ang_a = angolo - ostacolo_A.a;
        ang_b = ostacolo_B.a - angolo;
        /* calcolo quanto gli estremi sono distanti dal centro del corridoio */
        d_a_centro = fabs(ostacolo_A.d * sin(deg_to_rad(ang_a)));
        d_b_centro = fabs(ostacolo_B.d * sin(deg_to_rad(ang_b)));
    }

    /* se il corridoio é abbastanza largo viene salvato, in caso contrario viene ignorato */
    if (d_a_centro > space_needed_to_turn && d_b_centro > space_needed_to_turn) {
        angolo = angolo == 0 ? 0.0001 : angolo;
    }
    else { angolo = 0; }
    if (debug_print || angolo) {
        trace("\nCorridoio_tra_due_punti");
        trace("ostacolo_A.x: %f", ostacolo_A.x);
        trace("ostacolo_A.y: %f", ostacolo_A.y);
        trace("ostacolo_A.a: %f", ostacolo_A.a);
        trace("ostacolo_A.d: %f", ostacolo_A.d);
        trace("ostacolo_B.x: %f", ostacolo_B.x);
        trace("ostacolo_B.y: %f", ostacolo_B.y);
        trace("ostacolo_B.a: %f", ostacolo_B.a);
        trace("ostacolo_B.d: %f", ostacolo_B.d);
        trace("y_medio: %f", y_medio);
        trace("x_medio: %f", x_medio);
        trace("passage.distance_a_to_center: %f", d_a_centro);
        trace("passage.distance_b_to_center: %f", d_b_centro);
        trace("passage.angle: %f\n", angolo);
    }
    return angolo;
}

/**
 * @brief elabora i valori calcolati dal sensore di prossimità per mappare gli ostacoli,
 * calcola le coordinate dei punti rilevati rispetto al centro
 * @param distanza_1
 * @param angolo_1
 * @return
 */
point punto_rispetto_al_centro(double distanza_1, double angolo_1) {

    double distanza_2 = car_half_length + distanza_1;
    double x = distanza_2 * cos(deg_to_rad(angolo_1));
    double y = distanza_2 * sin(deg_to_rad(angolo_1));
    point res = {x, y, distanza_2, angolo_1};
    return res;
}

/**
 * @brief sceglie il migliore tra i passaggi trovati
 * @param num_v_c
 * @param num_corridoi
 * @param passaggio_da_controllare
 * @return
 */
int controlla_passaggio(int num_corridoi, int passaggio_da_controllare) {

    trace("\nControlla_passaggio");
    trace("passaggio_da_controllare: %d", passaggio_da_controllare);
    for (int i = 0; i <= num_v_c; i++) {
        /* se la posizione e direzione non é ancora stata esplorata la prova ad avanzare di una distanza fissa */
        int check1 = 0;
        if (num_v_c > 0) {
            check1 = passaggio_da_controllare > vicoli_cechi[i].facing_angle - 10 && passaggio_da_controllare < vicoli_cechi[i].facing_angle + 10 && actual.x > vicoli_cechi[i].x - 10 && actual.x < vicoli_cechi[i].x + 10 && actual.y > vicoli_cechi[i].y - 10 && actual.y < vicoli_cechi[i].y + 10;
        }
        if (!check1) {
            double angle_to_rotate = format_angle(passaggio_da_controllare - format_angle(actual.facing_angle));
            /* se possibile evito corridoi che mi mandano nella direzione opposta a quella di partenza
             * se l'angolo é prossimo a 180 la macchina sta tornando indietro */
            if (!(fabs(angle_to_rotate) > 135 && fabs(angle_to_rotate) < 225) || (num_corridoi == 1 || i == num_corridoi - 1)) {
                /* la macchina si allinea con il passaggio individuato*/
                if (angle_to_rotate < 0) {
                    move((int) fabs(angle_to_rotate), left);
                }
                else if (angle_to_rotate > 0) {
                    move((int) fabs(angle_to_rotate), right);
                }
                /* se l'angolo é prossimo a 180 la macchina sta tornando indietro e la posizione si salva tra i vicoli cechi */
                if (fabs(angle_to_rotate) > 135 && fabs(angle_to_rotate) < 225) {
                    vicoli_cechi[num_v_c].facing_angle = precedent_position.facing_angle;
                    vicoli_cechi[num_v_c].x = precedent_position.x;
                    vicoli_cechi[num_v_c].y = precedent_position.y;
                    num_v_c++;
                }
                /* aggiorno la posizione precedente con quella attuale */
                precedent_position.facing_angle = actual.facing_angle;
                precedent_position.x = actual.x;
                precedent_position.y = actual.y;
                /* la macchina si muove in avanti di una distanza minore del range del sensore per evitare eventuali ostacoli non rilevati */
                move(range_TOF - 300 - timingBudget, forward);
                osalThreadDelayMilliseconds(1000);
                return 1;
            }
        }
    }
    return 0;
}

