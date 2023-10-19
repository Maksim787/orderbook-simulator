#pragma once

// Data headers
const std::string MULTIPLE_INSTRUMENTS_HEADER = "NO,SECCODE,BUYSELL,TIME,ORDERNO,ACTION,PRICE,VOLUME,TRADENO,TRADEPRICE";
const std::string ONE_INSTRUMENT_HEADER = "NO,BUYSELL,TIME,ORDERNO,ACTION,PRICE,VOLUME,TRADENO,TRADEPRICE";

/// Only for tests
// File name for test
const std::string MULTIPLE_INSTRUMENTS_TEST_FILENAME = "data/examples/SE/OrderLog20200103.txt";
const std::string SINGLE_INSTRUMENT_TEST_FILENAME = "data/examples/SE_by_instrument/ABRD/OrderLog20200103.txt";

const size_t ORDERBOOK_MAX_SIZE = 10000;