//
//#include <stdint.h>
//#include <stdbool.h>
//#include "ESP.h"
//#include "uart_helper.h"
//
//#define readBufferSize 100
//
//char transmitBuffer[15];
//uint8_t buildBuffer[15];
//volatile char readBuffer[readBufferSize];
//uint8_t readBufferIndex;
//uint8_t buildBufferIndex;
//volatile uint8_t readBufferLength;
//volatile int bytesReading = -1;
//
//void Wifi_initialize() {
//    bytesReading = -1;
//    readBufferIndex = 0;
//    readBufferLength = 0;
//}
//
//void Wifi_readIntoBuffer() {
//    volatile int length = 0;
//    volatile char* read = ESP_GetString(&length);
//
//    for(int i = 0; i < length; i++) {
//        readBuffer[(readBufferIndex + i) % readBufferSize] = read[i];
//    }
//    readBufferLength += length;
//}
//
//int lastReading = 0;
//void Wifi_handler() {
//    Wifi_readIntoBuffer();
//
//    if(bytesReading == 0) {
//        if(buildBuffer[0] == 1) {
//            primaryR = buildBuffer[1];
//            primaryG = buildBuffer[2];
//            primaryB = buildBuffer[3];
//            secondaryR = buildBuffer[4];
//            secondaryG = buildBuffer[5];
//            secondaryB = buildBuffer[6];
//        } else if(buildBuffer[0] == 2) {
//            fontIndex = buildBuffer[1];
//        } else if(buildBuffer[0] == 3) {
//            bladeEffectIndex = buildBuffer[1];
//        } else if(buildBuffer[0] == 4) {
//            Wifi_sendSettings(primaryR, primaryG, primaryB, secondaryR, secondaryG, secondaryB, fontIndex, bladeEffectIndex);
//        }
//        bytesReading = -1;
//    } else if(bytesReading > 0) {
//        //  in middle of reading packet
//        while(bytesReading > 0 && readBufferLength > 0) {
//            buildBuffer[buildBufferIndex++] = readBuffer[readBufferIndex];
//            readBufferIndex = (readBufferIndex + 1) % readBufferSize;
//            bytesReading--; readBufferLength--;
//
//            lastReading = millis();
//        }
//
//        if(millis() - lastReading > 500) bytesReading = -1;
//    } else {
//        //  awaiting header
//        if(readBufferLength > 0) {
//            //  read header
//            buildBuffer[0] = readBuffer[readBufferIndex];
//            buildBufferIndex = 1;
//            readBufferIndex = (readBufferIndex + 1) % readBufferSize;
//
//            if(buildBuffer[0] == 1) bytesReading = 6;
//            else if(buildBuffer[0] == 2) bytesReading = 1;
//            else if(buildBuffer[0] == 3) bytesReading = 1;
//            else if(buildBuffer[0] == 4) bytesReading = 0;
//            else readBufferLength = 0;  //  throw away
//
//            lastReading = millis();
//        }
//    }
//
//}
