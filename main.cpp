#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <ctime>
#include <algorithm>

#include "Market.h"
#include "Pricer.h"
#include "EuropeanTrade.h"
#include "Bond.h"
#include "Swap.h"
#include "AmericanTrade.h"
#include "BlackScholesPricer.h" // Black Scholes for task 4

using namespace std;

int getMonths(const string& tenor) {
    if (tenor == "ON") return 0; // Treat overnight as 0 months
    if (tenor.back() == 'M') return stoi(tenor.substr(0, tenor.size() - 1));
    if (tenor.back() == 'Y') return stoi(tenor.substr(0, tenor.size() - 1)) * 12;
    throw std::invalid_argument("Invalid tenor format");
}

void readCurveData(const string& fileName, Market& market) {
    ifstream file(fileName);
    string line;
    string curveName = "USD-SOFR"; // Set the curve name directly
    RateCurve rateCurve(curveName);

    getline(file, line);

    while (getline(file, line)) {
        istringstream iss(line);
        string date;
        string rateStr;
        double rate;

        try {
            iss >> date;
            if (date.back() == ':') date.pop_back();  // Remove trailing colon
            iss >> rateStr;
            rateStr.erase(remove(rateStr.begin(), rateStr.end(), '%'), rateStr.end()); // Remove percentage sign
            rate = stod(rateStr) / 100.0;  // Convert to a fraction
            int months = getMonths(date);
            rateCurve.addRate(Date(0, months, 0), rate);
        } catch (const std::invalid_argument& e) {
            cerr << "Invalid data in curve file: " << line << endl;
        }
    }
    market.addCurve(curveName, rateCurve);
}

void readVolData(const string& fileName, Market& market) {
    ifstream file(fileName);
    string line;
    string curveName = "defaultVolCurve";
    VolCurve volCurve(curveName);

    while (getline(file, line)) {
        istringstream iss(line);
        string date;
        string volStr;
        double vol;

        try {
            iss >> date;
            if (date.back() == ':') date.pop_back();  // Remove trailing colon
            iss >> volStr;
            volStr.erase(remove(volStr.begin(), volStr.end(), '%'), volStr.end()); // Remove percentage sign
            vol = stod(volStr) / 100.0;  // Convert to a fraction
            int months = getMonths(date);
            volCurve.addVol(Date(0, months, 0), vol);
        } catch (const std::invalid_argument& e) {
            cerr << "Invalid data in vol file: " << line << endl;
        }
    }
    market.addVolCurve(curveName, volCurve);
}


void readBondPriceData(const string& fileName, Market& market) {
    ifstream file(fileName);
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        string bondName;
        double price;
        try {
            iss >> bondName;
            if (bondName.back() == ':') bondName.pop_back();
            iss >> price;
            market.addBondPrice(bondName, price);
        } catch (const std::invalid_argument& e) {
            cerr << "Invalid data in bond price file: " << line << endl;
        }
    }
}

void readStockPriceData(const string& fileName, Market& market) {
    ifstream file(fileName);
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        string stockName;
        double price;
        try {
            iss >> stockName;
            if (stockName.back() == ':') stockName.pop_back();
            iss >> price;
            market.addStockPrice(stockName, price);
        } catch (const std::invalid_argument& e) {
            cerr << "Invalid data in stock price file: " << line << endl;
        }
    }
}

int main()
{
  //task 1, create an market data object, and update the market data from from txt file 
  cout << "Task 1 - Market Data" << endl;
  std::time_t t = std::time(0);
  auto now_ = std::localtime(&t);
  Date valueDate;
  valueDate.year = now_->tm_year + 1900;
  valueDate.month = now_->tm_mon + 1;
  valueDate.day = now_->tm_mday;

  Market mkt = Market(valueDate);
  /*
  load data from file and update market object with data
  */
  readCurveData("curve.txt", mkt);
  readVolData("vol.txt", mkt);
  readBondPriceData("bondPrice.txt", mkt);
  readStockPriceData("stockPrice.txt", mkt);

  mkt.Print();
  
  //task 2, create a portfolio of bond, swap, european option, american option
  //for each time, at least should have long / short, different tenor or expiry, different underlying
  //totally no less than 16 trades
  vector<Trade*> myPortfolio;

  // Add bonds
  myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5, "SGD-GOV"));
  myPortfolio.push_back(new Bond(Date(2025, 1, 1), Date(2035, 1, 1), 20000000, 105.0, "USD-GOV"));
  myPortfolio.push_back(new Bond(Date(2024, 6, 1), Date(2034, 6, 1), 15000000, 104.0, "SGD-MAS-BILL"));
  myPortfolio.push_back(new Bond(Date(2023, 1, 1), Date(2033, 1, 1), 12000000, 102.0, "SGD-GOV"));

  // Add swaps
  myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2029, 1, 1), 15000000, 0.05, 1));
  myPortfolio.push_back(new Swap(Date(2025, 1, 1), Date(2030, 1, 1), 25000000, 0.045, 1));
  myPortfolio.push_back(new Swap(Date(2026, 1, 1), Date(2031, 1, 1), 20000000, 0.04, 2));
  myPortfolio.push_back(new Swap(Date(2027, 1, 1), Date(2032, 1, 1), 10000000, 0.035, 4));

  // Add European options
  myPortfolio.push_back(new EuropeanOption(Call, 100, Date(2024, 12, 31)));
  myPortfolio.push_back(new EuropeanOption(Put, 100, Date(2025, 12, 31)));
  myPortfolio.push_back(new EuropeanOption(Call, 105, Date(2026, 6, 30)));
  myPortfolio.push_back(new EuropeanOption(Put, 95, Date(2027, 6, 30)));

  // Add American options
  myPortfolio.push_back(new AmericanOption(Call, 100, Date(2024, 12, 31)));
  myPortfolio.push_back(new AmericanOption(Put, 100, Date(2025, 12, 31)));
  myPortfolio.push_back(new AmericanOption(Call, 110, Date(2026, 12, 31)));
  myPortfolio.push_back(new AmericanOption(Put, 90, Date(2027, 12, 31)));
  
  //task 3, creat a pricer and price the portfolio, output the pricing result of each deal.
  Pricer* treePricer = new CRRBinomialTreePricer(10);
  Pricer* blackScholesPricer = new BlackScholesPricer(); // Define blackScholesPricer here (for task 4)
  for (auto trade: myPortfolio) {
    double pv = treePricer->Price(mkt, trade);
    cout << trade->getType() << endl;
    //log pv details out in a file

  }

  //task 4, analyzing pricing result
  // a) compare CRR binomial tree result for an european option vs Black model
  // b) compare CRR binomial tree result for an american option vs european option

cout << "Task 4a - Comparison between CRR Binomial Tree and Black-Scholes for European options" << endl;
    for (auto trade: myPortfolio) {
        const EuropeanOption* option = dynamic_cast<const EuropeanOption*>(trade);
        if (option) {
            double pvCRR = treePricer->Price(mkt, trade);
            double pvBS = blackScholesPricer->Price(mkt, trade);
            cout << "European Option " << option->getUnderlying() << " " << (option->isCall() ? "Call" : "Put")
                 << " CRR Price: " << pvCRR << " Black-Scholes Price: " << pvBS << endl;
        }
    }
  //final
  cout << "Project build successfully!" << endl;
  return 0;

}
