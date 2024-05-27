#include "Swap.h"
#include "Market.h"

// Function to calculate the annuity
double Swap::getAnnuity() const {
    // Implement the calculation of the annuity
    double annuity = 0.0;
    int periods = (maturityDate.year - startDate.year) * frequency; // Total number of periods
    double periodLength = 1.0 / frequency; // Length of each period in years

    for (int i = 1; i <= periods; ++i) {
        double time = i * periodLength;
        double discountFactor = exp(-tradeRate * time);
        annuity += notional * discountFactor;
    }

    return annuity;
}