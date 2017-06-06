#include <ESP8266WiFi.h>    //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//for LED status
#include <Ticker.h>

#define RELAY1  0
#define RELAY2  2
#define RELAY3  4
#define RELAY4  5

#define RELAY5  12
#define RELAY6  13
#define RELAY7  14
#define RELAY8  15

#define RELAY_COUNT 8


int relays[RELAY_COUNT] = {RELAY1, RELAY2, RELAY3, RELAY4, RELAY5, RELAY6, RELAY7, RELAY8};

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

Ticker ticker;

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setup() {
  Serial.begin(115200);
  
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");  
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);

  server.begin();

  //setup relays, all of them are connected via output pin and they are off
  for (int i = 0; i < RELAY_COUNT; i++)
  {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], HIGH);
  }
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request with the corresponding relay
  int val;
  int i;
  bool found = false;
  for (i = 0; i < RELAY_COUNT && !found; i++)
  {
    String request = "/gpio/";
    request += i;    
    if (req.indexOf(request) != -1)
    {
      Serial.println(request);      
      val = togglePin(relays[i]);
      digitalWrite(relays[i], val);
      found = true;
    }
  }  

  client.flush();

  if (found)
  {
    // Send the response to the client
    client.print(createRelayStatusResponseMessage(i-1));
  }
  else
  {
    String request = "/gpio";
    if (req.indexOf(request) != -1)
    {
      // Send the response to the client
      client.print(createStatusResponseMessage());
    }
  }
  
  delay(1);
  Serial.println("Client disonnected"); 
}

//returns with the relays status
String createRelayStatusResponseMessage(int i)
{
  // Prepare the response

  String response = getHeader();
  response += getRelayStatusJson(i);

  return response;
}

//returns with the relays statuses
String createStatusResponseMessage()
{
  String response = getHeader();
  response += "[\r\n";
  for(int i = 0; i < RELAY_COUNT; i ++)
  {
    response += getRelayStatusJson(i);
    if (i < RELAY_COUNT - 1)
    {
      response += ",\r\n";
    }
  } 
  response += "]\r\n";

  return response;  
}

//gets the HTTP header
String getHeader()
{
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: application/json\r\n\r\n";
  return response;
}

//gets the status of the relay in JSON fromat
String getRelayStatusJson(int i)
{
  //{
  //  id: 0,
  //  status: true/false,
  //}
  String response = "{\r\n";
  response += " id: ";
  response += i;
  response += ",\r\n";
  response += " status: ";
  response += digitalRead(relays[i]) ? "false" : "true";
  response += "\r\n}\r\n";

  return response;
}

//toggles the pin
int togglePin(int pin)
{
  int pinVal = digitalRead(pin);
  if (pinVal == LOW) return HIGH;
  if (pinVal == HIGH) return LOW;
}

//makes the built in led to blink
void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}
