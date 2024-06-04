#include "Market.h"

using namespace std;

void RateCurve::display() const {
    cout << "rate curve:" << name << endl;
    for (size_t i=0; i<tenors.size(); i++) {
      cout << tenors[i] << ":" << rates[i] << endl;
  }
  cout << endl;
}

void VolCurve::display() const {
  cout << "Volatility Curve: " << name << endl;
  for (size_t i = 0; i < tenors.size(); ++i){
    cout << tenors[i] << " : " << vols[i] << endl;
  }
}

void RateCurve::addRate(Date tenor, double rate) {
  //consider to check if tenor already exist
  if (true){
    tenors.push_back(tenor);
    rates.push_back(rate);
  }  
}

void VolCurve::addVol(Date tenor, double vol) {
    tenors.push_back(tenor);
    vols.push_back(vol);
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

double VolCurve::getVol(Date tenor) const {
  //use linear interpolation to get rate
  if (tenors.empty()) return 0.0;
  if (tenor <= tenors.front()) return vols.front();
  if (tenor >= tenors.back()) return vols.back();

  for (size_t i = 1; i < tenors.size(); ++i){
    if (tenor < tenors[i]){
      double t1 = tenors[i-1] - tenors[0];
      double t2 = tenors[i] - tenors[0];
      double r1 = vols[i-1];
      double r2 = vols[i];
      double t = tenor - tenors[0];
      return r1 + (r2 - r1) * (t - t1) / (t2 - t1);
    }
  }
  return 0.0;
}



void Market::Print() const
{
  cout << "market asof: " << asOf << endl;
  cout << "Rate curve: " << endl;
  for (auto curve: curves) {
    curve.second.display();
  }
  for (auto vol: vols) {
    vol.second.display();
  }
  cout << "\n" << "Bond prices: " << endl;
  for (auto bondprice: bondPrices){
    cout << bondprice.first << ":" << bondprice.second << endl;
  }
  cout << "\n" << "Stock prices: " <<endl;
  for (auto stockprice: stockPrices){
    cout << stockprice.first << ":" << stockprice.second << endl;
  }
}

void Market::addCurve(const std::string& curveName, const RateCurve& curve){
  curves.emplace(curveName, curve);
}

void Market::addVolCurve(const std::string& curveName, const VolCurve& curve) {
    vols[curveName] = curve;
}

void Market::addBondPrice(const std::string& bondName, double price){
  bondPrices[bondName] = price;
}

void Market::addStockPrice(const std::string& stockName, double price) {
    stockPrices[stockName] = price;
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
