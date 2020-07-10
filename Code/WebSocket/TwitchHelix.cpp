/* Project : Twitch/Arduino Connection
 * Version : 0.1 (dev) 
 * Developper : Lazare (twitch.tv/lazarelive)
 * With the help of Akanoa, ghostwake, Nesquiik69
 * May 2020
 * 
 * Class : TwitchHelix
 * Allow user to connect to the Twitch Helix API using a ESP8266
 * Webhooks tried but not working for now using this technology
 * Modify connexions.h to get your data working
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "Connections.h"
#include "TwitchHelix.h"

TwitchHelix::TwitchHelix(void) {
}

//OAuth2 requestion function
byte TwitchHelix::requestOAuth(void) {
  HTTPClient client;
  
  byte _status = _HELIX_NO_CHANGE;

  //Prepare parameters for the HTTP:POST
  String postRequest = "?client_id=" + String(TWITCH_CLIENT_ID) +
                       "&client_secret=" + String(TWITCH_CLIENT_SECRET) +
                       "&grant_type=client_credentials"; //+
                       //"&scope=channel:read:subscriptions+bits:read";

  //Prepare connection to client
  client.begin(_URLTwitchID, 443, _URIOAuthToken + postRequest, true, _fingerprintID);

  //Execute POST function
  int httpCode = client.POST("");
  String payload = client.getString();

  //Deserialize if return from Twitch is OK
  if (httpCode == 200) {
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(5) + payload.length());
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
      _status = _HELIX_ERROR_PARSING;
    else {
      String result = doc["access_token"].as<String>();
      if(_OAuthKey != result) {
        _status = _HELIX_CHANGED;
        _OAuthKey = result;
      }
    }
  } else _status = _HELIX_ERROR_WEB;
    
  client.end();
  return _status;
}

byte TwitchHelix::revokeOAuth(void) {
  HTTPClient client;
    
  byte _status = _HELIX_CHANGED;
  
  //Prepare parameters for the HTTP:POST
  String postRequest = "?client_id=" + String(TWITCH_CLIENT_ID) +
                       "&client_secret=" + String(TWITCH_CLIENT_SECRET) +
                       "&token=" + _OAuthKey;

  //Prepare connection to client
  client.begin(_URLTwitchID, 443, _URIOAuthRevoke + postRequest, true, _fingerprintID);
  
  int httpCode = client.POST("");

  if (httpCode==200)  _OAuthKey = "";
  else                _status = _HELIX_ERROR_WEB;

  client.end();
  return _status;
}

byte TwitchHelix::validateOAuth(void) {
  HTTPClient client;  
  byte _status = _HELIX_CHANGED;

  //Prepare connection to client
  client.begin(_URLTwitchID, 443, _URIOAuthValidate, true, _fingerprintID);
  client.addHeader("Client-ID", TWITCH_CLIENT_ID);
  client.addHeader("Authorization", "OAuth " + _OAuthKey);  
  
  int httpCode = client.GET();
  String payload = client.getString();

  if (httpCode == 200) {
    DynamicJsonDocument doc(JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(9) + payload.length());
    DeserializationError error = deserializeJson(doc, payload);
   if (error)
      _status = _HELIX_ERROR_PARSING;
    else {
      Serial.print(payload);
    }
  } else if (httpCode == 401) _status = _HELIX_ERROR_OAUTH;
    else                      _status = _HELIX_ERROR_WEB;
 
  client.end();
  return _status;
}

byte TwitchHelix::refreshOAuth(void) {
  byte _status = _HELIX_NO_CHANGE;

  _status = revokeOAuth();
  _status = requestOAuth();
  
  return _status;
}

byte TwitchHelix::requestUserID(void) {
  HTTPClient client;  
  byte _status = _HELIX_NO_CHANGE;

  //Prepare parameters for the HTTP:GET
  String getRequest = "?login=" + String(TWITCH_LOGIN);

  //Prepare connection to client
  client.begin(_URLTwitchAPI, 443, _URIUser + getRequest, true, _fingerprintAPI);
  client.addHeader("Client-ID", TWITCH_CLIENT_ID);
  client.addHeader("Authorization", "Bearer " + _OAuthKey);
  
  int httpCode = client.GET();
  String payload = client.getString();

  if (httpCode == 200) {
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(9) + payload.length());
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
      _status = _HELIX_ERROR_PARSING;
    else {
      String result = doc["data"][0]["id"].as<String>();
      if (_TwitchUserID != result) {
        _status = _HELIX_CHANGED;
        _TwitchUserID = result;
      }
    }
  } else if (httpCode == 401) _status = _HELIX_ERROR_OAUTH;
    else                      _status = _HELIX_ERROR_WEB;

  client.end();
  return _status;  
}

byte TwitchHelix::requestStreamInformations(void) {
  HTTPClient client;  
  byte _status = _HELIX_NO_CHANGE;

  //Prepare parameters for the HTTP:GET
  //String getRequest = "?user_id=" + _TwitchUserID + "&first=1";

  //Prepare connection to client
  client.begin(_URLTwitchAPI, 443, _URIStreams /*+ getRequest*/, true, _fingerprintAPI);
  client.addHeader("Client-ID", TWITCH_CLIENT_ID);
  client.addHeader("Authorization", "Bearer " + _OAuthKey);
  
  int httpCode = client.GET();
  String payload = client.getString();

  if (httpCode == 200) {
    DynamicJsonDocument doc(payload.length() + JSON_OBJECT_SIZE(18));
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
      _status = _HELIX_ERROR_PARSING;
    else {
      _TwitchStreamTitle = doc["data"][0]["title"].as<String>();
      _TwitchStreamLive = (doc["data"][0]["type"].as<String>() == "live");
      _TwitchViewerCount = doc["data"][0]["viewer_count"].as<int>();
      } 
      Serial.println(payload);
  } else if (httpCode == 401) _status = _HELIX_ERROR_OAUTH;
    else                      _status = _HELIX_ERROR_WEB;
    
  client.end();
  return _status;  
}

void TwitchHelix::webhookSubscribeFollower(void) {
  HTTPClient client;
  client.begin(_URLTwitchAPI, 443, _URIWebhook, true, _fingerprintAPI);
  client.addHeader("Client-ID", TWITCH_CLIENT_ID);
  client.addHeader("Authorization", "Bearer " + _OAuthKey);
  client.addHeader("Content-Type", "application/json");

  //TODO : Passage sur ArduinoJson
  String postContent = "{\"hub.callback\":\"" + String(WEBHOOK_CALLBACK) + _webhookURIFollow + "\",\"hub.mode\":\"subscribe\",\"hub.topic\":\"" +
                       String("https://api.twitch.tv/helix/users/follows?first=1&to_id=" + _TwitchUserID) +
                       "\",\"hub.lease_seconds\":6000}";

                    
  int httpCode = client.POST(postContent);
  String payload = client.getString();

  if(httpCode == 202)
    Serial.println("Webhook Follow Attempted");
  else
    Serial.println(payload);    
  
  client.end();
}

void TwitchHelix::webhookSubscribeStream(void) {
  HTTPClient client;
  client.begin(_URLTwitchAPI, 443, _URIWebhook, true, _fingerprintAPI);
  client.addHeader("Client-ID", TWITCH_CLIENT_ID);
  client.addHeader("Authorization", "Bearer " + _OAuthKey);
  client.addHeader("Content-Type", "application/json");

  //TODO : Passage sur ArduinoJson
  String postContent = "{\"hub.callback\":\"" + String(WEBHOOK_CALLBACK) + _webhookURIStream + "\",\"hub.mode\":\"subscribe\",\"hub.topic\":\"" +
                       String("https://api.twitch.tv/helix/streams?user_id=" + _TwitchUserID) +
                       "\",\"hub.lease_seconds\":6000}";

                    
  int httpCode = client.POST(postContent);
  String payload = client.getString();

  if(httpCode == 202)
    Serial.println("Webhook Stream Attempted");
  else
    Serial.println(payload);    
  
  client.end();
}
