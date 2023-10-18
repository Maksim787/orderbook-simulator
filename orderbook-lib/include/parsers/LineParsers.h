#pragma once

#include <orderbook/Event.h>

EventWithInstrument parse_line_with_sec_code(const char* data, size_t len);

