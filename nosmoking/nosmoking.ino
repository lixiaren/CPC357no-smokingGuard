
      #include <WiFi.h>
      #include <PubSubClient.h>
      #include <DHT.h>
      #include <Adafruit_NeoPixel.h>




      #define RGB_PIN        48    
      #define BUZZER_PIN     12   

      #define DHTPIN         42     
      #define DHTTYPE        DHT11
      #define MQ2PIN         10     



      const char* ssid = "TIMI";        
      const char* password = "985usm211";  


      const char* mqtt_server = "broker.emqx.io"; 
      const int mqtt_port = 1883;
      const char* mqtt_topic = "building/toilet/smoke_detector";



      const int SMOKE_CIGARETTE = 1000;
      const int SMOKE_VAPE = 800;
      const float HUM_JUMP = 2.0;   // 2 秒内上升 ≥3%




      WiFiClient espClient;
      PubSubClient client(espClient);
      DHT dht(DHTPIN, DHTTYPE);
      Adafruit_NeoPixel pixels(1, RGB_PIN, NEO_GRB + NEO_KHZ800);

      float lastHumidity = 0.0;
      unsigned long lastMsgTime = 0;




      void setLight(int r, int g, int b) {
        pixels.setPixelColor(0, pixels.Color(r, g, b));
        pixels.show();
      }


      void playAlarm() {
        tone(BUZZER_PIN, 3000, 100); 
        delay(150);
        tone(BUZZER_PIN, 3000, 100);
      }



      void setup_wifi() {
        delay(10);
        Serial.println();
        Serial.print("Connecting to WiFi: ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);


        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
          setLight(0, 0, 50); 

          delay(100);
          setLight(0, 0, 0);  
        }

        Serial.println("");
        Serial.println("WiFi Connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        setLight(0, 50, 0); 
      }

      void reconnect() {
        while (!client.connected()) {
          Serial.print("Attempting MQTT connection...");
          String clientId = "Feather-" + String(random(0xffff), HEX);
          
          if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            setLight(0, 50, 0); // Green
          } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            setLight(50, 50, 0); 
            delay(5000);
          }
        }
      }



      void setup() {
        Serial.begin(115200);


        pinMode(MQ2PIN, INPUT);
        pinMode(BUZZER_PIN, OUTPUT);
        
        dht.begin();
        pixels.begin();
        pixels.setBrightness(100); 


        setLight(50, 50, 50);
        tone(BUZZER_PIN, 1000, 500);
        delay(1000);

        // 连接网络
        setup_wifi();
        client.setServer(mqtt_server, mqtt_port);
      }

      void loop() {
        if (!client.connected()) reconnect();
        client.loop();

        unsigned long now = millis();
        

        if (now - lastMsgTime > 2000) {
          lastMsgTime = now;


          float h = dht.readHumidity();
          float t = dht.readTemperature();
          int smokeValue = analogRead(MQ2PIN); 

          
          if (isnan(h) || isnan(t)) {
            Serial.println("Error: DHT sensor failed! ");
            return; 
          }
          
      
          if (lastHumidity == 0) lastHumidity = h;


      float humDiff = h - lastHumidity;
      lastHumidity = h;

      String status = "Safe";
      String type = "None";
      bool alert = false;


      Serial.print("Smoke: "); Serial.print(smokeValue);
      Serial.print(" | Hum: "); Serial.print(h);
      Serial.print(" | HumDiff: "); Serial.println(humDiff);



      //  纸烟
      if (smokeValue >= SMOKE_CIGARETTE) {
        alert = true;
        status = "VIOLATION";
        type = "Cigarette Smoke";

        Serial.println(">> ALARM: Traditional Cigarette Detected!");
        setLight(255, 0, 0);   // 红灯
        playAlarm();
      }

      // 电子烟
      else if (smokeValue >= SMOKE_VAPE && humDiff >= HUM_JUMP) {
        alert = true;
        status = "VIOLATION";
        type = "Vape / E-Cigarette";

        Serial.println(">> ALARM: Vape Detected (Humidity Spike)!");
        setLight(150, 0, 200); // 紫灯
        playAlarm();
      }

      // 安全
      else {
        alert = false;
        status = "Safe";
        type = "None";

        setLight(0, 10, 0);    // 绿灯
        noTone(BUZZER_PIN);
      }
          
          // JSON: {"temp":26.5, "hum":60.2, "smoke":1200, "status":"Safe", "type":"None", "alert":false}
          String payload = "{";
          payload += "\"temp\": " + String(t, 1) + ",";
          payload += "\"hum\": " + String(h, 1) + ",";
          payload += "\"smoke\": " + String(smokeValue) + ",";
          payload += "\"status\": \"" + status + "\",";
          payload += "\"type\": \"" + type + "\",";
          payload += "\"alert\": " + String(alert ? "true" : "false");
          payload += "}";

          Serial.print("Publishing: ");
          Serial.println(payload);

          client.publish(mqtt_topic, payload.c_str());
        }
      }