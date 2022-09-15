#include <WiFi.h>

const char* ssid = "SUA REDE WIFI VAI AQUI";
const char* password = "SUA SENHA VAI AQUI";

WiFiServer server(80);

String header;

String output2State = "off";
const int output2 = 2;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(output2, OUTPUT);
  digitalWrite(output2, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Aguardando conexao...");
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();

  if (client) {                             
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Cliente conectado.");         
    String currentLine = "";                
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 2 on");
              output2State = "on";
              digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 2 off");
              output2State = "off";
              digitalWrite(output2, LOW);
            }
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; box-shadow: 2px 2px 5px rgba(0, 0, 0, 0.2);");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            client.println("<body><h1>App Mesquita</h1>");
            
            client.println("<p>GPIO 2 - State " + output2State + "</p>");   
            if (output2State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button\">ON</button></a></p>");
            } 
               
            client.println("</body></html>");
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;     
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Cliente desconectado!");
    Serial.println("");
  }
}
