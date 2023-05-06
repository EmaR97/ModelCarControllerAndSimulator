#include <iostream>
#include <math.h>
#include <vector>
#include <unistd.h>
#include <algorithm>

float angolo_min = 10; //il valore si puo diminuire per vedere se si puó ruotare di angoli piú piccoli in modo ripetibile
float raggio = 24; //distanza sensore centro macchina
float angolo_max = 40; //angolo massimo oltre il quale il muro risulta troppo distante, penso che 60 sia il massimo
int intervallo_tra_rilevazioni_distanza = 100; //intervallo in milli secondi
int i_max = (int) (angolo_max /
                   angolo_min); //facciamo le rielevazioni ad intervalli costanti di angolo in modo da vedere se dopo la partenza ruota a velocità angolare costante, se cosi fosse occorre capire dopo quanto tempo va a regime e poi possiamo farla girare dellángolo che vogliamo
                   
float rileva_distanza() {
    //TODO
}

double deg_to_rad(float ang) {
    return 3.14 / 180 * ang;
}


float distanza_obbiettivo(float distanza_iniziale,
                          float angolo) //definiamo la funzione in modo da poter effettuare le prove con i vari angoli
{
    return (float) ((distanza_iniziale + raggio) / cos(deg_to_rad(angolo))) - raggio;
}

void start_rotation() {
    //TODO
}

void stop_motor() {
    //TODO
}

float get_time() {
    //TODO
}

void calibra_tempo_di_rotazione() {
    float D_i = rileva_distanza();
    float D_o = distanza_obbiettivo(D_i, angolo_min);
    float t_i = get_time();
    int i = 1;
    std::vector<float> map;

    start_rotation();

    while (true) {
        if (D_o == rileva_distanza()) {
            float t_f = get_time();
            map[i] = t_f - t_i; //salva i vari intervalli per poterli verificare
            t_i = t_f; //aggiorna il tempo iniziale con l'ultima rilevazione
            std::cout << map[i] << std::endl;
            if (++i > i_max) {
                stop_motor();
                break;
            }
            D_o = distanza_obbiettivo(D_i, angolo_min *
                                           (float) i);//calcola la nuova distanza obbiettivo per lángolo successivo
        }
        sleep(intervallo_tra_rilevazioni_distanza);
    }
}


int main() {
    calibra_tempo_di_rotazione();
}

struct Point {
    float x;
    float y;
    float d;
    float a;
};

Point
punto_rispetto_al_centro(float distanza_1, float angolo_1) //calcola le coridiante dei punti rilevati rispetto al centro
{
    float x_1 = distanza_1 * cos(angolo_1);
    float y = distanza_1 * sin(angolo_1);
    float x_2 = raggio + x_1;
    float angolo_2 = atan(y / x_2);
    float distanza_2 = y / sin(angolo_2);
    return Point{x_2, y, distanza_2, angolo_2};
}

std::tuple<float, float, float> corridoio_tra_due_punti(Point ostacolo_A,
                                                        Point ostacolo_B) //calcola le dimensioni del corridoio_tra_due_punti per individuare la strada ottimale per la macchinina
{
    float x_medio = (ostacolo_A.x + ostacolo_B.x) / 2;
    float y_medio = (ostacolo_A.y + ostacolo_B.y) / 2;
    float angolo = atan(y_medio / x_medio);
    float d_a_centro = ostacolo_A.d * sin(angolo - ostacolo_A.a);
    float d_b_centro = ostacolo_B.d * sin(angolo - ostacolo_B.a);
    return {d_a_centro, d_b_centro, angolo};
}

float raggio2 = 15; //metà della larghezza della macchinina
float distanza_di_sicurezza = 5;
float meta_diagonale_macchina = (float) pow((raggio2 * raggio2 + raggio * raggio), 0.5);
float spazio_per_sterzata =
        meta_diagonale_macchina + distanza_di_sicurezza; //spazio necessario affinche la macchina possa ruotare

float cerca_corridoio(
        std::vector<Point> ostacoli_rilevati) //controlla le distanze tra gli ostacoli per cercare il corridoio ottimale per spostarsi in avanti
{
    std::vector<float> angoli_possibili;
    for (int i = 0; i < ostacoli_rilevati.size() - 2; i++) {
        auto [d_a_centro, d_b_centro, angolo] = corridoio_tra_due_punti(ostacoli_rilevati[i], ostacoli_rilevati[i + 1]);
        if (d_a_centro > spazio_per_sterzata && d_b_centro > spazio_per_sterzata) {
            angoli_possibili.push_back(angolo);
        }
    }
    return *std::min_element(angoli_possibili.begin(), angoli_possibili.end());
}

std::vector<float> rileva_ostacoli() {
    return ostacoli_rilevati;
}

