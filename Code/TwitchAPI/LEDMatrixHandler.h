#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 16

#define CLK_PIN   13  // or SCK
#define DATA_PIN  11  // or MOSI
#define CS_PIN    10  // or SS

//Objet de contrôle de la matrice de LEDs
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);  

//Buffer de scroll
byte  _mxHighBuffer[FRAME_LENGTH];
byte  _mxLowBuffer[FRAME_LENGTH];

byte  _mxTextBuffer[MAX_BUFFER_SIZE];
int   _mxTextBufferSize;

void prepareMatrixAnimation(const byte anim[][FRAME_LENGTH], byte frame) {
  for(int i=0; i<FRAME_LENGTH; i++) {
    _mxHighBuffer[i] = pgm_read_byte_near(anim[(frame<<1) + 0] + i);
    _mxLowBuffer[i]  = pgm_read_byte_near(anim[(frame<<1) + 1] + i);
  }
}

void prepareMatrixTextBuffer(String mesg) {
  int   i, j, pos;
  byte  letter;

  pos = 0;
  for (i=0; i<mesg.length(); i++) {
    letter = (byte)mesg[i];
    Serial.println(letter);
    for(j=0; j<4; j++) {
      if (pgm_read_byte_near(mxCharset[letter]+ j) > 0)
        _mxTextBuffer[pos++] = pgm_read_byte_near(mxCharset[letter] + j);
    }
      
    if ((letter == 'M') || (letter == 'T') || (letter == 'W') || (letter == 'X') || (letter == 'm') || (letter == 'w') || (letter == 'x'))
      _mxTextBuffer[pos++] = pgm_read_byte_near(mxCharset[letter]);

    if (letter == ' ') {
      for(j=0; j<2; j++)
        _mxTextBuffer[pos++] = 0;
    }

    _mxTextBuffer[pos++] = 0x00;

    _mxTextBufferSize = pos - 1;
    if (pos > MAX_BUFFER_SIZE)
      return;
  }
}

inline byte frameText(void) {
  return max(_mxTextBufferSize - (FRAME_LENGTH - 4), 0);
}

void prepareMatrixText(byte offset) {
  prepareMatrixAnimation(mxAnimSub, 3);
  //No scroll
  if (_mxTextBufferSize < (FRAME_LENGTH - 4)) {
    for(int i=0; i<_mxTextBufferSize; i++) {
      _mxHighBuffer[i + ((FRAME_LENGTH - _mxTextBufferSize)>>1)] += (_mxTextBuffer[i] >> 4);
      _mxLowBuffer[i + ((FRAME_LENGTH - _mxTextBufferSize)>>1)] += (_mxTextBuffer[i] << 4);
    }
  } else {
  //Scroll
    for(int i=0; i<(FRAME_LENGTH - 4); i++) {
      _mxHighBuffer[i+2] += (_mxTextBuffer[i + offset] >> 4);
      _mxLowBuffer[i+2] += (_mxTextBuffer[i + offset] << 4);
    }
  }
}

void prepareMatrixViewers(void) {
  prepareMatrixAnimation(mxStaticVues, 0);
  //No scroll{
  int offset = ((FRAME_LENGTH - _mxTextBufferSize)>>1) + 8;
  for(int i=0; i<_mxTextBufferSize; i++) {
    _mxHighBuffer[i + offset] += (_mxTextBuffer[i] >> 4);
    _mxLowBuffer[i + offset] += (_mxTextBuffer[i] << 4);
  }
}

inline byte matrixColumnTransform(byte i) {
  return ((i&0xF8)+(7-(i&0x07)));
}

void showMatrixDisplay(void) {
  for(int i=0; i<FRAME_LENGTH; i++) {
    mx.setColumn(matrixColumnTransform(i), _mxHighBuffer[i]);
    mx.setColumn(matrixColumnTransform(i + FRAME_LENGTH), _mxLowBuffer[i]);
  }
}

inline void displayMatrixAnimation(const byte anim[][FRAME_LENGTH], byte frames, byte loops, int del) {
  byte l, f;
  for(l=0; l<loops; l++) {
    for(int f=0; f<frames; f++) {
      prepareMatrixAnimation(anim, f);
      showMatrixDisplay();
      delay(del);
    }
  }
}

inline void displayMatrixText(String text, int preDel, int postDel) {
  int text_size, scroll;
    
  prepareMatrixTextBuffer(text);
  text_size = frameText();
  if (text_size > 0) {
    prepareMatrixText(0);
    showMatrixDisplay();
    delay(preDel);
    for(scroll=0; scroll<=text_size; scroll++) {
      prepareMatrixText(scroll);
      showMatrixDisplay();
    }
  } else {
    prepareMatrixText(0);
    showMatrixDisplay();  
  }
  delay(postDel);    
}

void callMatrixSub(String user) {
  displayMatrixAnimation(mxAnimSub, 4, 3, 150);
  displayMatrixText(user, 1000, 2000);
  mx.clear();
}

void callMatrixBits(String user, int amount) {
  displayMatrixAnimation(mxAnimCheer, 4, 5, 50);
  displayMatrixText(String(amount) + " V", 1000, 2000);
  displayMatrixText(user, 1000, 2000);
  mx.clear();
}

void callMatrixFollow(String user) {
  displayMatrixAnimation(mxAnimFollow, 4, 1, 500);
  displayMatrixText(user, 1000, 2000);
  mx.clear();
}

void callMatrixGift(String user) {
  displayMatrixAnimation(mxAnimGift, 4, 5, 50);
  displayMatrixText(user, 1000, 2000);
  mx.clear();
}

void callMatrixViewers(int views) {
  prepareMatrixTextBuffer(String(views));
  prepareMatrixViewers();
  showMatrixDisplay();
}

void callMatrixMessage(String message) {
  displayMatrixAnimation(mxAnimMessage, 4, 1, 150);
  delay(1000);
  displayMatrixText(message, 1000, 2000);
  mx.clear();
}

void callMatrixSpace(void) {
  displayMatrixAnimation(mxAnimSpaceInvader, 7, 1, 150);
  prepareMatrixAnimation(mxAnimSpaceInvader, 0);
  showMatrixDisplay();
  delay(1000);
  mx.clear();
}

void callMatrixHost(String hoster, int views) {
  displayMatrixAnimation(mxAnimHost, 2, 5, 100);
  displayMatrixText(hoster, 1000, 2000);
  displayMatrixText(String(views), 1000, 2000);
  mx.clear();
}

void callMatrixRaid(String raider, int views) {
  displayMatrixAnimation(mxAnimRaid, 2, 5, 100);
  displayMatrixText(raider, 1000, 2000);
  displayMatrixText(String(views), 1000, 2000);
  mx.clear();
}

void resetMatrix(void) {
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
}
