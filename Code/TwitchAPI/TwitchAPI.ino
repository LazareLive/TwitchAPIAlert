#define  EVT_SIZE   10

#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Animation.h"
#include "LEDMatrixHandler.h"
#include <Wire.h>

String evtList[EVT_SIZE]; 
byte   evtPosition;
byte   evtLastRead;

unsigned int chatPoints;

void setup() {
  mx.begin();
  resetMatrix();
  
  Wire.begin(0x50);
  Wire.setClock(100000);
  Wire.onReceive(receiveEvent);

  Serial.begin(115200);
  
  callMatrixViewers(chatPoints);
}

void loop() {
  int pos, value;
  String person;

  if(evtLastRead != evtPosition) {
    //Serial.println(evtList[evtLastRead][0]);
    switch(evtList[evtLastRead][0]) {
      case 'A' :
        callMatrixSpace();
        chatPoints += 50;
        break;
      case 'B':
        pos = evtList[evtLastRead].indexOf(';');
        person = evtList[evtLastRead].substring(1, pos);
        value = evtList[evtLastRead].substring(pos + 1).toInt();
        callMatrixBits(person, value);
        chatPoints += (value * 10);
        break;
      case 'F' :
        callMatrixFollow(evtList[evtLastRead].substring(1));
        chatPoints += 10;
        break;
      case 'G' :
        callMatrixGift(evtList[evtLastRead].substring(1));
        chatPoints += 100;
        break;
      case 'g' :
        callMatrixGift(evtList[evtLastRead].substring(1));
        chatPoints += 100;
        break;
      case 'h' :
        callMatrixGift("???");
        chatPoints += 100;
        break;
      case 'H' :
        pos = evtList[evtLastRead].indexOf(';');
        person = evtList[evtLastRead].substring(1, pos);
        value = evtList[evtLastRead].substring(pos + 1).toInt();
        callMatrixHost(person, value);    
        chatPoints += (value * 20);
        break;
      case 'M' :
        callMatrixMessage(evtList[evtLastRead].substring(1));
        chatPoints += 25;
        break;
      case 'R' :
        pos = evtList[evtLastRead].indexOf(';');
        person = evtList[evtLastRead].substring(1, pos);
        value = evtList[evtLastRead].substring(pos + 1).toInt();
        callMatrixRaid(person, value);     
        chatPoints += (value * 10);
        break;      
      case 'S' :
        callMatrixSub(evtList[evtLastRead].substring(1));
        chatPoints += 100;
        break;
    }
    evtLastRead = (evtLastRead + 1) % EVT_SIZE;
    callMatrixViewers(chatPoints);
  }
}

void receiveEvent(int noct) {
  char c;
  
  while (Wire.available() > 0) {
    c = Wire.read();

    if (c == '\n') {
      evtPosition = (evtPosition + 1) % EVT_SIZE;
      evtList[evtPosition] = "";
    } else if (c != 0) {
      evtList[evtPosition] += c;
    } 
  }
}
