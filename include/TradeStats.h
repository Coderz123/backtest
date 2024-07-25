#ifndef TRADESTATS_H_
#define TRADESTATS_H_

#include <string>
#include <vector>
#include <map>

enum CCY {
  EURUSD,
  GBPUSD,
  USDJPY,
  USDCAD,
  AUDUSD,
  USDCHF,
  USDZAR,
  GBPJPY,
  EURJPY,
  EURGBP
};

class Trade {
    public:
    Trade(){}
    ~Trade(){}
    double m_tradeTime = 0;
    double m_closeTime = 0;
    double m_openPrice = 0;
    double m_closePrice = 0;
    bool m_tpTime = false;
    double m_takeProfit = 0;
    double m_stopLoss = 0;
    double m_takeProfitTime = 0;
    double m_stopLossTime = 0;
    CCY m_instrument = EURUSD;
    double m_Profit = 0;
    bool m_isBuyTrade = true;
    double m_tradeId = 0.0;
    bool m_isOpen = false;
};

class TradeStats {
    public:
    TradeStats(){}
    ~TradeStats(){}
    void getResults(std::vector<Trade> _trades){};
};

#endif // TRADESTATS_H_
