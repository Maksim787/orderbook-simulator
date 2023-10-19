#include <orderbook/Event.h>

#include <iomanip>

std::ostream& operator<<(std::ostream& os, const Event& event) {
    os << "[Event ID: " << event.id << ", ";
    os << "Direction: " << event.direction << ", ";
    os << "Time (microseconds): " << event.time_microseconds << ", ";
    os << "Order ID: " << event.order_id << ", ";
    os << "Action: " << event.action << ", ";
    os << "Price (px): " << event.px << ", ";
    os << "Quantity (qty): " << event.qty << ", ";
    if (event.action == Action::TRADE) {
        os << "Trade ID: " << event.trade_id << ", ";
        os << "Trade Price (trade_px): " << event.trade_px << "]";
    } else {
        os << "Trade ID: null, ";
        os << "Trade Price (trade_px): null]\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const EventWithInstrument& event) {
    os << static_cast<const Event&>(event); // Print the base class fields
    os << "Security Code (" << strlen(event.sec_code) << "): " << event.sec_code << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Direction& direction) {
    switch (direction) {
        case Direction::BUY:
            os << "BUY";
            break;
        case Direction::SELL:
            os << "SELL";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Action& action) {
    switch (action) {
        case Action::CANCEL:
            os << "CANCEL";
            break;
        case Action::PLACE:
            os << "PLACE";
            break;
        case Action::TRADE:
            os << "TRADE";
            break;
    }
    return os;
}

void Event::Serialize(std::ostream& out) const {
    // buy/sell,time:HHMMSSZZZXXX,order_id,action:{0,1,2},px,qty,trade_id:optional,trade_px:optional
    out << (direction == Direction::BUY ? 'B' : 'S') << ','
        << std::setfill('0')
        << std::setw(2) << time_microseconds / 3600000000 // hours
        << std::setw(2) << (time_microseconds / 60000000) % 60 // minutes
        << std::setw(2) << (time_microseconds / 1000000) % 60 // seconds
        << std::setw(3) << (time_microseconds / 1000) % 1000 // milliseconds
        << std::setw(3) << time_microseconds % 1000  // microseconds
        << ','
        << order_id << ','
        << static_cast<int>(action) << ','
        << px << ','
        << qty << ',';
    if (action == Action::TRADE) {
        out << trade_id << "," << trade_px << '\n';
    } else {
        out << ",\n";
    }
}