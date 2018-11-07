/**
   Get authorized token for request header
*/
void getUserToken() {
  String authData = String("username=") + authUsername + String("&grant_type=password&password=") + authPassword;
  const char* token;
  // Endpoint for token
  String endPoint = "/api/account/token";
  String url = apiServer + endPoint;
  Serial.println(url);
  Serial.println(authData);
  if (WiFi.status() == WL_CONNECTED) {

    // Specify endpoint for http request
    http.begin(url);
    //Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(authData);
    Serial.println("httpResponseCode: " + http.errorToString(httpResponseCode));
    Serial.println(authData);
    if (httpResponseCode > 0) {

      //Get the response to the request
      String response = http.getString();

      //Print return code
      Serial.println(httpResponseCode);
      //Print request answer

      // Parse response
      int size = response.length() + 1;
      char json[size];
      response.toCharArray(json, size);
      Serial.println("JSON:");
      Serial.println(json);

      JsonObject& root = jsonBuffer.parseObject(json);
      token = root["access_token"];
      Serial.println("Access_token:");
      Serial.println(token);
      strncpy(access_token, token, strlen(token));
      if (!root.success())
      {
        Serial.print("parseObject(");
        Serial.print(response);
        Serial.println(") failed");
      }

    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    //Free resources
    http.end();

  } else {
    Serial.println("Error in WiFi connection");
  }
}

/**
   Send notification to user app
*/
void sendNotification(const char* catId, CatState state) {

  // Get token
  getUserToken();

  char data[1024];
  // Put json data
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["CatId"] = catId;
  root["DoorId"] = DoorIDnum.c_str();
  root["State"] = (int) state;
  root.printTo(data, sizeof(data));
  int len = root.measureLength();

  // Server endpoint url
  String endPoint = "/api/notifications";
  String url = apiServer + endPoint;
  Serial.println(url);
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {

    //Specify destination for HTTP request
    http.begin(url);
    //Specify content-type header
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(access_token);
    Serial.println(data);
    //Send the actual POST request
    int httpResponseCode = http.POST(data);
    Serial.println(http.getString());
    Serial.println("httpResponseCode: " + http.errorToString(httpResponseCode));
    if (httpResponseCode > 0) {

      //Get the response to the request
      String response = http.getString();

      //Print return code
      Serial.println(httpResponseCode);
      //Print request answer
      Serial.println(response);

    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    //Free resources
    http.end();

  } else {
    Serial.println("Error in WiFi connection");
  }

  //delay 10 seconds
  delay(10000);
}

void getSchedules() {
  // Get token
  getUserToken();

  // Server endpoint url
  String endPoint = String("/api/doors/") + DoorIDnum + String("/schedules");
  String url = apiServer + endPoint;
  Serial.println(url);

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {

    //Specify destination for HTTP request
    http.begin(url);
    //Specify content-type header
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(access_token);
    //Send the actual POST request
    int httpResponseCode = http.GET();
    //String response = http.getString();
    Serial.println("httpResponseCode: " + http.errorToString(httpResponseCode));
    if (httpResponseCode > 0) {

      //Get the response to the request
      String response = http.getString();

      //Print return code
      Serial.println(httpResponseCode);
      //Print request answer
      Serial.println(response);

      // Parse response
      int size = response.length() + 1;
      char json[size];
      response.toCharArray(json, size);
      Serial.println("JSON:");
      Serial.println(json);
            
            JsonObject& root = jsonBuffer.parseObject(json);
            int hour, minute;
            hour = root["OpenTime"]["Hour"];
            minute = root["OpenTime"]["Minute"];
            
            Serial.println(String("Hour: ") + hour);
            Serial.println(String("Minute: ") + minute);
           
            if (!root.success())
            {
              Serial.print("parseObject(");
              Serial.print(response);
              Serial.println(") failed");
            }
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    //Free resources
    http.end();

  } else {
    Serial.println("Error in WiFi connection");
  }

  //delay 10 seconds
  delay(10000);
}
