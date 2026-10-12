# Start here — XAGUSD silver bot

This is an MT5 Expert Advisor written in MQL5. It runs on desktop MT5 or a VPS.
An iPhone can monitor your account, but the mobile MT5 app cannot run this EA.

1. Unzip `xagusd-silver-bot.zip` on your MT5 computer.
2. In MT5, choose **File → Open Data Folder**.
3. Open **MQL5 → Experts**. Copy in the complete **BhavyaSilver** folder from
   this download's `MQL5/Experts` folder. Keep both source files together.
4. Press **F4** to open MetaEditor. Open **BhavyaSilver_XAGUSD.mq5** and press
   **F7** to compile. You must see **0 errors**. The compiler creates the `.ex5`.
5. Return to MT5 and open **View → Strategy Tester**. Select the newly compiled EA.
6. Choose your broker's exact **XAGUSD** symbol and **M15**. Select
   **Every tick based on real ticks**, match deposit/currency/leverage to your
   intended account, and include the broker's actual fees and a realistic delay.
7. In **Inputs**, load `presets/research-defaults.set` if desired. The built-in
   defaults are identical. Adjust the commission estimate and server-time session
   to your broker; the preset is not optimized.
8. Run a baseline and an untouched later period. Read **README.md** for the
   evaluation procedure. Check equity drawdown, costs, and how many trades occur.
9. After those checks, attach it to an **XAGUSD M15 demo chart** and enable Algo
   Trading. The chart status explains why the EA is waiting or blocked.

Default behavior: H1/D1 trend confirmation, an eight-bar M15 box breakout,
then a later retest. One silver position at a time; no grid or martingale.

| Setting | Initial value |
|---|---:|
| Risk target per entry | 0.25% of equity |
| Maximum daily entries | 2 |
| Daily start/peak equity loss lock | 1% |
| Campaign peak equity loss lock | 5% |
| Stop distance | At least 1.8 M15 ATR |
| Take-profit distance | 2 times initial quoted stop distance |
| Entry session | 07:00–20:00 broker server time |
| Friday cutoff | 16:00 broker server time |
| Absolute spread cap | 0.040 in the silver quote |

**Performance is unverified.** Core risk/signal and evaluation tests passed here;
MetaEditor compilation and broker tick backtesting have not been run. The EA
defaults to refusing real accounts. A risk target or equity guard cannot guarantee
a maximum loss when prices gap or execution fails.

If it does not trade, check the chart status and Experts log. Typical causes are
insufficient H1/D1 history, no qualifying later retest, spread above either cap,
minimum lot exceeding the budget, a closed server-time session, an existing
position, a cooldown, or a tripped equity lock.
