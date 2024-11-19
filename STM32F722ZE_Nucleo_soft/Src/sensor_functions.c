/*
 * sensor_functions.c
 *
 *  Created on: Mar 4, 2024
 *      Author: lilia
 */

#include "sensor_functions.h"

char RxBuf_gps[RxBuf_gps_SIZE] = "\0";
char gps_data[TAB_LENGTH] = "\0";
HAL_StatusTypeDef feedback = HAL_ERROR;
GPGGAData_t GPGGA_decoded;
State_UART state_uart4 = Free;
Flag_Rx_UART Rx_flag_gps = Idle;
State_UART Treatment_flag_gps = Free;

void sensor_inits(UART_HandleTypeDef *huart_log, UART_HandleTypeDef *huart_neo6m_gps, UART_HandleTypeDef *huart_HC05){
//	  char message[] = "hello";
//	  // Assume HC-05 configuration for master mode
//	  HAL_GPIO_WritePin(HC05_KEY_GPIO_Port, HC05_KEY_Pin, SET);//passer en mode AT opour config du hc05 avec pin key à l'état haut
//	  HAL_Delay(10000);  // Delay for connection
//	  // Send "AT+ROLE=1\r\n" to set HC-05 as master
//	  HAL_UART_Transmit(huart_HC05, (uint8_t *)"AT+ROLE=1\r\n", strlen("AT+ROLE=1\r\n"), HAL_MAX_DELAY);
//	  HAL_Delay(500);  // Delay for HC-05 response
//	  //configuration of baud rate
//	  HAL_UART_Transmit(huart_HC05, (uint8_t *)"AT+UART=115200,1,0\r\n", strlen("AT+UART=115200,1,0\r\n"), HAL_MAX_DELAY);
//	  HAL_Delay(500);  // Delay for HC-05 response
//	  // Connect to slave HC-05 (assuming slave address is known)
//	  HAL_UART_Transmit(huart_HC05, (uint8_t *)"AT+BIND=98da,50,03357a\r\n", strlen("AT+BIND=98da,50,03357a\r\n"), HAL_MAX_DELAY);
//	  HAL_Delay(1000);  // Delay for connection
//	  HAL_GPIO_WritePin(HC05_KEY_GPIO_Port, HC05_KEY_Pin, RESET);//leave AT mode
//	  HAL_Delay(1000);  // Delay for connection
//	  // Send message "hello" over Bluetooth
//	  HAL_UART_Transmit(huart_HC05, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

//	printf("\rsensor_inits\n");
	char NEO_6M_fields[] = "Heure UTC,Latitude(DD),Longitude(DD),Fix quality,Satellites used,Altitude(m),Checksum\r\n";

	feedback = HAL_UART_Transmit_DMA(huart_HC05, (uint8_t *)NEO_6M_fields, sizeof(NEO_6M_fields));
	if(feedback == HAL_ERROR)
		Error_Handler();

//	feedback = HAL_UART_Transmit_DMA(huart_log, (uint8_t *)NEO_6M_fields, sizeof(NEO_6M_fields));
	feedback = HAL_UART_Transmit(huart_log, (uint8_t *)NEO_6M_fields, sizeof(NEO_6M_fields), HAL_MAX_DELAY);
	if(feedback == HAL_ERROR)
		Error_Handler();

//	feedback = HAL_UART_Receive_DMA(huart_neo6m_gps, (uint8_t *)gps_data, NEO6M_GPS_MAXFRAMELENGHT);
//	HAL_UART_Abort(huart_neo6m_gps);
//	feedback = HAL_UARTEx_ReceiveToIdle_DMA(huart_neo6m_gps, (uint8_t *)RxBuf_gps, sizeof(RxBuf_gps));
//	if(feedback == HAL_ERROR)
//		Error_Handler();
}

void NEO_6M_Read_data(UART_HandleTypeDef *huart_log, UART_HandleTypeDef *huart_gps, UART_HandleTypeDef *hc05){
	static char GGA[GGA_LENGTH] = "\0";
	static char format[GGA_LENGTH*2] = "\0";
	char send_to_uart[GGA_LENGTH] = "\0";
	int j=0;
	memset(GGA,'\0',sizeof(GGA));
	memset(format,'\0',sizeof(format));
	for(int i=0; i<sizeof(gps_data); i++){
		if(gps_data[i]=='$' && gps_data[i+1]=='G' && gps_data[i+2]=='P' && gps_data[i+3]=='G' && gps_data[i+4]=='G' && gps_data[i+5]=='A' && gps_data[i+6]==','){
			j=0;
			i=i+7;
			while(gps_data[i]!='\r' && i<sizeof(gps_data)){
				GGA[j]=gps_data[i];
				i++;
				j++;
			}
			sprintf(format,"$GPGGA,%s\r\n", GGA);
//			printf("%s",format);
			GGA_Frame_Decode(format);
//			printf("Heure UTC: %02d:%02d:%02d\n", GPGGA_decoded.hour, GPGGA_decoded.minute, GPGGA_decoded.second);
//			printf("Latitude(DD): %lf\n", GPGGA_decoded.latitude);
//			printf("Longitude(DD): %lf\n", GPGGA_decoded.longitude);
//			printf("Fix quality: %d\n", GPGGA_decoded.fix_quality);
//			printf("Satellites used: %d\n", GPGGA_decoded.satellites);
//			printf("Altitude(m): %.2lf\n", GPGGA_decoded.altitude);
//			printf("Heure UTC;Latitude(DD);Longitude(DD);Fix quality;Satellites used;Altitude(m);Checksum\r\n");
//			printf("%02d:%02d:%02d;%.5lf;%.5lf;%d;%d;%.2lf;%d\r\n",GPGGA_decoded.hour, GPGGA_decoded.minute, GPGGA_decoded.second,GPGGA_decoded.latitude,GPGGA_decoded.longitude,GPGGA_decoded.fix_quality,GPGGA_decoded.satellites,GPGGA_decoded.altitude,GPGGA_decoded.checksum);
			sprintf(send_to_uart,"%02d:%02d:%02d,%.5lf,%.5lf,%d,%d,%.2lf,%d\r\n",GPGGA_decoded.hour, GPGGA_decoded.minute, GPGGA_decoded.second,GPGGA_decoded.latitude,GPGGA_decoded.longitude,GPGGA_decoded.fix_quality,GPGGA_decoded.satellites,GPGGA_decoded.altitude,GPGGA_decoded.checksum);

//			while(state_uart4 != Free){
//			while(huart_log->gState != HAL_UART_STATE_READY && huart_log->RxState != HAL_UART_STATE_READY){
//				//wait until the UART DMA stream getting released
//			}
//			feedback = HAL_UART_Transmit_DMA(huart_log, (uint8_t *)send_to_uart, strlen(send_to_uart));
			feedback = HAL_UART_Transmit(huart_log, (uint8_t *)send_to_uart, strlen(send_to_uart), HAL_MAX_DELAY);
//			state_uart4 = Busy;//waiting to be free in the complete callback function
//			feedback = HAL_UART_Transmit(huart_log, (uint8_t *)send_to_uart, strlen(send_to_uart), HAL_MAX_DELAY);
			if(feedback == HAL_ERROR)
				printf("Error !\n");

			feedback = HAL_UART_Transmit(hc05, (uint8_t *)send_to_uart, strlen(send_to_uart), HAL_MAX_DELAY);
//			feedback = HAL_UART_Transmit_DMA(hc05, (uint8_t *)send_to_uart, strlen(send_to_uart));
			if(feedback == HAL_ERROR)
				printf("Error !\n");
//			printf("%s\n",send_to_uart);
		}
	}
}

void NEO_6M_Read_data_basic(UART_HandleTypeDef *huart_gps, UART_HandleTypeDef *hc05){
	static char GGA[GGA_LENGTH] = "\0";
	static char format[GGA_LENGTH*2] = "\0";
	char send_to_uart[GGA_LENGTH] = "\0";
	char send_to_hc05[SEND_HC05_LENGTH] = "\0";
	int j=0;
	memset(GGA,'\0',sizeof(GGA));
	memset(format,'\0',sizeof(format));
	for(int i=0; i<sizeof(gps_data); i++){
		if(gps_data[i]=='$' && gps_data[i+1]=='G' && gps_data[i+2]=='P' && gps_data[i+3]=='G' && gps_data[i+4]=='G' && gps_data[i+5]=='A' && gps_data[i+6]==','){
			j=0;
			i=i+7;
			while(gps_data[i]!='\r' && i<sizeof(gps_data)){
				GGA[j]=gps_data[i];
				i++;
				j++;
			}
			sprintf(format,"$GPGGA,%s\r\n", GGA);
//			printf("%s",format);
			GGA_Frame_Decode(format);
//			printf("Heure UTC: %02d:%02d:%02d\n", GPGGA_decoded.hour, GPGGA_decoded.minute, GPGGA_decoded.second);
//			printf("Latitude(DD): %lf\n", GPGGA_decoded.latitude);
//			printf("Longitude(DD): %lf\n", GPGGA_decoded.longitude);
//			printf("Fix quality: %d\n", GPGGA_decoded.fix_quality);
//			printf("Satellites used: %d\n", GPGGA_decoded.satellites);
//			printf("Altitude(m): %.2lf\n", GPGGA_decoded.altitude);
//			printf("Heure UTC;Latitude(DD);Longitude(DD);Fix quality;Satellites used;Altitude(m);Checksum\r\n");
//			printf("%02d:%02d:%02d;%.5lf;%.5lf;%d;%d;%.2lf;%d\r\n",GPGGA_decoded.hour, GPGGA_decoded.minute, GPGGA_decoded.second,GPGGA_decoded.latitude,GPGGA_decoded.longitude,GPGGA_decoded.fix_quality,GPGGA_decoded.satellites,GPGGA_decoded.altitude,GPGGA_decoded.checksum);
			sprintf(send_to_uart,"%02d:%02d:%02d,%.5lf,%.5lf,%d,%d,%.2lf,%d\r\n",GPGGA_decoded.hour, GPGGA_decoded.minute, GPGGA_decoded.second,GPGGA_decoded.latitude,GPGGA_decoded.longitude,GPGGA_decoded.fix_quality,GPGGA_decoded.satellites,GPGGA_decoded.altitude,GPGGA_decoded.checksum);

//			while(state_uart4 != Free){
//			while(huart_log->gState != HAL_UART_STATE_READY && huart_log->RxState != HAL_UART_STATE_READY){
//				//wait until the UART DMA stream getting released
//			}
//			feedback = HAL_UART_Transmit_DMA(huart_log, (uint8_t *)send_to_uart, strlen(send_to_uart));
//			feedback = HAL_UART_Transmit(huart_log, (uint8_t *)send_to_uart, strlen(send_to_uart), HAL_MAX_DELAY);
//			state_uart4 = Busy;//waiting to be free in the complete callback function
//			feedback = HAL_UART_Transmit(huart_log, (uint8_t *)send_to_uart, strlen(send_to_uart), HAL_MAX_DELAY);
//			if(feedback == HAL_ERROR)
//				printf("Error !\n");

			sprintf(send_to_hc05,"%.5lf,%.5lf\n",GPGGA_decoded.latitude,GPGGA_decoded.longitude);

			feedback = HAL_UART_Transmit(hc05, (uint8_t *)send_to_hc05, sizeof(send_to_hc05), HAL_MAX_DELAY);
//			HAL_UART_Abort(&huart5);
//			feedback = HAL_UART_Transmit_DMA(hc05, (uint8_t *)send_to_hc05, sizeof(send_to_hc05));
			if(feedback == HAL_ERROR)
				printf("Error !\n");

			printf("%s\n",send_to_hc05);

//			HAL_UART_Abort(&huart5);
//			feedback = HAL_UARTEx_ReceiveToIdle_DMA(&huart4, (uint8_t *) RxBuf_gps, sizeof(RxBuf_gps));
//				if(feedback != HAL_OK)
//					Error_Handler();
		}
	}
}

void GGA_Frame_Decode(char *frame){

	char check[3] = "\0", crc[3]="\0";
	uint8_t index = 0, checksum = 0;
	char copy[strlen(frame)];
	strcpy(copy, frame);//strtok modifie la chaine donc on utilisera une copie
	char *split = strtok(copy, ",");//diviser une chaine en divisions par une virgule en separateur

	while(split != NULL){
		switch(index){
			case 1: // Heure
				if (strcmp(split, "") != 0) {
					int time = atoi(split);
					GPGGA_decoded.hour = time/10000;
					GPGGA_decoded.minute = (time/100) % 100;
					GPGGA_decoded.second = time % 100;
				}
				break;
			case 2: // Latitude
				if (strcmp(split, "") != 0) {
					int lat_degrees = atof(split)/100;
					double lat_minutes = atof(split)-lat_degrees*100;
					GPGGA_decoded.latitude = lat_degrees + lat_minutes/60.0;
	            }
				break;
	        case 3: // Latitude hemisphere
	        	if (strcmp(split, "S") == 0)
	        		GPGGA_decoded.latitude = -GPGGA_decoded.latitude;
	        	break;
	        case 4: // Longitude
	        	if (strcmp(split, "") != 0) {
	        		int lon_degrees = atof(split)/100;//confert string to float
	        		double lon_minutes = atof(split)-lon_degrees*100;//confert string to integer
	        		GPGGA_decoded.longitude = lon_degrees + lon_minutes/60.0;
	            }
	        	break;
	        case 5: // Longitude hemisphere
	        	if (strcmp(split, "W") == 0)
	        		GPGGA_decoded.longitude = -GPGGA_decoded.longitude;
	            break;
	        case 6: // Fix quality
	        	GPGGA_decoded.fix_quality = atoi(split);
	        	break;
	        case 7: // Satellites in use
	        	GPGGA_decoded.satellites = atoi(split);
	        	break;
	        case 9: // Altitude
	        	GPGGA_decoded.altitude = atof(split);
	        	break;
	        }
	        split = strtok(NULL, ",");//go to next division
	        index++;
	    }

	    //Calcul du CRC (checksum) -> XOR octet par octet entre $ et * sans les inclures
	    index=1;
	    while(frame[index] != '*'){
	        //printf("%c",frame[index]);
	        checksum ^= frame[index]; // xor
	        index++;
	    }
	    //printf("\ncalcul:0x%02X\n",checksum);//affichage en hexa
	    //sprintf(check, "%02X", checksum);
	    crc[0] = frame[index+1];
	    crc[1] = frame[index+2];
	    crc[2] = '\0';
	    //printf("crc %s\n",crc);
	    //uint8_t verif = atoi(crc);
	    //printf("crc0 %c, crc1 %c\n",crc[0],crc[1]);
	    //printf("verif : 0x%d\n",verif);//checksum en hexa a la fin de la trame NMEA
	    sprintf(check, "%X", checksum);
	    //printf("check 0x%s\n",check);

	    if(strcmp(check, crc)==0){//2 chaines identiques
	    	GPGGA_decoded.checksum = OK;
	    	//printf("Checksum OK : 0x%x\n", checksum);
	    }
	    else{
	    	GPGGA_decoded.checksum = KO;
	    	//printf("Erreur : checksum incorrect\n");
	    }
}
