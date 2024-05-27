#pragma once
#include "Trade.h"
#include <cmath>


class Swap : public Trade {
public:
    //make necessary change
    Swap(Date start, Date end, double _notional, double _tradeRate, double _frequency)
        : startDate(start), maturityDate(end), notional(_notional), tradeRate(_tradeRate), frequency(_frequency), Trade("SwapTrade", start){}
    
    /*
    implement this, using pv = annuity * (trade rate - market rate);
    approximately, annuity = sum of (notional * Discount factor at each period end);
    for a 2 year swap, with annual freq and notinal 100, annuity = 100 * Df(at year 1 end) + 100* Df(at year 2 end);
    Df = exp(-rT), r taken from curve;
    */
    inline double Payoff(double marketRate) const override {
        double annuity = getAnnuity();
        return annuity * (tradeRate - marketRate);
    }
    double getAnnuity() const; //implement this in a cpp file

private:
    Date startDate;
    Date maturityDate;
    double notional;
    double tradeRate;
    double frequency; // use 1 for annual, 2 for semi-annual etc
};