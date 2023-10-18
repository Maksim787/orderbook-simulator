#pragma once

#include <cstring>
#include <ostream>

enum class Direction {
    BUY = 1,
    SELL = -1
};

enum class Action {
    CANCEL = 0,
    PLACE = 1,
    TRADE = 2
};

struct Event {
    int id; // unique row id
    Direction direction;
    uint64_t time_microseconds;
    uint64_t order_id; // unique order id
    Action action;
    float px;

    // action = Cancel: qty = remaining qty
    // action = Place:  qty = placed qty
    // action = Trade:  qty = trade qty
    int qty;

    // Only for action = Trade
    uint64_t trade_id;
    float trade_px;

    void Serialize(std::ostream& out) const;
};

constexpr int MAX_SEC_CODE_LENGTH = 15;

struct EventWithInstrument : Event {
    char sec_code[MAX_SEC_CODE_LENGTH + 1];
};

std::ostream& operator<<(std::ostream& os, const Direction& direction);

std::ostream& operator<<(std::ostream& os, const Action& direction);

std::ostream& operator<<(std::ostream& os, const Event& event);

std::ostream& operator<<(std::ostream& os, const EventWithInstrument& event);