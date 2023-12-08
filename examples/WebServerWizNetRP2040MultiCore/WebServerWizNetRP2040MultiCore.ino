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

uint32_t startSendTime;

//=================================================================================================
void setup()
{

    Serial.begin(115200);
    while (!Serial)
    {
    }

    flprog::printConsole(" Тест Modbus Slave TCP - WizNet ");

    Serial.print("CS - ");
    Serial.println(WiznetInterface.pinCs());
    Serial.print("SPI BUS - ");
    Serial.println(WiznetInterface.spiBus());
    pinMode(LED_BUILTIN, OUTPUT);
}
void setup1()
{
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
    webServer.add404Page(page_404);
}

//=================================================================================================
void loop()
{
    printStatusMessages();
    blinkLed();
}

void loop1()
{

    WiznetInterface.pool();
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