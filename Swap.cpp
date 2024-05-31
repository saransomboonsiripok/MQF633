#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "Swap.h"
#include "Market.h"
#include "Date.h"
#include <ctime>

// Function to calculate the annuity
double Swap::PayoffSwap(Market mkt) const {
    // 1 - get current date
    std::time_t t = std::time(0);
    auto now_ = localtime(&t);
    Date current_date;
    current_date.year = now_->tm_year + 1900;
    current_date.month = now_->tm_mon + 1;
    current_date.day = now_->tm_mday;

    // 2 - find payment date
    double interval_year = 1 / frequency;
    double num_payment = (maturityDate - startDate) * frequency;
    vector<Date> payment_dates;
    Date start = startDate;
    if (frequency == 2){
        for (double i = 0.5; i <= num_payment/2; i += 0.5){
            if (start.month + (i * 12) <= 12){
                Date payment_date;
                payment_date.year = start.year;
                payment_date.month = start.month + (i * 12);
                payment_date.day = start.day;
                payment_dates.push_back(payment_date);
        }
            else{
                Date payment_date;
                payment_date.year = start.year + i;
                if (fmod(i,1) != 0){
                    if (start.month + (fmod(i,1) * 12) > 12) {
                        payment_date.year += 1;
                        payment_date.month = (start.month + (fmod(i,1) * 12)) - 12;
                    }
                    else {
                        payment_date.month = start.month + (fmod(i,1) * 12);
                    }
                }
                else{
                    payment_date.month = start.month;
                }
                payment_date.day = start.day;
                payment_dates.push_back(payment_date);
            }
    }
    }
    else{
        for (double i = 1; i <= num_payment; i += 1){
            Date payment_date;
            payment_date.year = start.year + i;
            payment_date.month = start.month;
            payment_date.day = start.day;
            payment_dates.push_back(payment_date);
    }
    }

    // 3 - eliminate the payment date that is before current date
    payment_dates.erase(
        std::remove_if(payment_dates.begin(), payment_dates.end(), [current_date](const Date& date){
            return date < current_date;
        }),
        payment_dates.end()
    );
    
    // 4 - calculate the number of months from today 
    vector <Date> tenors;
    for (const auto& date: payment_dates){
        double period;
        period = (date - current_date) * 12;
        Date period_date;
        period_date.month = period;
        period_date.day = 0;
        period_date.year = 0;
        tenors.push_back(period_date);
    }
    
    // 5 - get discount factor
    vector <double> dfs;
    for (const auto& tenor: tenors){
        double int_rate = mkt.curves["USD-SOFR"].getRate(tenor);
        double month = tenor.month;
        double df = exp(-int_rate * (month /12));
        dfs.push_back(df);
    }

    // 6 - calculate pv
    double sum = 0;
    for (const auto& df: dfs){
        sum += df;
    }
    double annuity = sum * notional;
    double market_rate = mkt.curves["USD-SOFR"].getRate(Date(0,0,0));
    double swap_price = annuity * (tradeRate - market_rate);
    return swap_price + (notional * dfs.back());
}