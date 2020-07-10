#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "Connections.h"
#include "TwitchHelix.h"
#include <bot_credentials.h>
#include <memory_constraints.h>
#include <twitch_irc_api.h>
#include "UTF8.h"

//TwitchAPI handler
TwitchHelix Helix = TwitchHelix();

#include "WebServerHandler.h"

//Twitch Api Instance
TwitchWebsocketIRC twitch_api;

//Function to connect WiFi
bool WiFiInitialize(void) {
  byte WiFiTimeout = 0;

  WiFi.disconnect();                    //Disconnect if already connected
  WiFi.mode(WIFI_STA);                  //Mode WiFi Station
  WiFi.begin(ESP_LOGIN, ESP_PASSWORD);  //Connect to WiFi

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    WiFiTimeout++;
    if (WiFiTimeout > _ESP_TIMEOUT) {
      WiFi.disconnect();
      return false;
    }
  }
  Serial.println("Connected to WiFi");
  return true;
}

inline void sendWireData(String datum) {
  //datum += "\n";
  String ascii = utf8ascii(datum + "\n");
  for (int i = 0; i <= (ascii.length() / 32); i++) {
    Wire.beginTransmission(0x50);
    for (int j = 0; j < 32; j++)
      Wire.write(ascii[j + (i * 32)]);
    Wire.endTransmission();
    delay(50);
  }
  Serial.print(datum);
}

/* Custom Callbacks */
void onBitsCallback(PrivMsg data) {
  String datum = "B" + String(data.display_name) + ";" + String(data.bits);
  sendWireData(datum);
}

void onSubCallback(UserNotice data) {
  String datum = "S" + String(data.display_name);
  sendWireData(datum);
}

void onReSubCallback(UserNotice data) {
  String datum = "S" + String(data.display_name);
  sendWireData(datum);
}

void onSubGiftCallback(UserNotice data) {
  String datum = "G" + String(data.msg_param_recipient_display_name);
  sendWireData(datum);
}

void onAnonSubGiftCallback(UserNotice data) {
  String datum = "g" + String(data.msg_param_recipient_display_name);
  sendWireData(datum);
}

void onSubMysteryGiftCallback(UserNotice data) {
  String datum = "h";
  sendWireData(datum);
}

void onCustomRewardCallback(PrivMsg data) {
  if (strcmp(data.custom_reward_id, "0764a5ce-43f2-4361-8424-a2ac7209a6d0") == 0) {
    String datum = "A";
    sendWireData(datum);
  }
}

void onChannelHostedCallback(Hostee data) {
  String datum = "H" + String(data.display_name) + ";" + String(data.viewer_count);
  sendWireData(datum);
}

void onRaidCallback(UserNotice data) {
  String datum = "R" + String(data.display_name) + ";" + String(data.msg_param_viewerCount);
  sendWireData(datum);
}

void onHighlightedMsgCallback(PrivMsg data) {
  String datum = "M" + String(data.message);
  sendWireData(datum);
}

void setup() {
  //Debug mode
  Serial.begin(115200);
  Serial.println();

  Wire.begin(D6, D7);
  Wire.setClock(100000);

  if (WiFiInitialize()) {
    WebServerInitialize();

    Helix.requestOAuth();
    Helix.requestUserID();
    Helix.webhookSubscribeFollower();
    //Helix.webhookSubscribeStream();
    delay(100);

    //TwitchAPI connection & upload data
    twitch_api.connect_to_twitch_websocket();

    //Register callbacks
    twitch_api.onBits(onBitsCallback);
    twitch_api.onSub(onSubCallback);
    twitch_api.onReSub(onReSubCallback);
    twitch_api.onSubGift(onSubGiftCallback);
    twitch_api.onAnonSubGift(onAnonSubGiftCallback);
    twitch_api.onSubMysteryGift(onSubMysteryGiftCallback);
    twitch_api.onCustomReward(onCustomRewardCallback);
    twitch_api.onHighlightedMsg(onHighlightedMsgCallback);
    twitch_api.onChannelHosted(onChannelHostedCallback);
    twitch_api.onRaid(onRaidCallback);
  }

  //Serial.println("\n>>> Initialization done...\n");
}

void loop() {
  //Checking WiFi connection
  if (WiFi.status() == WL_CONNECTED) {
    if (twitch_api.available())
      twitch_api.poll();

    WebServer.handleClient();
  } else WiFiInitialize();
  delay(100);
}
