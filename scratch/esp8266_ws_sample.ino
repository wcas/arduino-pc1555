// Display Test
// Test several Display Adafruit_GFX functions
// for 1.8" TFT LCD Color Display (SainSmart).
// Hans Luijten, Tweaking4All.com

// Init pins
#define sclk 13
#define mosi 11
#define cs   10
#define dc   9
#define rst  8  

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SoftwareSerial.h>
#include <SPI.h>

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
SoftwareSerial dbgSerial(3, 2);

void setup() {
  // Init serial for serial monitor, so we can see debug text in the Serial Monitor Window
  Serial.begin(9600);
  Serial.println("Display Function Tests Started");

  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab  
  Serial.println("setup() - Init Completed");
  
  // Clear screen
  Serial.println("Fill screen with BLACK");
  tft.fillScreen(ST7735_BLACK);

  // Hello
  tft.setTextSize(1);
  tft.setCursor(0, 5);
  tft.setTextColor(ST7735_WHITE);
  
  
  // Configure wifi

  dbgSerial.begin(9600);
  
  while(1){
    tft.fillScreen(ST7735_BLACK);
    tft.println("Initializing WiFi...");
    dbgSerial.println("AT+RST");
    delay(100);
    if(dbgSerial.find("ready"))
    {
      tft.println("> Ready");
      delay(100);
      break;
    }
  }
  
  while(1)
  {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 5);
    tft.println("Connecting to AP");
    dbgSerial.println("AT+CWMODE=1");
    String cmd="AT+CWJAP=\"";
    cmd+="homeap";
    cmd+="\",\"";
    cmd+="password";
    cmd+="\"";
    dbgSerial.println(cmd);
    delay(200);
    if(dbgSerial.find("OK"))
    {
      tft.println("> OK");
      delay(100);
      break;
    }
  }
  
  while(1)
  {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 5);
    tft.println("Enabling connections");
    //set the multi connection mode
    dbgSerial.println("AT+CIPMUX=1");
    if(dbgSerial.find("OK"))
    {
      tft.println("> OK");
      delay(100);
      break;
    }
  }
  
  /*
  // Send alive packet to workstation
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "192.168.1.22";
  cmd += "\",8080";
  dbgSerial.println(cmd);
  if(dbgSerial.find("Error")) return;
  cmd = "GET / HTTP/1.0\r\n\r\n";
  dbgSerial.print("AT+CIPSEND=");
  dbgSerial.println(cmd.length());
  if(dbgSerial.find(">"))
  {
    tft.print(">");
  }else
  {
    dbgSerial.println("AT+CIPCLOSE");
    tft.println("connect timeout");
    delay(1000);
    return;
  }
  dbgSerial.print(cmd);
  delay(2000);
  //Serial.find("+IPD");
  while (dbgSerial.available())
  {
    char c = dbgSerial.read();
    tft.write(c);
    if(c=='\r') tft.println('5');
  }
  tft.println("====");
  delay(1000);
  */

  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 5);
  tft.println("Starting Server...");   
  dbgSerial.println("AT+CIPSERVER=1,8888");
  delay(100);
  
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 5);
}
void loop() {
  // Check for incoming data
  if(dbgSerial.find("+IPD,"))
  {
    // Clear screen
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 5);
    
    // Determine connection ID for response
    int conn_id = dbgSerial.read() - '0';
    String message = "";
    
    while (dbgSerial.available())
    {
      char c = dbgSerial.read();
      tft.write(c);
      message += c;
      if(c=='\r') tft.write('\n');
    }
    
    tft.println("Sending response to id: ");
    tft.println(conn_id);
    
    String html_response = "<html><body><h1>Success!</h1>";
    html_response += "<br>You wrote:<br>";
    html_response += message;
    html_response += "</body></html>\n\n";
    
    String response = "HTTP/1.1 200 OK\n";
    response += "Server: ESP8266/1.0\n";
    response += "Content-Length: ";
    response += html_response.length();
    response += " \n";
    response += "Connection: close\n";
    response += "Content-Type: text/html\n\n";
    
    String cmd = "AT+CIPSEND=";
    cmd += conn_id;
    cmd += ",";
    cmd += response.length() + html_response.length();
    dbgSerial.println(cmd);
    dbgSerial.print(response);
    dbgSerial.println(html_response);
    //dbgSerial.print("AT+CIPCLOSE=");
    //dbgSerial.println(conn_id);
  }
}
