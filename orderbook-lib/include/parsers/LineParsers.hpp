#include <cassert>
#include <string>

// Macro to parse an integer from the string
#define PARSE_INT(dest) \
    do { \
        char* endPtr; \
        errno = 0; \
        dest = strtol(data, &endPtr, 10); \
        assert(errno == 0 && "Integer conversion error"); \
        assert(endPtr != data && "Integer overflow"); \
        assert(endPtr <= data + len && "Integer overflow"); \
        data = endPtr; \
    } while (0)

#define PARSE_UINT64_T(dest) \
    do { \
        char* endPtr; \
        errno = 0; \
        dest = strtoull(data, &endPtr, 10); \
        assert(errno == 0 && "uint64_t conversion error"); \
        assert(endPtr != data && "uint64_t overflow"); \
        assert(endPtr <= data + len && "uint64_t overflow"); \
        data = endPtr; \
    } while (0)

// Macro to parse a double from the string
#define PARSE_DOUBLE(dest) \
    do { \
        char* endPtr; \
        errno = 0; \
        dest = strtod(data, &endPtr); \
        assert(errno == 0 && "Double conversion error"); \
        assert(endPtr != data && "Double overflow"); \
        assert(endPtr <= data + len && "Double overflow"); \
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
        errno = 0; \
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
        assert(hours <= 23); \
        dest = hours * 3600000000ull + minutes * 60000000ull + seconds * 1000000ull + milliseconds * 1000ull + microseconds; \
    } while (0)

#define SKIP_COMMA() \
    assert(*data == ',' && "Expected comma"); \
    ++data;

template <class EventType>
EventType parse_event_from_line(const char* data, size_t len) {
    static_assert(std::is_same_v<EventType, Event> || std::is_same_v<EventType, EventWithInstrument>);
    // 1,ABRD,B,100000000000,1,1,126,40,,
    // id,sec_code,buy/sell,time:HHMMSSZZZXXX,order_id,action:{0,1,2},px,qty,trade_id:optional,trade_px:optional
    const char* const data_start = data;

    EventType event;
    PARSE_INT(event.id);
    assert(event.id >= 1);
    SKIP_COMMA();

    if constexpr (std::is_same_v<EventType, EventWithInstrument>) {
        PARSE_STRING(event.sec_code);
        SKIP_COMMA();
    }

    PARSE_DIRECTION(event.direction);
    SKIP_COMMA();

    PARSE_TIME(event.time_microseconds);
    SKIP_COMMA();

    PARSE_UINT64_T(event.order_id);
    SKIP_COMMA();

    PARSE_ACTION(event.action);
    SKIP_COMMA();

    PARSE_DOUBLE(event.px);
    assert(event.px >= 0);
    SKIP_COMMA();

    PARSE_INT(event.qty);
    SKIP_COMMA();

    if (event.action == Action::TRADE) {
        PARSE_UINT64_T(event.trade_id);
        SKIP_COMMA();
        PARSE_DOUBLE(event.trade_px);
        assert(event.trade_px > 0);
    } else {
        SKIP_COMMA();
    }

    assert(data - data_start == len && "Line parsing error");
    return event;
}