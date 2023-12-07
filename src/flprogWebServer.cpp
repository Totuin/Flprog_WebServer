#include "flprogWebServer.h"

FLProgWebServer::FLProgWebServer(FLProgAbstractTcpInterface *sourse, uint16_t port)
{
    _server.setSourse(sourse);
    _server.setPort(port);
    _status = FLPROG_READY_STATUS;
}

void FLProgWebServer::pool()
{
    if (_server.getSourse() == 0)
    {
        return;
    }
    if (!_server.getSourse()->isReady())
    {
        return;
    }
    if (_status == FLPROG_WAIT_WEB_SERVER_READ_REQEST)
    {
        parseReqest();
        return;
    }
    if (!_server.connected())
    {
        return;
    }
    if (!_server.available())
    {
        return;
    }
    _reqest.headerKeysCount = 0;
    parseReqest();
}

void FLProgWebServer::parseReqest()
{
    if (_status != FLPROG_WAIT_WEB_SERVER_READ_REQEST)
    {
        _startReadStringTime = millis();
        _readingString = "";
        _status = FLPROG_WAIT_WEB_SERVER_READ_REQEST;
        _step = FLPROG_WEB_SERVER_READ_FIRST_LINE_STEP;
    }
    uint8_t result;
    if (_step == FLPROG_WEB_SERVER_READ_FIRST_LINE_STEP)
    {
        result = readStringUntil('\r');
        if (result == FLPROG_WITE)
        {
            return;
        }
        _reqest.reqest = _readingString;
        _readingString = "";
        _step = FLPROG_WEB_SERVER_READ_SECOND_LINE_STEP;
    }
    if (_step == FLPROG_WEB_SERVER_READ_SECOND_LINE_STEP)
    {
        result = readStringUntil('\n');
        if (result == FLPROG_WITE)
        {
            return;
        }
        _readingString = "";
        _step = FLPROG_WEB_SERVER_WORK_DATA_STEP_1;
    }

    if (_step == FLPROG_WEB_SERVER_WORK_DATA_STEP_1)
    {
        int addr_start = _reqest.reqest.indexOf(' ');
        int addr_end = _reqest.reqest.indexOf(' ', addr_start + 1);
        _reqest.methodStr = _reqest.reqest.substring(0, addr_start);
        _reqest.currentUri = _reqest.reqest.substring(addr_start + 1, addr_end);
        _reqest.currentVersion = atoi((_reqest.reqest.substring(addr_end + 8)).c_str());
        _reqest.searchStr = "";
        int hasSearch = _reqest.currentUri.indexOf('?');
        if (hasSearch != -1)
        {
            _reqest.searchStr = _reqest.currentUri.substring(hasSearch + 1);
            _reqest.currentUri = _reqest.currentUri.substring(0, hasSearch);
        }
        _reqest.chunked = false;
        _reqest.clientContentLength = 0;
        _readingString = "";
        if (_reqest.methodStr == "POST" || _reqest.methodStr == "PUT" || _reqest.methodStr == "PATCH" || _reqest.methodStr == "DELETE")
        {
            _step = FLPROG_WEB_SERVER_PARSE_POST_STEP_1;
        }
        else
        {
            _step = FLPROG_WEB_SERVER_PARSE_GET_STEP_1;
        }
    }

    if ((_step == FLPROG_WEB_SERVER_PARSE_POST_STEP_1) || (_step == FLPROG_WEB_SERVER_PARSE_POST_STEP_2))
    {
        _step = FLPROG_WEB_SERVER_INACTION_STEP;
    }

    if ((_step == FLPROG_WEB_SERVER_PARSE_GET_STEP_1) || (_step == FLPROG_WEB_SERVER_PARSE_GET_STEP_2))
    {
        result = parseGetReqest();
        if (result == FLPROG_WITE)
        {
            return;
        }
    }

    _step = FLPROG_WEB_SERVER_INACTION_STEP;
    _status = FLPROG_READY_STATUS;
    _server.println("HTTP/1.1 200 OK");
    _server.println("Content-Type: text/html");
    _server.println("Connection: close");
    _server.println();
    _server.println("<!DOCTYPE HTML>");
    _server.println("<html>");
    _server.println("AAAAAAAAAAAAAAAAAAAA");
    _server.println("</html>");
    _server.stopConnection();

    Serial.println(_reqest.reqest);
    Serial.println(_reqest.currentVersion);
    Serial.println(_reqest.currentUri);
    Serial.println(_reqest.methodStr);
    Serial.println(_reqest.searchStr);
    for (int i = 0; i < _reqest.headerKeysCount; i++)
    {
        Serial.print(_reqest.headers[i].key);
        Serial.print(" --- ");
        Serial.print(_reqest.headers[i].value);
    }
    Serial.println();
}

uint8_t FLProgWebServer::parseGetReqest()
{
    uint8_t result;
    if (_step == FLPROG_WEB_SERVER_PARSE_GET_STEP_1)
    {
        result = readStringUntil('\r');
        if (result == FLPROG_WITE)
        {
            return FLPROG_WITE;
        }
        _reqest.reqest = _readingString;
        _readingString = "";
        _step = FLPROG_WEB_SERVER_PARSE_GET_STEP_2;
    }
    if (_step == FLPROG_WEB_SERVER_PARSE_GET_STEP_2)
    {
        result = readStringUntil('\r');
        if (result == FLPROG_WITE)
        {
            return FLPROG_WITE;
        }
        _readingString = "";
        if (_reqest.reqest == "")
        {
            parseArguments(_reqest.searchStr);
            return FLPROG_SUCCESS;
        }
    }
    String headerName;
    String headerValue;
    int headerDiv = _reqest.reqest.indexOf(':');
    if (headerDiv == -1)
    {
        parseArguments(_reqest.searchStr);
        return FLPROG_SUCCESS;
    }
    headerName = _reqest.reqest.substring(0, headerDiv);
    headerValue = _reqest.reqest.substring(headerDiv + 2);
    if (headerName.equalsIgnoreCase("Host"))
    {
        _reqest.hostHeader = headerValue;
    }
    bool hasHeader = false;
    for (int i = 0; i < _reqest.headerKeysCount; i++)
    {
        if (_reqest.headers[i].key.equalsIgnoreCase(headerName))
        {
            _reqest.headers[i].value = headerValue;
            hasHeader = true;
            break;
        }
    }
    if (!hasHeader)
    {
        if (_reqest.headerKeysCount < FLPROG_WEB_SERVER_HEADERS_COUNT)
        {
            _reqest.headers[_reqest.headerKeysCount].key = headerName;
            _reqest.headers[_reqest.headerKeysCount].value = headerValue;
            _reqest.headerKeysCount++;
        }
    }
    _step = FLPROG_WEB_SERVER_PARSE_GET_STEP_1;
    return FLPROG_WITE;
}

void FLProgWebServer::parseArguments(String data)
{
}

uint8_t FLProgWebServer::readStringUntil(char terminator)
{
    if (flprog::isTimer(_startReadStringTime, 10))
    {
        return FLPROG_SUCCESS;
    }
    int readChar;
    while (_server.available())
    {
        _startReadStringTime = millis();
        readChar = _server.read();
        if (readChar == terminator)
        {
            return FLPROG_SUCCESS;
        }
        else
        {
            _readingString += (char)readChar;
        }
    }
    return FLPROG_WITE;
}