#include <cmath>
#include <iostream>
#include "Pricer.h"
#include "Bond.h"
#include "Swap.h"


double Pricer::Price(const Market& mkt, Trade* trade) {
  double pv;
  if (trade->getType() == "TreeProduct") {
    TreeProduct* treePtr = dynamic_cast<TreeProduct*>(trade);
    if (treePtr) { //check if cast is sucessful
      pv = PriceTree(mkt, *treePtr);
    }
  }
  else if (trade->getType() == "BondTrade"){
    Bond* bonds = dynamic_cast<Bond*>(trade);
    string underly = bonds->getUnderlying();
    double price = mkt.bondPrices.at(underly); //get from market data
    pv = trade->Payoff(price);
  }
  else{
    Swap* swaps = dynamic_cast<Swap*>(trade);
    pv = swaps->PayoffSwap(mkt);
    // implement swap pricer
  }
  return pv;
}

double BinomialTreePricer::PriceTree(const Market& mkt, const TreeProduct& trade) {
    // double T = trade.GetExpiry() - mkt.asOf;
    double T = static_cast<double>(trade.GetExpiry() - mkt.asOf);
    if (T <= 0) {
        std::cerr << "Invalid expiry date: T is non-positive." << std::endl;
        return nan(""); 
    }
    double dt = T / nTimeSteps;
    double stockPrice = 100.0; // Example value, replace with actual market data
    double vol = 0.2;          // Example value, replace with actual market data
    double rate = 0.05;        // Example value, replace with actual market data

    ModelSetup(stockPrice, vol, rate, dt);

    // Initialize states at maturity
    for (int i = 0; i <= nTimeSteps; i++) {
        states[i] = trade.Payoff(GetSpot(nTimeSteps, i));
    }

    // Backward induction
    for (int k = nTimeSteps - 1; k >= 0; k--) {
        for (int i = 0; i <= k; i++) {
            double df = exp(-rate * dt);
            double continuation = df * (states[i] * GetProbUp() + states[i + 1] * GetProbDown());
            states[i] = trade.ValueAtNode(GetSpot(k, i), dt * k, continuation);
        }
    }

    return states[0];
}

// double BinomialTreePricer::PriceTree(const Market& mkt, const TreeProduct& trade) {
//   // model setup
//   double T = trade.GetExpiry() - mkt.asOf;
//   double dt = T / nTimeSteps;
//   double stockPrice, vol, rate;
//   /*
//   get these data for the deal from market object
//   */
//   ModelSetup(stockPrice, vol, rate, dt);
  
//   // initialize
//   for (int i = 0; i <= nTimeSteps; i++) {
//     states[i] = trade.Payoff( GetSpot(nTimeSteps, i) );
//   }    
  
//   // price by backward induction
//   for (int k = nTimeSteps-1; k >= 0; k--)
//     for (int i = 0; i <= k; i++) {
//     // calculate continuation value
//       double df = exp(-rate *dt);	  
//       double continuation = df * (states[i]*GetProbUp() + states[i+1]*GetProbDown());
//       // calculate the option value at node(k, i)
//       states[i] = trade.ValueAtNode( GetSpot(k, i), dt*k, continuation);
//     }

//   return states[0];

// }

void CRRBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
  double b = std::exp((2*rate+sigma*sigma)*dt)+1;
  u = (b + std::sqrt(b*b - 4*std::exp(2*rate*dt))) / 2 / std::exp(rate*dt);
  p = (std::exp(rate*dt) -  1/u) / (u - 1/u);
  currentSpot = S0;

  std::cout << "CRR Model Setup - u: " << u << ", p: " << p << ", currentSpot: " << currentSpot << std::endl;
}

void JRRNBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
    u = std::exp( (rate - sigma*sigma/2) * dt + sigma * std::sqrt(dt) );
    d = std::exp( (rate - sigma*sigma/2) * dt - sigma * std::sqrt(dt) );
    p = (std::exp(rate*dt) -  d) / (u - d);
    currentSpot = S0;

    std::cout << "JRRN Model Setup - u: " << u << ", d: " << d << ", p: " << p << ", currentSpot: " << currentSpot << std::endl;
}
