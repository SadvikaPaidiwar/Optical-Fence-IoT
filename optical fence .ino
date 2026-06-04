#include <ESP8266WiFi.h>

#define DETECT D5  // Pin connected to the laser receiver
#define ACTION D2  // Pin for the action (e.g., buzzer or LED)

// WiFi credentials
char ssid[] = "**********"; // Put your ID name
char pass[] = "********";// Put your password here

// Google Sheets script ID and host
String GAS_ID = "AKfGOb___Your_Google_Sheet_ID"; // Spreadsheet script ID
const char* host = "script.google.com";

bool laserState = false; // Variable to hold the state of the LASER Rx
void setup() {
Serial.begin(9600);

// Define pin modes
 pinMode(DETECT, INPUT);   // Set laser receiver pin as input
 pinMode(ACTION, OUTPUT);  // Set action pin as output

  // Connect to WiFi
  Serial.print("Connecting");
  WiFi.begin(ssid, pass); // Connect to your WiFi router
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to WiFi");
}

void update_google_sheet() {
    Serial.print("Connecting to ");
    Serial.println(host);
  
    WiFiClientSecure client;
    const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
    client.setInsecure(); // This is the magical line that makes everything work
    
    if (!client.connect(host, httpPort)) {
      Serial.println("Connection failed");
      return;
    }
       
    // Processing data and sending data
    String url = "/macros/s/" + GAS_ID + "/exec?laserState=";
    url += (laserState ? "1" : "0");
    Serial.print("Requesting URL: ");
    Serial.println(url);
  
    // Send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    Serial.println();
    Serial.println("Closing connection");  
}

void loop() {
  laserState = digitalRead(DETECT); // Read the state of the laser receiver
  
  // Control the ACTION pin based on the laserState
  if (laserState == HIGH) {
    digitalWrite(ACTION, HIGH); // Turn ON the action (e.g., buzzer or LED)
  } else {
    digitalWrite(ACTION, LOW);  // Turn OFF the action
  }
  
  // Update data to Google Sheets periodically
  static unsigned long lastUpdate = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= 10000) { // Update every 10 seconds
    lastUpdate = currentMillis;
    update_google_sheet();
  }
delay(250); // Short delay for stability
}
