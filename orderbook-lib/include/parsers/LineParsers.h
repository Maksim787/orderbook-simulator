#pragma once

#include <orderbook/Event.h>

template <class EventType>
EventType parse_event_from_line(const char* data, size_t len);

#include <parsers/LineParsers.hpp>