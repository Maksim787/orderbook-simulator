#include <parsers/LineParsers.h>
#include <cassert>
#include <string>

// Macro to parse an integer from the string
#define PARSE_INT(dest) \
    do { \
        char* endPtr; \
        dest = strtol(data, &endPtr, 10); \
        assert(errno == 0 && "Integer conversion error"); \
        assert(endPtr != data && "Integer overflow"); \
        assert(endPtr <= data + len && "Integer overflow"); \
        data = endPtr; \
    } while (0)

#define PARSE_UINT64_T(dest) \
    do { \
        char* endPtr; \
        dest = strtoull(data, &endPtr, 10); \
        assert(errno == 0 && "uint64_t conversion error"); \
        assert(endPtr != data && "uint64_t overflow"); \
        assert(endPtr <= data + len && "uint64_t overflow"); \
        data = endPtr; \
    } while (0)

// Macro to parse a float from the string
#define PARSE_FLOAT(dest) \
    do { \
        char* endPtr; \
        dest = strtof(data, &endPtr); \
        assert(errno == 0 && "Float conversion error"); \
        assert(endPtr != data && "Float overflow"); \
        assert(endPtr <= data + len && "Float overflow"); \
        data = endPtr; \
    } while (0)

// Macro to parse a string from the string
#define PARSE_STRING(dest) \
    do { \
        size_t copyLen = 0; \
        while (*data != ',') { \
            char c = *data; \
            dest[copyLen] = c; \
            assert(((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) && "String overflow"); \
            ++copyLen; \
            ++data; \
        } \
        dest[copyLen] = '\0'; \
        assert(copyLen <= MAX_SEC_CODE_LENGTH && "String overflow"); \
    } while (0)

#define PARSE_DIRECTION(dest) \
    switch (*data) { \
        case 'B': \
            dest = Direction::BUY; \
            break; \
        case 'S': \
            dest = Direction::SELL; \
            break; \
        default: \
            assert(false && "Buy/Sell parsing error"); \
    } \
    ++data;

#define PARSE_ACTION(dest) \
    switch (*data) { \
        case '0': \
            dest = Action::CANCEL; \
            break; \
        case '1': \
            dest = Action::PLACE; \
            break; \
        case '2': \
            dest = Action::TRADE; \
            break; \
        default: \
            assert(false && "Action parsing error"); \
    } \
    ++data;

#define PARSE_TIME(dest) \
    do { \
        char* endPtr; \
        uint64_t time = strtoull(data, &endPtr, 10); \
        assert(errno == 0 && "Time overflow"); \
        assert(endPtr != data && "Time overflow"); \
        assert(endPtr <= data + len && "Time overflow"); \
        data = endPtr; \
        uint64_t hours = time / 10000000000; \
        uint64_t minutes = (time / 100000000) % 100; \
        uint64_t seconds = (time / 1000000) % 100; \
        uint64_t milliseconds = (time / 1000) % 1000; \
        uint64_t microseconds = time % 1000; \
        dest = hours * 3600000000ull + minutes * 60000000ull + seconds * 1000000ull + milliseconds * 1000ull + microseconds; \
    } while (0)

#define SKIP_COMMA() \
    assert(*data == ',' && "Expected comma"); \
    ++data;

EventWithInstrument parse_line_with_sec_code(const char* data, size_t len) {
    // 1,ABRD,B,100000000000,1,1,126,40,,
    // id,sec_code,buy/sell,time:HHMMSSZZZXXX,order_id,action:{0,1,2},px,qty,trade_id:optional,trade_px:optional
    const char* const data_start = data;

    EventWithInstrument event;
    PARSE_INT(event.id);
    SKIP_COMMA();

    PARSE_STRING(event.sec_code);
    SKIP_COMMA();

    PARSE_DIRECTION(event.direction);
    SKIP_COMMA();

    PARSE_TIME(event.time_microseconds);
    SKIP_COMMA();

    PARSE_UINT64_T(event.order_id);
    SKIP_COMMA();

    PARSE_ACTION(event.action);
    SKIP_COMMA();

    PARSE_FLOAT(event.px);
    SKIP_COMMA();

    PARSE_INT(event.qty);
    SKIP_COMMA();

    if (event.action == Action::TRADE) {
        PARSE_UINT64_T(event.trade_id);
        SKIP_COMMA();
        PARSE_FLOAT(event.trade_px);
    } else {
        SKIP_COMMA();
    }

    assert(data - data_start == len && "Line parsing error");
    return event;
}