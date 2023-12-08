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

/* Request Methods */
#define FLPROG_WEB_SERVER_DELETE 0
#define FLPROG_WEB_SERVER_GET 1
#define FLPROG_WEB_SERVER_HEAD 2
#define FLPROG_WEB_SERVER_POST 3
#define FLPROG_WEB_SERVER_PUT 4
/* pathological */
#define FLPROG_WEB_SERVER_CONNECT 5
#define FLPROG_WEB_SERVER_OPTIONS 6
#define FLPROG_WEB_SERVER_TRACE 7
/* WebDAV */
#define FLPROG_WEB_SERVER_COPY 8
#define FLPROG_WEB_SERVER_LOCK 9
#define FLPROG_WEB_SERVER_MKCOL 10
#define FLPROG_WEB_SERVER_MOVE 11
#define FLPROG_WEB_SERVER_PROPFIND 12
#define FLPROG_WEB_SERVER_PROPPATCH 13
#define FLPROG_WEB_SERVER_SEARCH 14
#define FLPROG_WEB_SERVER_UNLOCK 15
#define FLPROG_WEB_SERVER_BIND 16
#define FLPROG_WEB_SERVER_REBIND 17
#define FLPROG_WEB_SERVER_UNBIND 18
#define FLPROG_WEB_SERVER_ACL 19
/* subversion */
#define FLPROG_WEB_SERVER_REPORT 20
#define FLPROG_WEB_SERVER_MKACTIVITY 21
#define FLPROG_WEB_SERVER_CHECKOUT 22
#define FLPROG_WEB_SERVER_MERGE 23
/* upnp */
#define FLPROG_WEB_SERVER_MSEARCH 24
#define FLPROG_WEB_SERVER_NOTIFY 25
#define FLPROG_WEB_SERVER_SUBSCRIBE 26
#define FLPROG_WEB_SERVER_UNSUBSCRIBE 27
/* RFC-5789 */
#define FLPROG_WEB_SERVER_PATCH 28
#define FLPROG_WEB_SERVER_PURGE 29
/* CalDAV */
#define FLPROG_WEB_SERVER_MKCALENDAR 30
/* RFC-2068, section 19.6.1.2 */
#define FLPROG_WEB_SERVER_LINK 31
#define FLPROG_WEB_SERVER_UNLINK 33

struct FLProgWebServerRequestArgument
{
    String key;
    String value;
};

struct FLProgWebServerReqest
{
    uint8_t currentVersion;
    String currentUri;
    bool chunked;
    int clientContentLength;
    uint8_t method;
    String hostHeader;
    FLProgWebServerRequestArgument *headers;
    int headerKeysCount = 0;
    FLProgWebServerRequestArgument *currentArgs;
    int currentArgCount;
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
    String urlDecode(const String &text);
    void addHeader(String headerName, String headerValue);
    uint8_t getHttpMethodCode(String method);

    uint8_t _errorCode = FLPROG_NOT_ERROR;
    uint8_t _status = FLPROG_NOT_REDY_STATUS;
    FLProgEthernetServer _server;
    uint32_t _startReadStringTime;
    uint8_t _step = FLPROG_WEB_SERVER_INACTION_STEP;
    String _readingString = "";
    FLProgWebServerReqest _reqest;
    String _reqestString;
    String _searchStr;
};
