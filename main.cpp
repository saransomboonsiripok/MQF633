#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <ctime>
#include <algorithm>
#include <iomanip>

#include "Market.h"
#include "Pricer.h"
#include "EuropeanTrade.h"
#include "Bond.h"
#include "Swap.h"
#include "AmericanTrade.h"
#include "BlackScholes.h"

using namespace std;

int getDays(const string& tenor){
    if (tenor == "ON") return 0; // Treat overnight as 0 months
    if (tenor.back() == 'M') return stoi(tenor.substr(0, tenor.size() - 1)) * 30;
    if (tenor.back() == 'Y') return stoi(tenor.substr(0, tenor.size() - 1)) * 360;
    throw std::invalid_argument("Invalid tenor format");
}

int getMonths(const string& tenor){
    if (tenor == "ON") return 0; // Treat overnight as 0 months
    if (tenor.back() == 'M') return stoi(tenor.substr(0, tenor.size() - 1));
    if (tenor.back() == 'Y') return stoi(tenor.substr(0, tenor.size() - 1)) * 12;
    throw std::invalid_argument("Invalid tenor format");
}

void readCurveData(const string& fileName, Market& market){
  ifstream file(fileName);
  string line;
  string curveName = "USD-SOFR";
  RateCurve rateCurve(curveName);

  getline(file, line);
  while (getline(file, line)){
    istringstream iss(line);
    string date;
    string rateStr;
    double rate;
    
    try{
      iss >> date;
      if (date.back() == ':') date.pop_back();
      iss >> rateStr;
      rateStr.erase(remove(rateStr.begin(), rateStr.end(), '%'), rateStr.end());
      rate = stod(rateStr) / 100.0;
      int days = getDays(date);
      rateCurve.addRate(Date(days, 0, 0), rate);
      //int months = getMonths(date);
      //rateCurve.addRate(Date(0, months, 0), rate);
    } catch (const std::invalid_argument& e){
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
            int days = getDays(date);
            volCurve.addVol(Date(days, 0, 0), vol);
            // int months = getMonths(date);
            // volCurve.addVol(Date(0, months, 0), vol);
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
  ofstream resultFile("output.txt");
  resultFile << "QF633 - Project 1 - Output File" << "\n" << "\n";
  //task 1, create an market data object, and update the market data from from txt file 

  std::time_t t = std::time(0);
  auto now_ = std::localtime(&t);
  Date valueDate;
  valueDate.year = now_->tm_year + 1900;
  valueDate.month = now_->tm_mon + 1;
  valueDate.day = now_->tm_mday;

  Market mkt = Market(valueDate);
  // 1) curve data:
  readCurveData("curve.txt", mkt);
  //- to display data ----------------------
  // mkt.getCurve("USD-SOFR").display();
  //- to get interest rate data --------------------
  // double rates = mkt.getCurve("USD-SOFR").getRate(Date(570,0,0));
  // cout << rates << endl;

  // 2) volatility data:
  readVolData("vol.txt", mkt);
  // - to display data ---------------------
  // mkt.getVolCurve("defaultVolCurve").display();
  // - to get volatility data --------------
  // double vol = mkt.getVolCurve("defaultVolCurve").getVol(Date(209,0,0));
  // cout << vol << endl;

  // 3) Bondprice data;
  readBondPriceData("bondPrice.txt", mkt);
  // - to display bond price -------------------
  // auto bondprices = mkt.getBondPrice();
  // for (const auto& bondprice : bondprices){
  //   cout << bondprice.first << ": " << bondprice.second << endl;
  // }
  // // - to get a specific bondprice ----------
  // cout << bondprices["USD-GOV"] << endl;

  //4) Stockprice data:
  readStockPriceData("stockPrice.txt", mkt);
  // - to display stock price ------------------
  // auto stockprices = mkt.getStockPrice();
  // for (const auto& stockprice : stockprices){
  //   cout << stockprice.first << ": " << stockprice.second << endl;
  // }
  // // - to get a specific stock price -----------
  // cout << stockprices["APPL"] << endl;

  //5) Use Print() function
  mkt.Print();


  //task 2, create a portfolio of bond, swap, european option, american option
  //for each time, at least should have long / short, different tenor or expiry, different underlying
  //totally no less than 16 trades
  vector<Trade*> myPortfolio;

  //1) add bonds
  myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5, "SGD-GOV"));
  myPortfolio.push_back(new Bond(Date(2025, 1, 1), Date(2035, 1, 1), 20000000, 105.0, "USD-GOV"));
  myPortfolio.push_back(new Bond(Date(2024, 6, 1), Date(2034, 6, 1), 15000000, 104.0, "SGD-MAS-BILL"));
  myPortfolio.push_back(new Bond(Date(2023, 1, 1), Date(2033, 1, 1), 12000000, 102.0, "SGD-GOV"));
  
  //2) add swaps
  myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2029, 1, 1), 15000000, 0.070, 1));
  myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2031, 1, 1), 20000000, 0.060, 2));
  myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2030, 1, 1), 25000000, 0.065, 1));
  myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2032, 1, 1), 10000000, 0.055, 2));

  //3) add european option
  myPortfolio.push_back(new EuropeanOption(Call, 650, Date(2024, 12, 31), "APPL"));
  myPortfolio.push_back(new EuropeanOption(Put, 5000, Date(2025, 12, 31), "SP500"));
  myPortfolio.push_back(new EuropeanOption(Call, 3000, Date(2026, 6, 30), "STI"));
  myPortfolio.push_back(new EuropeanOption(Put, 680, Date(2027, 6, 30), "APPL"));

  //4) add american option
  myPortfolio.push_back(new AmericanOption(Call, 650, Date(2024, 12, 31), "APPL"));
  myPortfolio.push_back(new AmericanOption(Put, 5000, Date(2025, 12, 31), "SP500"));
  myPortfolio.push_back(new AmericanOption(Call, 3000, Date(2026, 6, 30), "STI"));
  myPortfolio.push_back(new AmericanOption(Put, 680, Date(2027, 6, 30), "APPL"));


  //task 3, creat a pricer and price the portfolio, output the pricing result of each deal.
  double totalPV = 0;
  int tradeID = 1;
  Pricer* treePricer = new CRRBinomialTreePricer(10);
  resultFile << "Task 3 - Present Value of Portfolio" << endl;
  resultFile << "----------------------------------------------------------------------" << "\n";

  for (auto trade: myPortfolio) {
    double pv = treePricer->Price(mkt, trade);
    cout << fixed << setprecision(6) << pv << endl;
    if (trade->getType() == "BondTrade"){
      Bond* bondptr = dynamic_cast<Bond*>(trade);
      resultFile << "Trade number: " << tradeID << " " << "type of asset: " << " " << "Bond" << "\n";
      resultFile << "Issuer: " << bondptr->getUnderlying() << "\n";
      Date start = bondptr->getStarting();
      Date matur = bondptr->getMaturity();
      resultFile << "Start Date of Bond: " << start.day << "/" << start.month << "/" << start.year << "\n";
      resultFile << "Maturity Date of Bond: " << matur.day << "/" << matur.month << "/" << matur.year << "\n";
      resultFile << "Notional: " << std::setprecision(8) << bondptr->getNotional() << "\n";
      resultFile << "Traded price: " << bondptr->getTradeprice() << "\n";
      resultFile << "Present Value: " << pv << "\n" << "\n";
      tradeID += 1;
      totalPV += pv;
    } 
    else if (trade->getType() == "SwapTrade"){
      Swap* swapptr = dynamic_cast<Swap*>(trade);
      resultFile << "Trade number: " << tradeID << " " << "type of asset: " << " " << "Swap" << "\n";
      Date start_swap = swapptr->getMaturity();
      Date matur_swap = swapptr->getMaturity();
      resultFile << "Start Date: " << start_swap.day << "/" << start_swap.month << "/" << start_swap.year << "\n";
      resultFile << "Maturity Date: " << matur_swap.day << "/" << matur_swap.month << "/" << matur_swap.year << "\n";
      resultFile << "Notional: " << swapptr->getNotional() << "\n";
      resultFile << "Traded Rate: " << swapptr->getTraderate() << "\n";
      resultFile << "Payment Frequency per year: " << swapptr->getFrequency() << "\n";
      resultFile << "Current PV: " << pv << "\n" << "\n";
      tradeID += 1;
      totalPV += pv;
    }
    else if (trade->getType() == "TreeProduct"){
      EuropeanOption* eur = dynamic_cast<EuropeanOption*>(trade);
      if (eur){
        resultFile << "Trade number: " << tradeID << " " << "type of asset: " << " " << "European Option" << "\n";
        if (eur->getoptiontype() == 0){
          resultFile << "Option Type: Call" << "\n";
        }
        else {
          resultFile << "Option Type: Put" << "\n";
        }
        resultFile << "Expiry Date: " << eur->GetExpiry().day << "/" << eur->GetExpiry().month << "/" << eur->GetExpiry().year << "\n";
        resultFile << "Strike Price: " << eur->getStike() << "\n";
        resultFile << "Underlying: " << eur->getUnderlying() << "\n";
        resultFile << "Current PV: " << pv << "\n" << "\n";
        tradeID += 1;
        totalPV += pv;
      }
      else{
        AmericanOption* amr = dynamic_cast<AmericanOption*>(trade);
        resultFile << "Trade number: " << tradeID << " " << "type of asset: " << " " << "American Option" << "\n";
        if (amr->getoptiontype() == 0){
          resultFile << "Option Type: Call" << "\n";
        }
        else {
          resultFile << "Option Type: Put" << "\n";
        }
        resultFile << "Expiry Date: " << amr->GetExpiry().day << "/" << amr->GetExpiry().month << "/" << amr->GetExpiry().year << "\n";
        resultFile << "Strike Price: " << amr->getStike() << "\n";
        resultFile << "Underlying: " << amr->getUnderlying() << "\n";
        resultFile << "Current PV: " << pv << "\n" << "\n";
        tradeID += 1;
        totalPV += pv;
      }  
    } 
  }
  resultFile << "Current PV of the portfolio: " << totalPV << "\n" << "\n";
  
  // task 4, analyzing pricing result
  resultFile << "Task 4 - Analyzing result" << "\n";
  resultFile << "----------------------------------------------------------------------" << "\n";
  vector<Trade*> taskfour;
  taskfour.push_back(new EuropeanOption(Put, 5000, Date(2025, 12, 31), "SP500"));
  taskfour.push_back(new AmericanOption(Put, 5000, Date(2025, 12, 31), "SP500"));
  // a) compare CRR binomial tree result for an european option vs Black model
  Trade* trade_1 = taskfour[0];
  // cout << trade->getType() << endl;
  double no_steps = 700;
  Pricer* treePricer2 = new CRRBinomialTreePricer(no_steps);
  double price_tree = treePricer2->Price(mkt, trade_1);
  EuropeanOption* eur_2 = dynamic_cast<EuropeanOption*>(trade_1);
  string ticker_1 = eur_2->getUnderlying();
  double spot = mkt.getStockPrice()[ticker_1];
  double T_1 = eur_2->GetExpiry() - mkt.asOf;
  // determine risk free rate:
  double year_diff_2 = eur_2->GetExpiry().year - mkt.asOf.year;
  double month_diff_2 = eur_2->GetExpiry().month - mkt.asOf.month;
  double day_diff_2 = eur_2->GetExpiry().day - mkt.asOf.day;
  Date tenor_lookup = Date((year_diff_2 * 360) + (month_diff_2 * 30) + day_diff_2, 0 ,0);
  double rate_2 = mkt.getCurve("USD-SOFR").getRate(tenor_lookup);
  double vol_2 = mkt.getVolCurve("defaultVolCurve").getVol(tenor_lookup);
  double price_black = BlackScholesPrice(spot, eur_2->getStike(), T_1, rate_2, vol_2, eur_2->getoptiontype());
  // cout << price_tree << endl;
  // cout << price_black << endl;
  resultFile << "A: comparing price from Binomial tree with Black model" << "\n";
  resultFile << "Parameters of option: " << "\n";
  if (eur_2->getoptiontype() == 0){
          resultFile << "Option Type: Call" << "\n";
        }
        else {
          resultFile << "Option Type: Put" << "\n";
        }
  resultFile << "Expiry Date: " << eur_2->GetExpiry().day << "/" << eur_2->GetExpiry().month << "/" << eur_2->GetExpiry().year << "\n";
  resultFile << "Strike Price: " << eur_2->getStike() << "\n";
  resultFile << "Underlying: " << eur_2->getUnderlying() << "\n" << "\n";
  resultFile << "Number of steps for Binomial tree: " << no_steps << "\n";
  resultFile << "Current PV calculated by Binonial tree: " << price_tree << "\n";
  resultFile << "Current PV calculated by Black model: " << price_black << "\n"; 
  // b) compare CRR binomial tree result for an american option vs european option

  resultFile << "B: comparing price from Binomial tree for an American and an European Option" << "\n";
  Trade* trade_2 = taskfour[1];
  double price_tree_amr = treePricer2->Price(mkt, trade_2);
  resultFile << "Current PV for an European Option: " << price_tree << "\n";
  resultFile << "Current PV for an American Option: " << price_tree_amr << "\n"; 
  resultFile << "------------------------ End of output File ------------------------" << "\n";
  
  //final
  // cout << "Project build successfully!" << endl;
  return 0;
}
