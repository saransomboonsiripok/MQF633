#ifndef BLACKSCHOLESPRICER_H
#define BLACKSCHOLESPRICER_H

#include "Pricer.h"
#include <cmath>

class BlackScholesPricer : public Pricer {
public:
    double Price(const Market& mkt, Trade* trade) override;

private:
    double calculateCallPrice(double S, double K, double T, double r, double sigma) const;
    double calculatePutPrice(double S, double K, double T, double r, double sigma) const;
};

#endif // BLACKSCHOLESPRICER_H
