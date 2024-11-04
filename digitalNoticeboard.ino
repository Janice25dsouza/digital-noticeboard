#include <Wire.h> 
#include <hd44780.h>         // Main hd44780 library header
#include <hd44780ioClass/hd44780_I2Cexp.h> 

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

hd44780_I2Cexp lcd;  // Use hd44780_I2Cexp for automatic address and pin mapping detection
AsyncWebServer server(80);
// WiFi Credentials
const char* ssid = "Galaxy A51D182";
const char* password = "12345678";

const char* PARAM_INPUT_1 = "input1";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Smart Notice Board</title>
  <meta name="viewport" content="width=device-width, initial-scale=5">
<p> <font size="9" face="sans-serif"> <marquee> Web Controlled Smart Notice Board </marquee> </font> </p>
  </head><body><center>
  <form action="/get">
    Enter the Text to Display: <input type="text" name="input1">
    <input type="submit" value="Send">
  </form><br>
</center></body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcd.begin(16, 2);  // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("hello world");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
 
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(PARAM_INPUT_1)) {
      message = request->getParam(PARAM_INPUT_1)->value();
      lcd.clear();
      lcd.setCursor(0, 0);  // Adjust cursor position as needed
      lcd.print(message);
    } else {
      message = "No message sent";
    }
    Serial.println(message);
    request->send(200, "text/html", index_html);
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  for (int positionCounter = 0; positionCounter < 29; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(500);
  }
}
