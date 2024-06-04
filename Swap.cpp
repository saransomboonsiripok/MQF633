#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include "Swap.h"
#include "Market.h"
#include "Date.h"
#include <ctime>

double Swap::getAnnuity(const Market& mkt) const {
    // 1 - get current date
    std::time_t t = std::time(0);
    auto now_ = localtime(&t);
    Date current_date;
    current_date.year = now_->tm_year + 1900;
    current_date.month = now_->tm_mon + 1;
    current_date.day = now_->tm_mday;

    //2 - find payment date
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

    //3 - eliminate the payment date that is before current date
    payment_dates.erase(
        std::remove_if(payment_dates.begin(), payment_dates.end(), [current_date](const Date& date){
            return date < current_date;
        }),
        payment_dates.end()
    );

    //4 - calculate the number of month from today
    vector <Date> tenors;
    for (const auto& date: payment_dates){
        int period;
        int year_dif = date.year - current_date.year;
        int month_dif = date.month - current_date.month;
        int day_dif = date.day - current_date.day;
        period = ((year_dif) * 360) + (month_dif * 30) + (day_dif);
        Date period_date;
        period_date.day = period;
        period_date.month = 0;
        period_date.year = 0;
        tenors.push_back(period_date);
    }

    //5 - get discount factor from the number of month
    vector <double> dfs;
    for (const auto& tenor: tenors){
        double int_rate = mkt.getCurve("USD-SOFR").getRate(Date(tenor.day, tenor.month, tenor.year));
        double day = tenor.day;
        double df = exp(-int_rate * (day / 365));
        dfs.push_back(df);
    }

    //6 - calculate annuity = notional * (sum of discount factor)
    double sum = 0;
    for (const auto& df: dfs){sum += df;}
    double annuity = sum * notional;
    // unmute to check the calculation 
    // for (auto& date: payment_dates){
    //     cout << date.day << " " << date.month << " " << date.year << endl;
    // }
    // for (auto& tenor: tenors){
    //     cout << tenor.day << " " << tenor.month << " " << tenor.year << endl;
    // }
    // for (auto& df: dfs){
    //     cout << std::setprecision(6) << df << endl;
    // }
    // cout << sum << endl;
    // cout << annuity << endl;
     return annuity;
}