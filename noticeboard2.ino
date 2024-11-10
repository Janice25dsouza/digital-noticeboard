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
const char* PARAM_PASSWORD = "password";

// Define the correct password
const String correctPassword = "notice";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
    <head>
        <title>Smart Notice Board</title>
        <meta name="viewport" content="width=device-width, initial-scale=5">
        <style>
            /* Add a border only on hover */
            input[type="text"], input[type="password"] {
                background-color: chartreuse;
                border: none;
                padding: 5px;
                transition: border 0.3s ease;
            }

            /* Show border on hover */
            input[type="text"]:hover, input[type="password"]:hover {
                border: 2px solid black;
            }
        </style>
    </head>
    <body style="background-color:rgb(250, 252, 132)">
        <center>
            <div>
                <p style="color:black; padding:40px;"><font size="9" face="cursive">Web Controlled Smart Notice Board</font></p>
            </div>
            
            <div style="color:black; font-size:20px; background-color:Chartreuse; padding-top:20px; padding-bottom:0; width:500px; border-width:25px; border-style:solid; border-color:black;">
                <form action="/get" style="text-align: left; padding-left: 20px;">
                    <div>
                        <label for="input1">Enter the text to display:</label>
                        <input type="text" id="input1" name="input1"><br><br>
                    </div>
                    <div>
                        <label for="password">Enter Password:</label>
                        <input type="password" id="password" name="password"><br><br>
                    </div>
                    <input type="submit" value="Send" style="background-color: chartreuse; color: black;font-size:15px;">
                </form><br>
                <p id="errorMessage" style="color:red;"></p>
            </div>
        </center>
    </body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcd.begin(16, 2);  // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("IEP project");

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
    String password;
    bool showError = false;

    // Check if both message and password parameters are provided
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_PASSWORD)) {
      message = request->getParam(PARAM_INPUT_1)->value();
      password = request->getParam(PARAM_PASSWORD)->value();

      // Check if the entered password matches the correct password
      if (password == correctPassword) {
        lcd.clear();
        lcd.setCursor(0, 0);  // Adjust cursor position as needed
        lcd.print(message);
      } else {
        showError = true;  // Set flag for an incorrect password
      }
    } else {
      message = "No message sent";
    }

    // Send response with an error message if the password is incorrect
    String htmlResponse = index_html;
    if (showError) {
      htmlResponse += "<script>document.getElementById('errorMessage').innerText = 'Incorrect password. Please try again.';</script>";
    }
    
    request->send(200, "text/html", htmlResponse);
    Serial.println(showError ? "Incorrect password" : message);
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
