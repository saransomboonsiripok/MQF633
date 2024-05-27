#pragma once
#include "Trade.h"
#include <string>

class Bond : public Trade {
public:
    Bond(Date start, Date end, double _notional, double _price, const std::string& _underlying): Trade("BondTrade", start) {
        startDate = start;
        maturityDate = end;
        notional = _notional;
        tradePrice = _price;
        underlying = _underlying;
    }

    inline double Payoff(double marketPrice) const override{
        return (marketPrice - tradePrice);
    } // implement this

    std::string getUnderlying() const { return underlying; }
    Date getIssueDate() const { return startDate; }
    Date getMaturityDate() const { return maturityDate; }
    double getTradePrice() const { return tradePrice; }

private:
    double notional;
    double tradePrice;
    Date startDate;
    Date maturityDate;
    std::string underlying;
};