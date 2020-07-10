ESP8266WebServer WebServer(52810);
int _viewers;

//Function to handle WebServer response to Twitch follow event
void WebServer_onFollowEvent(void) { 
  //Twitch follow event > Validation
  if (WebServer.arg("hub.challenge") != false) {
    WebServer.send(200, "text/plain", WebServer.arg("hub.challenge"));
    Serial.println("Follow Key Recieved");
  }

  //Recieve follow event
  else if (WebServer.arg("plain") != false) {
    WebServer.send(200, "text/plain", "");
    //Parsing element
    DynamicJsonDocument doc(JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(9) + WebServer.arg("plain").length());
    DeserializationError error = deserializeJson(doc, WebServer.arg("plain"));
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    //callMatrixFollow(doc["data"][0]["from_name"].as<String>());
    //pinballPoints += 10;
    //callMatrixViewers(pinballPoints);
    String data = doc["data"][0]["from_name"].as<String>();
    Wire.beginTransmission(0x50);
    Wire.write('F');
    Wire.write(data.c_str());
    Wire.endTransmission();   
  }
  else WebServer.send(200, "text/plain", "Invalid request to server. Try again.");
}

//Function to handle WebServer response to Twitch stream event
void WebServer_onStreamEvent(void) { 
  //Twitch follow event > Validation
  if (WebServer.arg("hub.challenge") != false) {
    WebServer.send(200, "text/plain", WebServer.arg("hub.challenge"));
    Serial.println("Stream Key Recieved");
  }

  //Recieve follow event
  else if (WebServer.arg("plain") != false) {
    WebServer.send(200, "text/plain", "");
    //Parsing element
    DynamicJsonDocument doc(JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(9) + WebServer.arg("plain").length());
    DeserializationError error = deserializeJson(doc, WebServer.arg("plain"));
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    
    _viewers = doc["data"][0]["viewer_count"].as<int>();
    Serial.println("Viewers : " + _viewers);
  }
  else WebServer.send(200, "text/plain", "Invalid request to server. Try again.");
}

//Function to handle WebServer response to access root : Test purpose
void WebServer_onRoot(void) {
  WebServer.send(200, "text/plain", "Welcome to Lazare's microServer Secured\nPlease go to twitch.tv/lazarelive\nThanks");
  Serial.println("[MESG] : Root accessed");
}
 
//Function to handle WebServer response to not found parameter
void WebServer_onNotFound(void) {
  WebServer.send(404, "text/plain", "Nothing here. (Secured)");
  Serial.println("[MESG] : Not adressed accessed");
}


//A modifier avec les URI en paramètre
void WebServerInitialize(void) {
  WebServer.on(Helix.getURIFollower(), WebServer_onFollowEvent);
  WebServer.on(Helix.getURIStream(), WebServer_onStreamEvent);
  WebServer.on("/", WebServer_onRoot);
  WebServer.onNotFound(WebServer_onNotFound);
  
  WebServer.begin();
}
