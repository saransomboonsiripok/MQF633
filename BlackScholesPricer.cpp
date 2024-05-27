#include "BlackScholesPricer.h"
#include "Market.h"
#include "EuropeanTrade.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

// Cumulative distribution function for standard normal distribution
double normcdf(double value) {
    return 0.5 * std::erfc(-value * M_SQRT1_2);
}

double BlackScholesPricer::Price(const Market& mkt, Trade* trade) {
    const EuropeanOption* option = dynamic_cast<const EuropeanOption*>(trade);
    if (!option) {
        throw std::invalid_argument("Trade is not a EuropeanOption");
    }

    std::string underlying = option->getUnderlying();
    double S = mkt.getStockPrice(underlying);
    double K = option->getStrike();
    double T = option->getTimeToMaturity(mkt.asOf);
    double r = mkt.getCurve("USD-SOFR").getRate(Date(0, static_cast<int>(T * 12), 0));
    double sigma = mkt.getVolCurve("defaultVolCurve").getVol(Date(0, static_cast<int>(T * 12), 0));

    if (option->isCall()) {
        return calculateCallPrice(S, K, T, r, sigma);
    } else {
        return calculatePutPrice(S, K, T, r, sigma);
    }
}

double BlackScholesPricer::calculateCallPrice(double S, double K, double T, double r, double sigma) const {
    double d1 = (std::log(S / K) + (r + sigma * sigma / 2) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return S * normcdf(d1) - K * std::exp(-r * T) * normcdf(d2);
}

double BlackScholesPricer::calculatePutPrice(double S, double K, double T, double r, double sigma) const {
    double d1 = (std::log(S / K) + (r + sigma * sigma / 2) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return K * std::exp(-r * T) * normcdf(-d2) - S * normcdf(-d1);
}
