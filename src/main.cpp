
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "@@@";
const char* password = "qwerty1234";

// Your OpenWeatherMap API key
String openWeatherMapApiKey = "dd84d5ef3feea45ed002f5863b4fac2b";

// Replace with your city and country code.  Make sure these are correct for OpenWeatherMap.
String city = "Tashkent";
String countryCode = "UZ";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 6000; // Default: 10 minutes (600000 milliseconds)

String jsonBuffer;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Timer set to ");
  Serial.print(timerDelay / 1000); // Print in seconds
  Serial.println(" seconds.");
}

void loop() {
  // Send an HTTP GET request at the specified interval
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      // Construct the API request URL
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric"; //Added units=metric

      jsonBuffer = httpGETRequest(serverPath.c_str()); // Pass the c_string
      Serial.println(jsonBuffer);

      // Parse the JSON response
      JSONVar myObject = JSON.parse(jsonBuffer);

      // Check if parsing was successful
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing JSON failed!");
        return; // Exit the loop iteration
      }

      // Print the parsed data
      Serial.print("JSON object = ");
      Serial.println(myObject);
      // Check for the existence of the "main" and "wind" objects before accessing their members.
      if (myObject.hasOwnProperty("main")) {
        Serial.print("Temperature: ");
        Serial.println((double)myObject["main"]["temp"]); // Cast to double
        Serial.print("Pressure: ");
        Serial.println((double)myObject["main"]["pressure"]);  //cast to double
        Serial.print("Humidity: ");
        Serial.println((int)myObject["main"]["humidity"]);     //cast to int
      } else {
        Serial.println("Error: 'main' object not found in JSON response.");
      }

      if (myObject.hasOwnProperty("wind")) {
        Serial.print("Wind Speed: ");
        Serial.println((double)myObject["wind"]["speed"]); //cast to double
      } else {
        Serial.println("Error: 'wind' object not found in JSON response.");
      }
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis(); // Update the timer
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP GET request
  int httpResponseCode = http.GET();
  String payload = "{}"; // Default empty JSON payload

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}
