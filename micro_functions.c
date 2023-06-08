////
//// Created by emanu on 27/05/2023.
////
//#include "components.h"
//#include "AEK_SNS_VL53L1X1.h"
//#include "EV_VNHx7xxx.h"
//#include "AEK_MOT_SM81M1_Driver.h"
//#include "serial_lld_cfg.h"
//#include "micro_functions.h"
//
//VL53L1X_Result_t result;
//
///**
// * @brief salvo la presenza o meno di Interrupt
// */
//volatile uint8_t irq_state = 0;
//
///**
// * @brief offset ottenuto dalla calibrazione auto automatica del sensore TOF
// */
//int16_t Offset = 50;
//
///**
//* @brief esegue le funzioni di inizializzazione delle componenti usate
//*/
//
//void initialization_sequence(void) {
//    /* Initialization of all the imported components in the order specified in
//     the application wizard. The function is generated automatically.*/
//    componentsInit();
//    /* Uncomment the below routine to Enable Interrupts. */
//    irqIsrEnable();
//    /* Initialize TOF */
//    initializeTOF();
//    /* Inizializza serial */
//    sd_lld_start(&SD2, &serial_config_configuration_name);
//    /* Inizializza motor */
//    initMotor(EV_VNHx7xxx_DEV0);
//    initMotor(EV_VNHx7xxx_DEV1);
//    init_AEK_MOT_SM81M1(AEK_MOT_SM81M1_DEV0);
//    /* start (wait di connessione seriale)*/
//    osalThreadDelayMilliseconds(5000);
//}
//
//int read_tof_micro_part() {
//
//     uint32_t start_time = osalThreadGetMilliseconds();
//    do {
//        AEK_TOF_METHODS.GetIRQSensorState(AEK_TOF_DEV0, &irq_state);
//        uint32_t time = osalThreadGetMilliseconds();
//        if (time >= start_time + (2 * timingBudget)) {
//            return 2000;
//        }
//    } while (irq_state != 1);
//    AEK_TOF_METHODS.GetResult(AEK_TOF_DEV0, &result);
//    AEK_TOF_METHODS.SetIRQSensorState(AEK_TOF_DEV0);
//    AEK_TOF_METHODS.ClearInterrupt(AEK_TOF_DEV0);
//    return result.Distance;
//}
//
//void motor_control_micro_part(direction *d, speed *s) {
//
//    uint8_t right_motor_direction;
//    uint8_t left_motor_direction;
//    uint8_t speed = (*s);
//    switch ((*d)) {
//        case right:
//            right_motor_direction = CLOCKWISE;
//            left_motor_direction = COUNTERCLOCKWISE;
//            break;
//        case left:
//            right_motor_direction = COUNTERCLOCKWISE;
//            left_motor_direction = CLOCKWISE;
//            break;
//        case forward:
//            right_motor_direction = CLOCKWISE;
//            left_motor_direction = CLOCKWISE;
//            break;
//        case backward:
//            right_motor_direction = COUNTERCLOCKWISE;
//            left_motor_direction = COUNTERCLOCKWISE;
//            break;
//    }
//    setSpeedMotor(EV_VNHx7xxx_DEV0, right_motor_direction, speed);
//    setSpeedMotor(EV_VNHx7xxx_DEV1, left_motor_direction, speed);
//}
//
//void stop_motor_micro_part() {
//
//    brakeMotor(EV_VNHx7xxx_DEV0);
//    brakeMotor(EV_VNHx7xxx_DEV1);
//}
//
///**
// * @brief configurazione e calibrazione del sensore di prossimità
// */
//void initializeTOF(void) {
//    /* Init Sensor  */
//    AEK_TOF_CONFIG();
//    /* stoppa il sensore se acceso*/
////    AEK_TOF_METHODS.StopRanging(AEK_TOF_DEV0);
//    /* valori default sensore */
//    AEK_TOF_METHODS.SensorInit(AEK_TOF_DEV0);
//    /* distance mode short (fino a 1.3m) in modo da avere una maggiore precisione in un tempo minore e avere minori problemi con la luce*/
//    AEK_TOF_METHODS.SetDistanceMode(AEK_TOF_DEV0, 1);
//    /* durata della singola misurazione */
//    AEK_TOF_METHODS.SetTimingBudgetInMs(AEK_TOF_DEV0, timingBudget); //20 ms
//    /* tempo inter-misura deve essere >= tempo di misura */
//    AEK_TOF_METHODS.SetInterMeasurementInMs(AEK_TOF_DEV0, timingBudget); //54 ms
//    /* setto l'area d'interesse, la dimensione minima ï¿½ 4x4 */
//    AEK_TOF_METHODS.SetROI(AEK_TOF_DEV0, 4, 4);
//    /* sposto il centro della ROI */
////AEK_TOF_METHODS.SetROICenter(AEK_TOF_DEV0, 191);
//    /*
//     * setto la finestra di alert
//     * par2 -> minima 0mm
//     * par3 -> massima 4m
//     * par4 -> alert dentro misura (compreso)
//     * par5 -> da documentazione dice 1
//     */
//    AEK_TOF_METHODS.SetDistanceThreshold(AEK_TOF_DEV0, 0, range_TOF, 3, 0);
//    /* calibrazione sensore posizionandolo a 30cm da un ostacolo */
////    AEK_TOF_METHODS.CalibrateOffset(AEK_TOF_DEV0, 300, &Offset);
//    /*letto il valore di offset va inserito nella funzione inserito nella funzione offset e commentata la funzione di calibrate*/
////    trace("Valore di Offset =", Offset);
//    AEK_TOF_METHODS.SetOffset(AEK_TOF_DEV0, Offset);
//    /* inizia il campionamento */
//    AEK_TOF_METHODS.StartRanging(AEK_TOF_DEV0);
//}
//
//
///* Serial functions */
//
///**
//* @brief  calculate string length
//*
//* @param[in] char *str - string
//*
//* @return string length
//*/
//int stringLen(const char *str) {
//
//    int n = 0;
//    while (str[n] != '\0') {
//        n++;
//    }
//    return n;
//}
//
///**
// *  @brief      Send a message to terminal for print traceError result
// *
// *  @param[in]  char *outputMessage - string
// *
// *  @api
// */
//void sendMessage(char *outputMessage) {
//
//    sd_lld_write(&SD2, (uint8_t *) outputMessage, (stringLen(outputMessage)));
//}
//
///**
// *  @brief non si può togliere serve a qualcosa di non ben specificato
// */
//void *sbrk(size_t incr) {
//
//    extern uint8_t __heap_base__;
//
//    extern uint8_t __heap_end__;
//
//    static uint8_t *p = &__heap_base__;
//
//    static uint8_t *newp;
//    newp = p + incr;
//    if (newp > &__heap_end__) {
//        return (void *) -1;
//    }
//    return p = newp;
//}
