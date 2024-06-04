#pragma once
#include "Trade.h"

class Bond : public Trade {
public:
    Bond(Date start, Date end, double _notional, double _price, const string& _underlying): Trade("BondTrade", start) {
        notional = _notional;
        tradePrice = _price;
        underlying = _underlying;
        startDate = start;
        maturityDate = end;
    }

    string getUnderlying() const {return underlying;};
    double getTradeprice() const {return tradePrice;};
    double getNotional() const {return notional;};
    Date getStarting() const {return startDate;};
    Date getMaturity() const {return maturityDate;};
    inline double Payoff(double marketPrice) const {
       return ((marketPrice / 100) * notional);
    }
    
    inline double Payoffswap(double market_rate, const Market&mkt) const {
        return 0.0;
    }

private:
    double notional;
    double tradePrice;
    Date startDate;
    Date maturityDate;
    string underlying;
};