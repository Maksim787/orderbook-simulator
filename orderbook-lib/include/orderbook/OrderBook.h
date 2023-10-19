#pragma once

#include <orderbook/Event.h>

template <size_t MAX_SIZE>
class OrderBook;

template <size_t MAX_SIZE, bool IsBid>
class OneSideOrderBook {
private:
    friend class OrderBook<MAX_SIZE>;

    void Update(const Event& event);
};

template <size_t MAX_SIZE>
class OrderBook {
private:
    double m_px_step;
public:
    OneSideOrderBook<MAX_SIZE, true> bid;
    OneSideOrderBook<MAX_SIZE, false> ask;

public:
    explicit OrderBook(double px_step);

    void Update(const Event& event);

    void Print() const;
};

#include <orderbook/OrderBook.hpp>