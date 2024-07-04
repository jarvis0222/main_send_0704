#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  //Set OLED pin position

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

#define SERVICE_UUID        "fcf6993d-ea05-4940-92f1-c7746b70b8aa"
#define CHARACTERISTIC_UUID "5038e12d-498d-4763-b546-cc4f5b6f3ec1"
#include <Pixy2SPI_SS_ESP32.h>

Pixy2SPI_SS pixy;  //Declare pixy name

unsigned long startTime, finishTime, passTime;
bool piano = 0 , violin = 0 , trumpet = 0;
int signature = 0 , instrument = 0;
int Send , server_send = -1 , t = 0, coordinate_y = 0 , area = 0;
int ttt = 0;

void setup() {
  u8g2.begin();  //OLED start
  setup_BLE();
  Serial.begin(115200);
  pixy.init();  //pixy start
  Serial.println("pixy OK");
}

void loop() {
  if (instrument == 1) {
    if (piano == 0) {
      server_send = 30;  //Turn on the light(piano)
      BLE_loop();
      Serial.println("piano turn on");
      delay(1000);
      server_send = 0;  //Avoid values ​​being detected multiple times
      BLE_loop();
      Serial.println("send 0");
    }
    piano = 1;
    while (instrument != 5) {
      //Until seeing the yellow block, break out of the circle
      music_scale_piano();
    }
    server_send = 31;  //Turn off the light(piano)
    BLE_loop();
    Serial.println("piano turn off");
    piano = 0;
    delay(100);
    server_send = 0;  //Avoid values ​​being detected multiple times
    BLE_loop();
    Serial.println("send 0");
  }
  else if (instrument == 2) {
    if (violin == 0) {
      server_send = 20;  //Turn on the light(violin)
      BLE_loop();
      Serial.println("no.2 turn on");
      delay(1000);
      server_send = 0;  //Avoid values ​​being detected multiple times
      BLE_loop();
      Serial.println("send 0");
    }
    violin = 1;
    while (instrument != 5) {
      music_scale_violin();
    }
    server_send = 21;  //Turn off the light(violin)
    BLE_loop();
    Serial.println("violin turn off");
    violin = 0;
    delay(100);
    server_send = 0;  //Avoid values ​​being detected multiple times
    BLE_loop();
    Serial.println("send 0");
  }
  else if (instrument == 3) {
    if (trumpet == 0) {
      server_send = 10;  //(trumpet)Turn on the light
      BLE_loop();
      Serial.println("trumpet turn on");
      delay(1000);
      server_send = 0;  //Avoid values ​​being detected multiple times
      BLE_loop();
      Serial.println("send 0");
    }
    trumpet = 1;
    while  (instrument != 5) {
      music_scale_trumpet();
    }
    server_send = 11;    //.................(trumpet)Turn off the light
    BLE_loop();
    Serial.println("trumpet turn off");
    trumpet = 0;
    delay(100);
    server_send = 0;
    BLE_loop();
    Serial.println("send 0");
  }
  //Ensemble mode.....................................................................................
  else if (instrument == 4) {
    server_send = 100;  //Play Canan together
    BLE_loop();
    Serial.println("send 100");
    delay(100);
    Serial.println("canon start");
    while (1) {
      pixy.ccc.getBlocks();
      if (pixy.ccc.blocks[0].m_signature == 5) {
        while (pixy.ccc.numBlocks > 0 && ttt <= 100) {
          pixy.ccc.getBlocks();
          ttt += 1;
          delay(1);
        }
        if (ttt > 100) {
          Serial.print("break");
          delay(2000);
          server_send = 200;  //Play pop music together
          BLE_loop();
          Serial.println("send 200");
          delay(100);
          server_send = 0;  //Avoid values ​​being detected multiple times
          BLE_loop();
          Serial.println("send 0");
          instrument = 0;
        }
        ttt = 0;
      }
      else if (pixy.ccc.blocks[0].m_signature == 4) {
        Serial.println("canon in");
        play_canon();
      }
    }
  }
  else {
    color();
  }
}

//Detect color blocks to determine color.................................................................................
void color() {
  u8g2.firstPage();  //OLED update page
  pixy.ccc.getBlocks();
  signature = pixy.ccc.blocks[0].m_signature;
  if (pixy.ccc.numBlocks > 0 && signature == 1) {
    while (pixy.ccc.numBlocks > 0 && t <= 100) {
      //Wait for pixy to continue to detect the color block for a period of time,
      //then set the variable to the corresponding number.
      pixy.ccc.getBlocks();
      t += 1;
      delay(1);
    }
    if (t > 100) {
      instrument = 1;
      do
      {
        //OLED displays the corresponding number according to the variable
        //Set font, position, and displayed words
        u8g2.setFont(u8g2_font_logisoso54_tr);
        u8g2.setFontPosTop();
        u8g2.setCursor(50, 2);
        u8g2.print("1");
      } while (u8g2.nextPage());
    }
    t = 0;
  }
  else if (pixy.ccc.numBlocks > 0 && signature == 2) {
    while (pixy.ccc.numBlocks > 0 && t <= 100) {
      pixy.ccc.getBlocks();
      t += 1;
      delay(1);
    }
    if (t > 100) {
      instrument = 2;
      do
      {
        u8g2.setFont(u8g2_font_logisoso54_tr);
        u8g2.setFontPosTop();
        u8g2.setCursor(50, 2);
        u8g2.print("2");
      } while (u8g2.nextPage());
    }
    t = 0;
  }
  else if (pixy.ccc.numBlocks > 0 && signature == 3) {
    while (pixy.ccc.numBlocks > 0 && t <= 100) {
      pixy.ccc.getBlocks();
      t += 1;
      delay(1);
    }
    if (t > 100) {
      instrument = 3;
      do
      {
        u8g2.setFont(u8g2_font_logisoso54_tr);
        u8g2.setFontPosTop();
        u8g2.setCursor(50, 2);
        u8g2.print("3");
      } while (u8g2.nextPage());
    }
    t = 0;
  }
  else if (pixy.ccc.numBlocks > 0 && signature == 4) {
    while (pixy.ccc.numBlocks > 0 && t <= 150 ) {
      pixy.ccc.getBlocks();
      t += 1;
      delay(1);
    }
    if (t > 150) {
      instrument = 4;
      do
      {
        u8g2.setFont(u8g2_font_logisoso54_tr);
        u8g2.setFontPosTop();
        u8g2.setCursor(50, 2);
        u8g2.print("4");
      } while (u8g2.nextPage());
    }
    t = 0;
  }
  else if (pixy.ccc.numBlocks > 0 && signature == 5) {
    while (pixy.ccc.numBlocks > 0 && t <= 150 ) {
      pixy.ccc.getBlocks();
      t += 1;
      delay(1);
    }
    if (t > 150) {
      instrument = 5;
      do
      {
        u8g2.setFont(u8g2_font_logisoso54_tr);
        u8g2.setFontPosTop();
        u8g2.setCursor(50, 2);
        u8g2.print("5");
      } while (u8g2.nextPage());
    }
    t = 0;
  }
}
//Canon program..............................................................................................
void play_canon() {
  pixy.ccc.getBlocks();
  do {
    Serial.println("111111");
    pixy.ccc.getBlocks();
    startTime = millis();
  } while (pixy.ccc.numBlocks <= 0);
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks > 0) {
    while (pixy.ccc.numBlocks > 0) {
      pixy.ccc.getBlocks();
      finishTime = millis();
    }
  }
  if (finishTime > startTime && passTime != finishTime - startTime) {
    passTime = finishTime - startTime;
    Serial.print("start:");
    Serial.println(startTime);
    Serial.print("finish:");
    Serial.println(finishTime);
    Serial.print("------------passTime:");
    Serial.println(passTime);
    if (passTime > 0 && passTime <= 200) {
      //If the elapsed time is <0.2 seconds, the canon rhythm will become faster.
      if (Send != 41) {
        server_send = 41;
        BLE_loop();
        delay(1);
        Serial.println("send 41");
      }
      Send = 41;
    }
    else if (passTime > 200) {
      //If the elapsed time is >0.2 seconds, the canon rhythm will slow down.
      if (Send != 42) {
        server_send = 42;
        BLE_loop();
        delay(1);
        Serial.println("send 42");
      }
      Send = 42;
    }
    passTime = 0;
    t = 0;
  }
}
//music_scale(piano , violin , trumpet)...........................................................................
void music_scale_piano() {
  //Dividing the picture seen by pixy into 8 blocks,
  //corresponding to the musical scale.
  //By detecting the Y coordinates, the corresponding sound will be played
  //as the baton passes through these areas.
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks > 0 && pixy.ccc.blocks[0].m_width > 20 && pixy.ccc.blocks[0].m_signature == 1) {
    coordinate_y = pixy.ccc.blocks[0].m_y;
    if (coordinate_y > 0 && coordinate_y < 26) {
      if (area != 51) {
        server_send = 51;
        BLE_loop();
        delay(1);
        Serial.println("send 51");
      }
      area = 51;
    }
    else if (coordinate_y >= 26 && coordinate_y < 52) {
      if (area != 52) {
        server_send = 52;
        BLE_loop();
        delay(1);
        Serial.println("send 52");
      }
      area = 52;
    }
    else if (coordinate_y >= 52 && coordinate_y < 78) {
      if (area != 53) {
        server_send = 53;
        BLE_loop();
        delay(1);
        Serial.println("send 53");
      }
      area = 53;
    }
    else if (coordinate_y >= 78 && coordinate_y < 104) {
      if (area != 54) {
        server_send = 54;
        BLE_loop();
        delay(1);
        Serial.println("send 54");
      }
      area = 54;
    }
    else if (coordinate_y >= 104 && coordinate_y < 130 && pixy.ccc.blocks[0].m_signature == 1) {
      if (area != 55) {
        server_send = 55;
        BLE_loop();
        delay(1);
        Serial.println("send 55");
      }
      area = 55;
    }
    else if (coordinate_y >= 130 && coordinate_y < 156 && pixy.ccc.blocks[0].m_signature == 1) {
      if (area != 56) {
        server_send = 56;
        BLE_loop();
        delay(1);
        Serial.println("send 56");
      }
      area = 56;
    }
    else if (coordinate_y >= 156 && coordinate_y < 182 && pixy.ccc.blocks[0].m_signature == 1) {
      if (area != 57) {
        server_send = 57;
        BLE_loop();
        delay(1);
        Serial.println("send 57");
      }
      area = 57;
    }
    else if (coordinate_y >= 182 && coordinate_y < 210 && pixy.ccc.blocks[0].m_signature == 1) {
      if (area != 58) {
        server_send = 58;
        BLE_loop();
        delay(1);
        Serial.println("send 58");
      }
      area = 58;
    }
    delay(10);
  }
  else {
    //When a yellow color patch is detected, stop the scale program
    pixy.ccc.getBlocks();
    if (pixy.ccc.blocks[0].m_width > 100 && pixy.ccc.blocks[0].m_signature == 5) {
      instrument = 5;
    }
  }
}

void music_scale_violin() {
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks > 0 && pixy.ccc.blocks[0].m_width > 20  && pixy.ccc.blocks[0].m_signature == 2) {
    coordinate_y = pixy.ccc.blocks[0].m_y;
    if (coordinate_y > 0 && coordinate_y < 26) {
      if (area != 61) {
        server_send = 61;
        BLE_loop();
        delay(1);
        Serial.println("send 61");
      }
      area = 61;
    }
    else if (coordinate_y >= 26 && coordinate_y < 52) {
      if (area != 62) {
        server_send = 62;
        BLE_loop();
        delay(1);
        Serial.println("send 62");
      }
      area = 62;
    }
    else if (coordinate_y >= 52 && coordinate_y < 78) {
      if (area != 63) {
        server_send = 63;
        BLE_loop();
        delay(1);
        Serial.println("send 63");
      }
      area = 63;
    }
    else if (coordinate_y >= 78 && coordinate_y < 104) {
      if (area != 64) {
        server_send = 64;
        BLE_loop();
        delay(1);
        Serial.println("send 64");
      }
      area = 64;
    }
    else if (coordinate_y >= 104 && coordinate_y < 130) {
      if (area != 65) {
        server_send = 65;
        BLE_loop();
        delay(1);
        Serial.println("send 65");
      }
      area = 65;
    }
    else if (coordinate_y >= 130 && coordinate_y < 156) {
      if (area != 66) {
        server_send = 66;
        BLE_loop();
        delay(1);
        Serial.println("send 66");
      }
      area = 66;
    }
    else if (coordinate_y >= 156 && coordinate_y < 182) {
      if (area != 67) {
        server_send = 67;
        BLE_loop();
        delay(1);
        Serial.println("send 67");
      }
      area = 67;
    }
    else if (coordinate_y >= 182 && coordinate_y < 210) {
      if (area != 68) {
        server_send = 68;
        BLE_loop();
        delay(1);
        Serial.println("send 68");
      }
      area = 68;
    }
    delay(10);
  }
  else {
    pixy.ccc.getBlocks();
    if (pixy.ccc.blocks[0].m_width > 100 && pixy.ccc.blocks[0].m_signature == 5) {
      instrument = 5;
    }
  }
}
void music_scale_trumpet() {
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks > 0 && pixy.ccc.blocks[0].m_width > 20 && pixy.ccc.blocks[0].m_signature == 3) {
    coordinate_y = pixy.ccc.blocks[0].m_y;
    if (coordinate_y > 0 && coordinate_y < 26) {
      if (area != 71) {
        server_send = 71;
        BLE_loop();
        delay(1);
        Serial.println("send 71");
      }
      area = 71;
    }
    else if (coordinate_y >= 26 && coordinate_y < 52) {
      if (area != 72) {
        server_send = 72;
        BLE_loop();
        delay(1);
        Serial.println("send 72");
      }
      area = 72;
    }
    else if (coordinate_y >= 52 && coordinate_y < 78) {
      if (area != 73) {
        server_send = 73;
        BLE_loop();
        delay(1);
        Serial.println("send 73");
      }
      area = 73;
    }
    else if (coordinate_y >= 78 && coordinate_y < 104) {
      if (area != 74) {
        server_send = 74;
        BLE_loop();
        delay(1);
        Serial.println("send 74");
      }
      area = 74;
    }
    else if (coordinate_y >= 104 && coordinate_y < 130) {
      if (area != 75) {
        server_send = 75;
        BLE_loop();
        delay(1);
        Serial.println("send 75");
      }
      area = 75;
    }
    else if (coordinate_y >= 130 && coordinate_y < 156) {
      if (area != 76) {
        server_send = 76;
        BLE_loop();
        delay(1);
        Serial.println("send 76");
      }
      area = 76;
    }
    else if (coordinate_y >= 156 && coordinate_y < 182) {
      if (area != 77) {
        server_send = 77;
        BLE_loop();
        delay(1);
        Serial.println("send 77");
      }
      area = 77;
    }
    else if (coordinate_y >= 182 && coordinate_y < 210) {
      if (area != 78) {
        server_send = 78;
        BLE_loop();
        delay(1);
        Serial.println("send 78");
      }
      area = 78;
    }
    delay(10);
  }
  else {
    pixy.ccc.getBlocks();
    if (pixy.ccc.blocks[0].m_width > 100 && pixy.ccc.blocks[0].m_signature == 5) {
      instrument = 5;
    }
  }
}
