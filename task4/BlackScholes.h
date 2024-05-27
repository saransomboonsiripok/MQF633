#ifndef BLACK_SCHOLES_H
#define BLACK_SCHOLES_H

#include <cmath>
#include "Types.h"

double norm_cdf(double x) {
    return 0.5 * erfc(-x * M_SQRT1_2);
}

double BlackScholesPrice(double S, double K, double T, double r, double sigma, OptionType optType) {
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    
    if (optType == Call) {
        return S * norm_cdf(d1) - K * exp(-r * T) * norm_cdf(d2);
    } else {
        return K * exp(-r * T) * norm_cdf(-d2) - S * norm_cdf(-d1);
    }
}

#endif // BLACK_SCHOLES_H
