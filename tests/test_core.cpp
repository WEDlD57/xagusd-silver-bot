#include <cmath>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

double MathFloor(double v) { return std::floor(v); }
double MathCeil(double v) { return std::ceil(v); }
double MathMin(double a,double b) { return std::min(a,b); }
double MathMax(double a,double b) { return std::max(a,b); }
double MathAbs(double v) { return std::abs(v); }
double NormalizeDouble(double v,int digits) {
    double scale=std::pow(10.0,digits); return std::round(v*scale)/scale;
}
// These are the production .mqh decisions, not a Python copy of their logic.
#include "../MQL5/Experts/BhavyaSilver/SilverCore.mqh"

int main() {
    assert(BSFloorLots(0.009,0.01,100.0,0.01)==0.0);
    assert(std::abs(BSFloorLots(0.185,0.01,100.0,0.01)-0.18)<1e-10);
    assert(BSFloorLots(0.05,0.10,100.0,0.10)==0.0);
    assert(std::abs(BSFloorLots(2.0,0.01,0.25,0.01)-0.25)<1e-10);
    assert(BSFloorLots(0.10,0.01,100.0,0.0)==0.0);
    assert(std::abs(BSFloorLots(0.075,0.03,100.0,0.02)-0.06)<1e-10);
    assert(BSFloorLots(0.035,0.03,100.0,0.02)==0.0);
    assert(BSStopRiskMoney(-60.0,.1,8.0)==60.8);
    assert(BSStopRiskMoney(100.0,.1,8.0)==.8);
    assert(std::abs(BSRiskBudget(25000,0.25,24750)-62.5)<1e-10);
    assert(std::abs(BSRiskBudget(24760,0.25,24750)-9.0)<1e-10);
    assert(BSRiskBudget(24750,0.25,24750)==0.0);
    assert(BSRiskFloor(25000,25100,26000,1,1,5)==24849);
    assert(BSInSession(7,7,20) && BSInSession(19,7,20) && !BSInSession(20,7,20));
    assert(BSInSession(23,22,6) && BSInSession(2,22,6) && !BSInSession(12,22,6));
    assert(BSInSession(12,0,0) && !BSInSession(25,0,0));

    // Box 99..101, ATR 1: a properly bounded, directional breakout can arm.
    assert(BSDetectBreakout(1,100.9,101.5,100.8,101.3,101,99,1,1,4,.1,.8,.15,2.5)==1);
    assert(BSDetectBreakout(0,100.9,101.5,100.8,101.3,101,99,1,1,4,.1,.8,.15,2.5)==0);
    assert(BSDetectBreakout(1,100.9,102.2,100.8,102.0,101,99,1,1,4,.1,.8,.15,2.5)==0);
    assert(BSDetectBreakout(1,100.9,101.5,98.0,101.3,101,99,1,1,4,.1,.8,.15,2.5)==0);
    assert(BSDetectBreakout(-1,99.1,99.2,98.5,98.7,101,99,1,1,4,.1,.8,.15,2.5)==-1);
    assert(BSIsRetest(1,101.05,101.4,100.95,101.2,101,1,.25,.5,.05,2.5));
    assert(BSIsRetest(-1,98.95,99.05,98.6,98.8,99,1,.25,.5,.05,2.5));
    assert(!BSIsRetest(1,101.05,101.4,100.4,101.2,101,1,.25,.5,.05,2.5));
    assert(BSInvalidRetest(1,100.4,101,1,.5));
    assert(BSInvalidRetest(-1,99.6,99,1,.5));
    assert(BSEntryNotChasing(1,101.3,101.2,101,1,.35,.9));
    assert(!BSEntryNotChasing(1,101.7,101.2,101,1,.35,.9));
    assert(BSEntryNotChasing(-1,98.7,98.8,99,1,.35,.9));
    assert(!BSEntryNotChasing(-1,98.2,98.8,99,1,.35,.9));
    assert(!BSEntryNotChasing(1,100.9,101.2,101,1,.35,.9));

    // Example silver CFD math, NOT assumed broker specifications or a backtest.
    // 5,000 oz/lot, bid 30.000 / ask 30.030, stop 29.600.
    // Reserve 0.020 on entry and 0.020 on the stop exit, plus 8 currency/lot.
    double loss_per_lot=((30.030+.020)-(29.600-.020))*5000.0+8.0;
    double silver_lots=BSFloorLots(62.5/loss_per_lot,.01,100,.01);
    assert(std::abs(silver_lots-.02)<1e-10);
    assert(silver_lots*loss_per_lot<=62.5);
    assert(BSFloorLots(62.5/(3.0*5000.0+8.0),.01,100,.01)==0.0);
    // Bid 30.000 sell, stop 30.400; same reserve on both sides.
    double sell_loss=((30.400+.020)-(30.000-.020))*5000.0+8.0;
    assert(std::abs(BSFloorLots(62.5/sell_loss,.01,100,.01)-.02)<1e-10);
    assert(std::abs(BSFloorPrice(30.017,.005,3)-30.015)<1e-10);
    assert(std::abs(BSCeilPrice(30.017,.005,3)-30.020)<1e-10);
    assert(std::abs(BSFloorLots(.79,.20,100,.25)-.75)<1e-10);

    std::mt19937_64 random(26100602);
    std::uniform_real_distribution<double> uniform(0.0,1.0);
    for(int i=0;i<100000;i++) {
        double budget=1.0+uniform(random)*10000;
        double loss_per_lot=1.0+uniform(random)*50000;
        double step=std::pow(10.0,-(1+i%3)), minimum=step;
        double desired=budget/loss_per_lot;
        double lots=BSFloorLots(desired,minimum,100.0,step);
        assert(lots==0.0 || lots>=minimum-1e-10);
        assert(lots<=desired+1e-9 && lots<=100.0+1e-9);
        assert(lots*loss_per_lot<=budget+1e-5);
        if(lots>0) assert(std::abs(lots/step-std::round(lots/step))<1e-6);
        double price=10.0+uniform(random)*90.0;
        double tick=(i%2==0 ? 0.001 : 0.005);
        double floor=BSFloorPrice(price,tick,3),ceil=BSCeilPrice(price,tick,3);
        assert(floor<=price+1e-7 && ceil>=price-1e-7);
        assert(ceil-floor<=tick+1e-7);
        double equity=1000.0+uniform(random)*100000;
        double risk_floor=equity-(uniform(random)*1000);
        double target=BSRiskBudget(equity,.25,risk_floor);
        assert(target<=equity*.0025+1e-8);
        assert(target<=.90*(equity-risk_floor)+1e-8);
    }
    // Irregular min/max/step combinations must still be multiples of STEP.
    for(int i=0;i<10000;i++) {
        double step=(i%3==0 ? .005 : (i%3==1 ? .02 : .25));
        double minimum=step*(.5+uniform(random)*4.0);
        double maximum=minimum+uniform(random)*20.0;
        double desired=uniform(random)*(maximum+5.0);
        double lots=BSFloorLots(desired,minimum,maximum,step);
        if(lots>0) {
            assert(lots>=minimum-1e-8 && lots<=maximum+1e-8);
            assert(lots<=desired+1e-8);
            assert(std::abs(lots/step-std::round(lots/step))<1e-6);
        }
    }
    std::cout << "Core: silver risk/signal cases and 110,000 randomized invariant cases passed.\n";
}
