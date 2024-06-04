#include <cmath>
#include "Pricer.h"
#include "Bond.h"
#include "Swap.h"
#include "Market.h"
#include <ctime>
#include <iomanip>


double Pricer::Price(const Market& mkt, Trade* trade) {
  double pv;
  if (trade->getType() == "TreeProduct") {
    TreeProduct* treePtr = dynamic_cast<TreeProduct*>(trade);
    if (treePtr) { //check if cast is sucessful
      pv = PriceTree(mkt, *treePtr);
    }
  }
  else{
    if (trade->getType() == "BondTrade"){
        Bond* bonds = dynamic_cast<Bond*>(trade);
        string underly = bonds->getUnderlying();
        double price = mkt.getBondPrice().at(underly); //get from market data
        pv = trade->Payoff(price);
    }
    else if (trade->getType() == "SwapTrade"){
        // dynamic cast swap
        Swap* swaps = dynamic_cast<Swap*>(trade);
        // specify current time
        std::time_t t = std::time(0);
        auto now_ = localtime(&t);
        Date current_date;
        current_date.year = now_->tm_year + 1900;
        current_date.month = now_->tm_mon + 1;
        current_date.day = now_->tm_mday;
        // find the tenor
        Date maturity = swaps->getMaturity();
        Date tenor;
        int year_dif = maturity.year - current_date.year;
        int month_dif = maturity.month - current_date.month;
        int day_dif = maturity.day - current_date.day;
        tenor.day = (year_dif * 360) + (month_dif * 30) + (day_dif);
        tenor.month = 0;
        tenor.year = 0;
        // cout << maturity.day << " " << maturity.month << " " << maturity.year << "\n";
        // cout << current_date.day << " " << current_date.month << " " << current_date.year << "\n";
        // cout << tenor.day << " " << tenor.month << " " << tenor.year << "\n";
        // cout << year_dif << endl;
        // cout << month_dif << endl;
        // cout << day_dif << endl;
        double market_rate = mkt.getCurve("USD-SOFR").getRate(Date(tenor.day, tenor.month, tenor.year));
        // cout << std::setprecision(8) << market_rate << endl;
        pv = trade->Payoffswap(market_rate, mkt);
    }
  }

  return pv;
}

double BinomialTreePricer::PriceTree(const Market& mkt, const TreeProduct& trade) {
  // model setup
  double T = trade.GetExpiry() - mkt.asOf;
  // cout << std::setprecision(8) << "T = " << T << endl; 
  double dt = T / nTimeSteps;
  // cout << std::setprecision(8) << "dt = " << dt << endl;
  double stockPrice, vol, rate;
  string ticker = trade.getUnderlying();
  stockPrice = mkt.getStockPrice()[ticker];
  //cout << "stockprice = " <<stockPrice << endl;
  //cout << mkt.asOf.day << " " << mkt.asOf.month << " " << mkt.asOf.year << endl;
  int year_diff = trade.GetExpiry().year - mkt.asOf.year;
  int month_diff = trade.GetExpiry().month - mkt.asOf.month;
  int day_diff = trade.GetExpiry().day - mkt.asOf.day;
  double period = (360 * year_diff) + (30 * month_diff) + day_diff;
  //cout << period << endl;
  vol = mkt.getVolCurve("defaultVolCurve").getVol(Date(period, 0, 0));
  // cout << "vol: "<< std::setprecision(8) << vol << endl;
  rate = mkt.getCurve("USD-SOFR").getRate(Date(period,0,0));
  // cout << "rate: " << std::setprecision(8) << rate << endl;


  ModelSetup(stockPrice, vol, rate, dt);
  
  // initialize
  for (int i = 0; i <= nTimeSteps; i++) {
    states[i] = trade.Payoff(GetSpot(nTimeSteps, i) );
    //cout << states[i] << endl;
  }    
  
  // price by backward induction
  for (int k = nTimeSteps-1; k >= 0; k--)
    for (int i = 0; i <= k; i++) {
    // calculate continuation value
      double df = exp(-rate *dt);	  
      double continuation = df * (states[i]*GetProbUp() + states[i+1]*GetProbDown());
      // calculate the option value at node(k, i)
      states[i] = trade.ValueAtNode( GetSpot(k, i), dt*k, continuation);
      //cout << states[i] << endl;
    }

  return states[0];

}

void CRRBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
  double b = std::exp((2*rate+sigma*sigma)*dt)+1;
  u = (b + std::sqrt(b*b - 4*std::exp(2*rate*dt))) / 2 / std::exp(rate*dt);
  p = (std::exp(rate*dt) -  1/u) / (u - 1/u);
  currentSpot = S0;
  //cout << std::setprecision(6) << "b = " << b << " u = " << u << " p = " << p << endl;
}

void JRRNBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
    u = std::exp( (rate - sigma*sigma/2) * dt + sigma * std::sqrt(dt) );
    d = std::exp( (rate - sigma*sigma/2) * dt - sigma * std::sqrt(dt) );
    p = (std::exp(rate*dt) -  d) / (u - d);
    currentSpot = S0;
}
