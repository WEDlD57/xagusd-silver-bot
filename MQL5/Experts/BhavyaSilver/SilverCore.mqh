#ifndef BHAVYA_SILVER_CORE_MQH
#define BHAVYA_SILVER_CORE_MQH

// Platform-independent decisions. The validation harness runs this same code.
double BSFloorPrice(const double price,const double tick,const int digits)
{
   if(tick<=0.0 || price<=0.0) return 0.0;
   return NormalizeDouble(MathFloor(price/tick+1e-9)*tick,digits);
}

double BSCeilPrice(const double price,const double tick,const int digits)
{
   if(tick<=0.0 || price<=0.0) return 0.0;
   return NormalizeDouble(MathCeil(price/tick-1e-9)*tick,digits);
}

double BSFloorLots(const double desired,const double minimum,
                   const double maximum,const double step)
{
   if(step<=0.0 || minimum<=0.0 || maximum<minimum || desired<minimum)
      return 0.0; // Never round a too-small risk budget up to the minimum lot.
   double capped=MathMin(desired,maximum);
   // The broker requires a multiple of STEP, not an offset from MINIMUM.
   double lots=MathFloor(capped/step+1e-9)*step;
   lots=NormalizeDouble(lots,8);
   if(lots>capped+1e-10 || lots<minimum-1e-10) return 0.0;
   return lots;
}

double BSStopRiskMoney(const double stop_profit,const double volume,
                       const double commission_per_lot)
{
   // A tightened stop above break-even is not a loss of abs(profit).
   return MathMax(-stop_profit,0.0)+volume*commission_per_lot;
}

bool BSInSession(const int hour,const int start,const int end)
{
   if(hour<0 || hour>23 || start<0 || start>23 || end<0 || end>23)
      return false;
   if(start==end) return true; // Explicit 24-hour configuration.
   if(start<end) return hour>=start && hour<end;
   return hour>=start || hour<end;
}

double BSRiskFloor(const double day_start,const double day_peak,
                   const double campaign_peak,const double daily_loss_pct,
                   const double daily_trail_pct,const double total_trail_pct)
{
   return MathMax(day_start*(1.0-daily_loss_pct/100.0),
          MathMax(day_peak*(1.0-daily_trail_pct/100.0),
                  campaign_peak*(1.0-total_trail_pct/100.0)));
}

double BSRiskBudget(const double equity,const double risk_pct,
                    const double risk_floor)
{
   if(equity<=risk_floor || equity<=0.0 || risk_pct<=0.0) return 0.0;
   return MathMin(equity*risk_pct/100.0,(equity-risk_floor)*0.90);
}

int BSDetectBreakout(const int trend,const double open,const double high,
                     const double low,const double close,const double box_high,
                     const double box_low,const double atr,const double min_width,
                     const double max_width,const double buffer,const double extension,
                     const double min_body,const double max_range)
{
   if(atr<=0.0 || box_high<=box_low || high<low) return 0;
   double width=(box_high-box_low)/atr;
   if(width<min_width || width>max_width ||
      MathAbs(close-open)<min_body*atr || high-low>max_range*atr) return 0;
   if(trend==1 && close>open && close>box_high+buffer*atr &&
      close<=box_high+extension*atr) return 1;
   if(trend==-1 && close<open && close<box_low-buffer*atr &&
      close>=box_low-extension*atr) return -1;
   return 0;
}

bool BSInvalidRetest(const int direction,const double close,const double level,
                     const double atr,const double penetration)
{
   if(atr<=0.0) return true;
   if(direction==1) return close<level-penetration*atr;
   if(direction==-1) return close>level+penetration*atr;
   return true;
}

bool BSIsRetest(const int direction,const double open,const double high,
                const double low,const double close,const double level,
                const double atr,const double touch,const double penetration,
                const double close_buffer,const double max_range)
{
   if(atr<=0.0 || high<low || high-low>max_range*atr) return false;
   if(direction==1)
      return low<=level+touch*atr && low>=level-penetration*atr &&
             close>level+close_buffer*atr && close>open;
   if(direction==-1)
      return high>=level-touch*atr && high<=level+penetration*atr &&
             close<level-close_buffer*atr && close<open;
   return false;
}

bool BSEntryNotChasing(const int direction,const double entry,const double close,
                       const double level,const double atr,const double max_move,
                       const double max_extension)
{
   if(atr<=0.0) return false;
   if(direction==1)
      return entry>level && entry-close<=max_move*atr &&
             entry-level<=max_extension*atr;
   if(direction==-1)
      return entry<level && close-entry<=max_move*atr &&
             level-entry<=max_extension*atr;
   return false;
}
#endif
