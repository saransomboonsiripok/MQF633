#include <fstream>
#include <ctime>
#include <chrono>

#include "BlackScholes.h"
#include "Market.h"
#include "Pricer.h"
#include "EuropeanTrade.h"
#include "Bond.h"
#include "Swap.h"
#include "AmericanTrade.h"

using namespace std;

void readFromFile(const string& fileName, string& outPut){
  string lineText;
  ifstream MyReadFile(fileName);
  while (getline (MyReadFile, lineText)) {
    outPut.append(lineText);
  }
  MyReadFile.close();
}

// Declaration of the Black-Scholes price function
double BlackScholesPrice(double S, double K, double T, double r, double sigma, OptionType optType);

int main() {
  // Task 1: Create a Market object and update it from a text file

  // Get the current date
  std::time_t currentTime = std::time(0);
  auto currentLocalTime = std::localtime(&currentTime);
  Date valueDate;
  valueDate.year = currentLocalTime->tm_year + 1900;
  valueDate.month = currentLocalTime->tm_mon + 1;
  valueDate.day = currentLocalTime->tm_mday;

  // Create a Market object
  Market market = Market(valueDate);

  // Load data from the file and update the Market object

  // Task 2: Create a portfolio of trades

  vector<Trade*> myPortfolio;

  // Create a Bond trade
  Trade* bond = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5);
  myPortfolio.push_back(bond);

  // Task 3: Create a pricer and calculate the prices of trades in the portfolio

  // Create a pricer using the CRR binomial tree
  Pricer* binomialTreePricer = new CRRBinomialTreePricer(10);

  // Calculate the prices of trades in the portfolio
  for (auto trade : myPortfolio) {
    double price = binomialTreePricer->Price(market, trade);
    // Save price details to a file
  }

  // Task 4: Analyze the pricing results

  // Define the parameters of the European option
  OptionType optType = Call;
  double strike = 100.0;
  Date expiryDate(2025, 1, 1);
  double spotPrice = 100.0;
  double volatility = 0.2;
  double riskFreeRate = 0.05;
  double T = (expiryDate - valueDate) / 365.0;

  // Initialize the European option
  EuropeanOption euroOption(optType, strike, expiryDate);

  // Create a pricer using the CRR binomial tree
  CRRBinomialTreePricer treePricer(100);

  // Calculate the price of the European option using the CRR binomial tree model
  double priceTree = treePricer.PriceTree(market, euroOption);

  // Calculate the price of the European option using the Black-Scholes model
  double priceBlackScholes = BlackScholesPrice(spotPrice, strike, T, riskFreeRate, volatility, optType);

  // Display the results
  cout << "Price of the European option with CRR Binomial Tree: " << priceTree << endl;
  cout << "Price of the European option with Black-Scholes: " << priceBlackScholes << endl;

  // Task 4b: Compare with an American option

  AmericanOption americanOption(optType, strike, expiryDate);
  double priceAmericanOption = treePricer.PriceTree(market, americanOption);

  cout << "Price of the American option with CRR Binomial Tree: " << priceAmericanOption << endl;

  // Final message
  cout << "Project built successfully!" << endl;
  return 0;
}
