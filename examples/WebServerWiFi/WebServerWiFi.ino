// Подключаем необходимую библиотеку
#include "flprogWebServer.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

/*
  -------------------------------------------------------------------------------------------------
        Создание интерфейса для работы с Wifi интерфейсом

  -------------------------------------------------------------------------------------------------
*/
FLProgOnBoardWifiInterface WifiInterface;

/*
  -----------------------------------------------------------------------------------------
     Создаем объект непосредстредственно вебсервера на необходимом интерфейсе
  -----------------------------------------------------------------------------------------
*/

FLProgWebServer webServer(&WifiInterface, 80);

/*
  -----------------------------------------------------------------------------------------
          Определение рабочих параметров и функций
  -----------------------------------------------------------------------------------------
*/
uint32_t blinkStartTime = 0;

uint8_t ethernetStatus = 255;
uint8_t ethernetError = 255;

bool isNeedClientSendConnectMessage = true;
bool isNeedClientSendDisconnectMessage = true;

bool isNeedApSendConnectMessage = true;
bool isNeedApSendDisconnectMessage = true;

uint32_t startSendTime;

//=================================================================================================
void setup()
{

  Serial.begin(115200);
  while (!Serial)
  {
  }

  flprog::printConsole(" Тест WebServer - Wifi ");
  pinMode(LED_BUILTIN, OUTPUT);

  /*
    -----------------------------------------------------------------------------------------
         Настройка интерфейса
    -----------------------------------------------------------------------------------------
  */
  WifiInterface.clientOn();
  WifiInterface.mac(0x78, 0xAC, 0xC0, 0x2C, 0x30, 0x45);
  //WifiInterface.localIP(IPAddress(192, 168, 199, 38));
  //WifiInterface.resetDhcp();
  WifiInterface.setClientSsidd("totuin-router");
  WifiInterface.setClientPassword("12345678");
  /*
    -----------------------------------------------------------------------------------------
           Настройка Вэб сервера
    -----------------------------------------------------------------------------------------
  */
  webServer.addHandler("", mainPage);
  webServer.addHandler("/test1", testPage1);
  webServer.addHandler("/test2", testPage2);
  webServer.add404Page(page_404);
}

//=================================================================================================
void loop()
{
  printStatusMessages();
  blinkLed();
  WifiInterface.pool();
  webServer.pool();
}

//=================================================================================================

void page_404()
{
  startSendTime = millis();
  String result = "HTTP/1.1 404 Not Found";
  result += "\r\n";
  result += "Content-Type: text/html";
  result += "\r\n";
  result += "Connection: close";
  result += "\r\n";
  result += "\r\n";
  result += "<!DOCTYPE HTML>";
  result += "\r\n";
  result += "<html>";
  result += "<h1>Page not found</h1>";
  result += "<br>";
  result += sendWebServerData();
  webServer.print(result);
  result = "<h4> SendTime - ";
  result += String(millis() - startSendTime);
  result += "</h4></html>";
  webServer.print(result);
}
void mainPage()
{
  startSendTime = millis();
  String result = sendHeader();
  result += "<h1>MainPage</h1>";
  result += "<br>";
  result += "<a href=\"/test1\">Test page 1</a>";
  result += "<br>";
  result += "<a href=\"/test2?value1=10&value2=blabla&value3=12345678\">Test page 2</a>";
  result += "<br>";
  result += sendWebServerData();
  webServer.print(result);
  result = "<h4> SendTime - ";
  result += String(millis() - startSendTime);
  result += "</h4></html>";
  webServer.print(result);
}

void testPage1()
{
  startSendTime = millis();
  String result = sendHeader();
  result += "<h1>Test Page 1</h1>";
  result += "<br>";
  result += "<a href=\"/\">MainPage</a>";
  result += "<br>";
  result += "<a href=\"/test2?value1=10&value2=blabla&value3=12345678\">Test page 2</a>";
  result += sendWebServerData();
  webServer.print(result);
  result = "<h4> SendTime - ";
  result += String(millis() - startSendTime);
  result += "</h4></html>";
  webServer.print(result);
}

void testPage2()
{
  startSendTime = millis();
  String result = sendHeader();
  result += "<h1>Test Page 2</h1>";
  result += "<br>";
  result += " <a href = \"/\">MainPage</a>";
  result += "<br>";
  result += "<a href=\"/test1\">Test page 1</a>";
  result += sendWebServerData();
  webServer.print(result);
  result = "<h4> SendTime - ";
  result += String(millis() - startSendTime);
  result += "</h4></html>";
  webServer.print(result);
}

String sendHeader()
{
  String result = "HTTP/1.1 200 OK";
  result += "\r\n";
  result += "Content-Type: text/html";
  result += "\r\n";
  result += "Connection: close";
  result += "\r\n";
  result += "\r\n";
  result += "<!DOCTYPE HTML>";
  result += "\r\n";
  result += "<html>";
  result += "\r\n";
  return result;
}

String sendWebServerData()
{
  String result = "<h2> Server data</h2>";
  result += "<h3> Main data</h3>";
  result += "<h4> Method - ";
  result += String(webServer.method());
  result += "<br> Method version - ";
  result += String(webServer.methodVersion());
  result += "<br> Host - ";
  result += webServer.host();
  result += "<br> URL - ";
  result += webServer.uri();
  result += "</h4>";
  result += "<h3> Headers </h3>";
  result += "<h4>";
  for (int i = 0; i < webServer.headersCount(); i++)
  {
    String key = webServer.headerKeyAtIndex(i);
    if (webServer.hasHeaderKey(key))
    {
      String value = webServer.headerValueAtKey(key);
      result += String(i);
      result += ": ";
      result += key;
      result += " -- ";
      result += value;
      result += "<br>";
    }
  }
  result += "</h4>";
  result += "<h3> Arguments </h3>";
  result += "<h4>";
  for (int i = 0; i < webServer.argumentsCount(); i++)
  {
    String key = webServer.argumentKeyAtIndex(i);
    if (webServer.hasArgumentKey(key))
    {
      String value = webServer.argumentValueAtKey(key);
      result += String(i);
      result += ": ";
      result += key;
      result += " = ";
      result += value;
      result += "<br>";
    }
  }
  result += "</h4>";
  return result;
}

void blinkLed()
{
  if (flprog::isTimer(blinkStartTime, 50))
  {
    blinkStartTime = millis();
    digitalWrite(LED_BUILTIN, !(digitalRead(LED_BUILTIN)));
  }
}

void printStatusMessages()
{
  if (WifiInterface.getStatus() != ethernetStatus)
  {
    ethernetStatus = WifiInterface.getStatus();
    Serial.println();
    Serial.print("Статус интерфейса - ");
    Serial.println(flprog::flprogStatusCodeName(ethernetStatus));
  }
  if (WifiInterface.getError() != ethernetError)
  {
    ethernetError = WifiInterface.getError();
    if (ethernetError != FLPROG_NOT_ERROR)
    {
      Serial.println();
      Serial.print("Ошибка интерфейса - ");
      Serial.println(flprog::flprogErrorCodeName(ethernetError));
    }
  }
  printClientConnectMessages();
  printClientDisconnectMessages();
  printApConnectMessages();
  printApDisconnectMessages();
}

void printClientConnectMessages()
{
  if (!WifiInterface.clientIsReady())
  {
    return;
  }
  if (!isNeedClientSendConnectMessage)
  {
    return;
  }
  Serial.println("WiFiClient подключён!");
  Serial.print("Ssid - ");
  Serial.println(WifiInterface.clientSsid());
  Serial.print("Ip - ");
  Serial.println(WifiInterface.localIP());
  Serial.print("DNS - ");
  Serial.println(WifiInterface.dns());
  Serial.print("Subnet - ");
  Serial.println(WifiInterface.subnet());
  Serial.print("Gateway - ");
  Serial.println(WifiInterface.gateway());
  Serial.println();
  isNeedClientSendConnectMessage = false;
  isNeedClientSendDisconnectMessage = true;
}

void printApConnectMessages()
{
  if (!WifiInterface.apIsReady())
  {
    return;
  }
  if (!isNeedApSendConnectMessage)
  {
    return;
  }
  Serial.println("WiFi точка включенна!");
  Serial.print("Ssid - ");
  Serial.println(WifiInterface.apSsid());
  Serial.print("Ip - ");
  Serial.println(WifiInterface.apLocalIP());
  Serial.print("DNS - ");
  Serial.println(WifiInterface.apDns());
  Serial.print("Subnet - ");
  Serial.println(WifiInterface.apSubnet());
  Serial.print("Gateway - ");
  Serial.println(WifiInterface.apGateway());
  Serial.println();
  isNeedApSendConnectMessage = false;
  isNeedApSendDisconnectMessage = true;
}

void printClientDisconnectMessages()
{
  if (WifiInterface.clientIsReady())
  {
    return;
  }
  if (isNeedClientSendConnectMessage)
  {
    return;
  }
  if (!isNeedClientSendDisconnectMessage)
  {
    return;
  }
  Serial.println("WiFiClient отключён!");
  isNeedClientSendConnectMessage = true;
  isNeedClientSendDisconnectMessage = false;
}

void printApDisconnectMessages()
{
  if (WifiInterface.apIsReady())
  {
    return;
  }
  if (isNeedApSendConnectMessage)
  {
    return;
  }
  if (!isNeedApSendDisconnectMessage)
  {
    return;
  }
  Serial.println("WiFi точка отключёна!");
  isNeedApSendConnectMessage = true;
  isNeedApSendDisconnectMessage = false;
}