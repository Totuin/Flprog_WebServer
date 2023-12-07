#pragma once
#include <Arduino.h>
#include "flprogEthernet.h"

#define FLPROG_WEB_SERVER_INACTION_STEP 0
#define FLPROG_WEB_SERVER_READ_FIRST_LINE_STEP 1
#define FLPROG_WEB_SERVER_READ_SECOND_LINE_STEP 2

#define FLPROG_WEB_SERVER_WORK_DATA_STEP_1 3

#define FLPROG_WEB_SERVER_PARSE_POST_STEP_1 4
#define FLPROG_WEB_SERVER_PARSE_POST_STEP_2 5

#define FLPROG_WEB_SERVER_PARSE_GET_STEP_1 6
#define FLPROG_WEB_SERVER_PARSE_GET_STEP_2 7
#define FLPROG_WEB_SERVER_PARSE_GET_STEP_3 8


#define FLPROG_WEB_SERVER_HEADERS_COUNT 50

struct FLProgWebServerRequestArgument
{
    String key;
    String value;
};

struct FLProgWebServerReqest
{
    String reqest;
    uint8_t currentVersion;
    String currentUri;
    bool chunked;
    int clientContentLength;
    String methodStr;
    String searchStr;
    String hostHeader;
    int headerKeysCount = 0;
    FLProgWebServerRequestArgument headers[FLPROG_WEB_SERVER_HEADERS_COUNT];
};

class FLProgWebServer
{
public:
    FLProgWebServer(FLProgAbstractTcpInterface *sourse, uint16_t port = 80);
    void pool();

    uint8_t getStatus() { return _status; };
    uint8_t getError() { return _errorCode; };

private:
    void parseReqest();
    uint8_t readStringUntil(char terminator);
    uint8_t parseGetReqest();
     void parseArguments(String data);

    uint8_t _errorCode = FLPROG_NOT_ERROR;
    uint8_t _status = FLPROG_NOT_REDY_STATUS;
    FLProgEthernetServer _server;
    uint32_t _startReadStringTime;
    uint8_t _step = FLPROG_WEB_SERVER_INACTION_STEP;
    String _readingString = "";
    FLProgWebServerReqest _reqest;
};