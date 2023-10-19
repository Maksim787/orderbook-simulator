
#include "OrderBook.h"

template <size_t MAX_SIZE, bool IsBid>
void OneSideOrderBook<MAX_SIZE, IsBid>::Update(const Event& event) {
    // TODO:
}

template <size_t MAX_SIZE>
OrderBook<MAX_SIZE>::OrderBook(double px_step) : m_px_step(px_step) {
    assert(px_step > 0);
    // TODO:
}

template <size_t MAX_SIZE>
void OrderBook<MAX_SIZE>::Update(const Event& event) {
    bid.Update(event);
    ask.Update(event);
}

template <size_t MAX_SIZE>
void OrderBook<MAX_SIZE>::Print() const {
    // TODO:
}
