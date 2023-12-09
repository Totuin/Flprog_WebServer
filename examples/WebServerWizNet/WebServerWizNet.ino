// Подключаем необходимую библиотеку
#include "flprogWebServer.h"

/*
  -------------------------------------------------------------------------------------------------
        Создание интерфейса для работы с чипом W5100(W5200,W5500)
        Шина SPI и пин CS берутся из  RT_HW_Base.device.spi.busETH и RT_HW_Base.device.spi.csETH
  -------------------------------------------------------------------------------------------------
*/
FLProgWiznetInterface WiznetInterface;

/*
  -------------------------------------------------------------------------------------------------
        Второй вариант cоздания интерфейса для работы с чипом W5100(W5200,W5500).
        С непосредственной привязкой  пину.
        Пин CS - 10
        Шина SPI берётся из RT_HW_Base.device.spi.busETH
  -------------------------------------------------------------------------------------------------
*/
// FLProgWiznetInterface WiznetInterface(10);

/*
  -------------------------------------------------------------------------------------------------
      Третий вариант cоздания интерфейса для работы с чипом W5100(W5200,W5500).
      С непосредственной привязкой  пину и шине.
      Пин CS - 10
      Шина SPI - 0
  -------------------------------------------------------------------------------------------------
*/
// FLProgWiznetInterface WiznetInterface(10, 0);

/*
  -----------------------------------------------------------------------------------------
     Создаем объект непосредстредственно вебсервера на необходимом интерфейсе
  -----------------------------------------------------------------------------------------
*/

FLProgWebServer webServer(&WiznetInterface, 80);

/*
  -----------------------------------------------------------------------------------------
          Определение рабочих параметров и функций
  -----------------------------------------------------------------------------------------
*/
uint32_t blinkStartTime = 0;

uint8_t ethernetStatus = 255;
uint8_t ethernetError = 255;

bool isNeedSendConnectMessage = true;
bool isNeedSendDisconnectMessage = true;

uint32_t startCicleTime;
uint32_t maxCicleTime = 0;

//=================================================================================================
void setup()
{

    Serial.begin(115200);
    while (!Serial)
    {
    }

    flprog::printConsole(" Тест WebServer - WizNet ");

    Serial.print("CS - ");
    Serial.println(WiznetInterface.pinCs());
    Serial.print("SPI BUS - ");
    Serial.println(WiznetInterface.spiBus());
    pinMode(LED_BUILTIN, OUTPUT);

    /*
      -----------------------------------------------------------------------------------------
           Настройка интерфейса
      -----------------------------------------------------------------------------------------
    */
    WiznetInterface.mac(0x78, 0xAC, 0xC0, 0x0D, 0x5B, 0x86);
    WiznetInterface.localIP(IPAddress(192, 168, 1, 10));
    WiznetInterface.resetDhcp();
    /*
      -----------------------------------------------------------------------------------------
             Настройка Вэб сервера
      -----------------------------------------------------------------------------------------
    */
    webServer.addHandler("", mainPage);
    webServer.addHandler("/test1", testPage1);
    webServer.addHandler("/test2", testPage2);
    webServer.addHandler("/resetCounter", resetCounter);

    webServer.add404Page(page_404);
}

//=================================================================================================
void loop()
{
    startCicleTime = millis();
    printStatusMessages();
    blinkLed();
    WiznetInterface.pool();
    webServer.pool();
    uint32_t diff = flprog::difference32(startCicleTime, millis());
    if (diff > maxCicleTime)
    {
        maxCicleTime = diff;
    }
}

//=================================================================================================

void page_404()
{

    webServer.println("HTTP/1.1 404 Not Found");
    webServer.println("Content-Type: text/html");
    webServer.println("Connection: close");
    webServer.println();
    webServer.println("<!DOCTYPE HTML>");
    webServer.print("<html>");
    webServer.print("<h1>Page not found</h1>");
    webServer.print("<br>");
    sendWebServerData();
    webServer.print("<h4> Max cicle time - ");
    webServer.print(maxCicleTime);
    webServer.print("</h4></html>");
}
void mainPage()
{
    sendHeader();
    webServer.print("<h1>MainPage</h1>");
    webServer.print("<br>");
    webServer.print("<a href=\"/test1\">Test page 1</a>");
    webServer.print("<br>");
    webServer.print("<a href=\"/test2?value1=10&value2=blabla&value3=12345678\">Test page 2</a>");
    webServer.print("<br>");
    webServer.print("<a href=\"/resetCounter\">Reset max cicle time</a>");
    webServer.print("<br>");
    sendWebServerData();
    webServer.print("<h4> Max cicle time - ");
    webServer.print(maxCicleTime);
    webServer.print("</h4></html>");
}

void resetCounter()
{
    maxCicleTime = 0;
    sendHeader();
    webServer.print("<h1>Max cicle time is resetng</h1>");
    webServer.print("<br>");
    webServer.print("<a href=\"/\">MainPage</a>");
    webServer.print("<h4> Max cicle time - ");
    webServer.print(maxCicleTime);
    webServer.print("</h4></html>");
}

void testPage1()
{
    sendHeader();
    webServer.print("<h1>Test Page 1</h1>");
    webServer.print("<br>");
    webServer.print("<a href=\"/\">MainPage</a>");
    webServer.print("<br>");
    webServer.print("<a href=\"/test2?value1=10&value2=blabla&value3=12345678\">Test page 2</a>");
    webServer.print("<br>");
    sendWebServerData();
    webServer.print("<h4> Max cicle time - ");
    webServer.print(maxCicleTime);
    webServer.print("</h4></html>");
}

void testPage2()
{
    sendHeader();
    webServer.print("<h1>Test Page 2</h1>");
    webServer.print("<br>");
    webServer.print(" <a href = \"/\">MainPage</a>");
    webServer.print("<br>");
    webServer.print("<a href=\"/test1\">Test page 1</a>");
    webServer.print("<br>");
    sendWebServerData();
    webServer.print("<h4> Max cicle time - ");
    webServer.print(maxCicleTime);
    webServer.print("</h4></html>");
}

void sendHeader()
{
    webServer.println("HTTP/1.1 200 OK");
    webServer.println("Content-Type: text/html");
    webServer.println("Connection: close");
    webServer.println();
    webServer.println("<!DOCTYPE HTML>");
    webServer.println("<html>");
}

void sendWebServerData()
{
    webServer.print("<h2> Server data</h2>");
    webServer.print("<h3> Main data</h3>");
    webServer.print("<h4> Method - ");
    webServer.print(webServer.method());
    webServer.print("<br> Method version - ");
    webServer.print(webServer.methodVersion());
    webServer.print("<br> Host - ");
    webServer.print(webServer.host());
    webServer.print("<br> URL - ");
    webServer.print(webServer.uri());
    webServer.print("</h4>");
    webServer.print("<h3> Headers </h3>");
    webServer.print("<h4>");
    for (int i = 0; i < webServer.headersCount(); i++)
    {
        String key = webServer.headerKeyAtIndex(i);
        if (webServer.hasHeaderKey(key))
        {
            String value = webServer.headerValueAtKey(key);
            webServer.print(i);
            webServer.print(": ");
            webServer.print(key);
            webServer.print(" -- ");
            webServer.print(value);
            webServer.print("<br>");
        }
    }
    webServer.print("</h4>");
    webServer.print("<h3> Arguments </h3>");
    webServer.print("<h4>");
    for (int i = 0; i < webServer.argumentsCount(); i++)
    {
        String key = webServer.argumentKeyAtIndex(i);
        if (webServer.hasArgumentKey(key))
        {
            String value = webServer.argumentValueAtKey(key);
            webServer.print(i);
            webServer.print(": ");
            webServer.print(key);
            webServer.print(" = ");
            webServer.print(value);
            webServer.print("<br>");
        }
    }
    webServer.print("</h4>");
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
    if (WiznetInterface.getStatus() != ethernetStatus)
    {
        ethernetStatus = WiznetInterface.getStatus();
        Serial.println();
        Serial.print("Статус интерфейса - ");
        Serial.println(flprog::flprogStatusCodeName(ethernetStatus));
    }
    if (WiznetInterface.getError() != ethernetError)
    {
        ethernetError = WiznetInterface.getError();
        if (ethernetError != FLPROG_NOT_ERROR)
        {
            Serial.println();
            Serial.print("Ошибка интерфейса - ");
            Serial.println(flprog::flprogErrorCodeName(ethernetError));
        }
    }
    printConnectMessages();
    printDisconnectMessages();
}

void printConnectMessages()
{
    if (!WiznetInterface.isReady())
    {
        return;
    }
    if (!isNeedSendConnectMessage)
    {
        return;
    }
    Serial.println("Ethernet подключён!");
    Serial.print("Ip - ");
    Serial.println(WiznetInterface.localIP());
    Serial.print("DNS - ");
    Serial.println(WiznetInterface.dns());
    Serial.print("Subnet - ");
    Serial.println(WiznetInterface.subnet());
    Serial.print("Gateway - ");
    Serial.println(WiznetInterface.gateway());
    isNeedSendConnectMessage = false;
    isNeedSendDisconnectMessage = true;
}

void printDisconnectMessages()
{
    if (WiznetInterface.isReady())
    {
        return;
    }
    if (isNeedSendConnectMessage)
    {
        return;
    }
    if (!isNeedSendDisconnectMessage)
    {
        return;
    }
    Serial.println("Ethernet отключён!");
    isNeedSendConnectMessage = true;
    isNeedSendDisconnectMessage = false;
}