# GPS_Coordinates_Acquisitions
Get the GPS coordinates on a mobile App from a GPS module controlled by a microcontroller

Utilisation of an NEO-6M GPS module controlled by an STM32 transmitting the position continuously by Bluetooth with an HC-06 device. Reception of the BT data on a smartphone, printing and saving of the position on a mobile App using Google MAP's API coded in JAVA on Android Studio

## Pinout :

  *  UART4 Tx  PF7 - NEO 6M GPS Rx
      Rx  PE7 - NEO 6M GPS Tx

  *  UART5 Tx  PC12 - HC-05 BT Rx
      Rx  PD2 - HC-05 BT Tx

UART4 & UART5 115200 bauds with DMA transmistions and receptions
D and I caches enabled
