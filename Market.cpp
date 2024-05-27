#include "Market.h"
#include <algorithm>
#include <cmath>

using namespace std;

void RateCurve::display() const {
    cout << "rate curve:" << name << endl;
    for (size_t i=0; i<tenors.size(); i++) {
      cout << tenors[i].toString() << " : " << rates[i] << endl;
  }
  cout << endl; 
}

void RateCurve::addRate(Date tenor, double rate) {
  //consider to check if tenor already exist
  auto it = std::find(tenors.begin(), tenors.end(), tenor);
  if (it == tenors.end()){
    tenors.push_back(tenor);
    rates.push_back(rate);
  }else{
    cout << "Tenor already exists: " << tenor << endl;
  }  
}

double RateCurve::getRate(Date tenor) const {
  //use linear interpolation to get rate
  if (tenors.empty()) return 0.0;
  if (tenor <= tenors.front()) return rates.front();
  if (tenor >= tenors.back()) return rates.back();

  for (size_t i = 1; i < tenors.size(); ++i){
    if (tenor < tenors[i]){
      double t1 = tenors[i-1] - tenors[0];
      double t2 = tenors[i] - tenors[0];
      double r1 = rates[i-1];
      double r2 = rates[i];
      double t = tenor - tenors[0];
      return r1 + (r2 - r1) * (t - t1) / (t2 - t1);
    }
  }
  return 0.0;
}


void VolCurve::display() const {
  cout << "Volatility Curve: " << name << endl;
  for (size_t i = 0; i < tenors.size(); ++i){
    cout << tenors[i].toString() << " : " << vols[i] << endl;
  }
}

void VolCurve::addVol(Date tenor, double vol) {
    tenors.push_back(tenor);
    vols.push_back(vol);
}

void Market::addCurve(const std::string& curveName, const RateCurve& curve){
  curves.emplace(curveName, curve);
}

void Market::addVolCurve(const std::string& curveName, const VolCurve& curve) {
    vols[curveName] = curve;
}

void Market::addBondPrice(const std::string& bondName, double price) {
    bondPrices[bondName] = price;
}

void Market::addStockPrice(const std::string& stockName, double price) {
    stockPrices[stockName] = price;
}

double Market::getDiscountFactor(const std::string& curveName, double time) const {
    double rate = getCurve(curveName).getRate(Date(0, static_cast<int>(time * 12), 0));
    return exp(-rate * time);
}

void Market::Print() const
{
  cout << "Market Data as of: " << asOf << endl;
  cout << "Rate Curves:" << endl;
  for (const auto& curve : curves) {
      cout << curve.first << ": ";
      curve.second.display();
  }
  cout << "Volatility Curves:" << endl;
  for (const auto& volCurve : vols) {
      cout << volCurve.first << ": ";
      volCurve.second.display();
  }
cout << " " << endl;

  cout << "Bond Prices:" << endl;
  for (const auto& bondPrice : bondPrices) {
      cout << bondPrice.first << ": " << bondPrice.second << endl;
  }
cout << " " << endl;

  cout << "Stock Prices:" << endl;
  for (const auto& stockPrice : stockPrices) {
      cout << stockPrice.first << ": " << stockPrice.second << endl;
  }
cout << " " << endl;
}

std::ostream& operator<<(std::ostream& os, const Market& mkt)
{
  os << mkt.asOf << std::endl;
  return os;
}

std::istream& operator>>(std::istream& is, Market& mkt)
{
  is >> mkt.asOf;
  return is;
}

// Black Scholes Pricer Addition (Added method implementations)
double Market::getStockPrice(const std::string& stockName) const {
    auto it = stockPrices.find(stockName);
    if (it != stockPrices.end()) {
        return it->second;
    }
    throw std::invalid_argument("Stock price not found for: " + stockName);
}

double Market::getInterestRate(const std::string& curveName, const Date& date) const {
    return getCurve(curveName).getRate(date);
}

double Market::getVolatility(const std::string& curveName, const Date& date) const {
    return getVolCurve(curveName).getVol(date);
}