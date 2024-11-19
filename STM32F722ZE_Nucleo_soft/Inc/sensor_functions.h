/*
 * sensor_functions.h
 *
 *  Created on: Mar 4, 2024
 *      Author: lilia
 */

#ifndef INC_SENSOR_FUNCTIONS_H_
#define INC_SENSOR_FUNCTIONS_H_


#include "main.h"

#define NEO6M_GPS_MAXFRAMELENGHT	1024 // MAx length of data transmittable for gps
#define TAB_LENGTH	512//NEO6M_GPS
#define GGA_LENGTH	70
#define RxBuf_gps_SIZE	1024
#define RxBuf_HC05_BT_SIZE	200
#define SEND_HC05_LENGTH 18//size of a longitude and latitude data

typedef enum{KO = 0, OK = 1}Checksum;
typedef enum{Idle = 0, Event = 1}Flag_Rx_UART;
typedef enum{Free = 0, Busy = 1}State_UART;

typedef struct GPGGAData {
    int hour;
    int minute;
    int second;
    double latitude;
    double longitude;
    int fix_quality;
    int satellites;
    double altitude;
    Checksum checksum;
}GPGGAData_t;

extern Flag_Rx_UART Rx_flag_gps;
extern char RxBuf_gps[RxBuf_gps_SIZE];//gps neo6m
extern char gps_data[TAB_LENGTH];
extern HAL_StatusTypeDef feedback;
extern State_UART state_uart4;
extern State_UART Treatment_flag_gps;

void sensor_inits(UART_HandleTypeDef *huart_log, UART_HandleTypeDef *huart_neo6m_gps, UART_HandleTypeDef *huart_HC05);
//void NEO_6M_Read_data(UART_HandleTypeDef *huart_log, UART_HandleTypeDef *huart_gps);
void NEO_6M_Read_data(UART_HandleTypeDef *huart_log, UART_HandleTypeDef *huart_gps, UART_HandleTypeDef *hc05);
void NEO_6M_Read_data_basic(UART_HandleTypeDef *huart_gps, UART_HandleTypeDef *hc05);
void GGA_Frame_Decode(char *frame);

#endif /* INC_SENSOR_FUNCTIONS_H_ */
