#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>

using namespace std;

class Trade
{
protected:
	float open;
	float high;
	float low;
	float close;
	int volume;
public:
	Trade(float open_, float high_, float low_, float close_, int volume_) :
		open(open_), high(high_), low(low_), close(close_), volume(volume_) {}
	~Trade() {}
	float GetOpen() const { return open; }
	float GetHigh() const { return high; }
	float GetLow() const { return low; }
	float GetClose() const { return close; }
	int GetVolume() const { return volume; }
};

class DailyTrade : public Trade
{
private:
	string date;
	float adjusted_close;
public:
	DailyTrade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_) :
		Trade(open_, high_, low_, close_, volume_), date(date_), adjusted_close(adjusted_close_) {}
	~DailyTrade() {}
	string GetDate() const { return date; }
	float GetAdjustedClose() const { return close; }
	friend ostream& operator << (ostream& out, const DailyTrade& t)
	{
		out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low
			<< " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
		return out;
	}
};


class Stock
{
private:
	string symbol;
	vector<DailyTrade> dailyTrades;
public:
	Stock() :symbol("") { dailyTrades.clear(); }
	Stock(string symbol_) :symbol(symbol_) { dailyTrades.clear(); }
	Stock(const Stock& stock) { memcpy(this, &stock, sizeof(stock)); }
	~Stock() {}
	void addDailyTrade(const DailyTrade& aTrade) { dailyTrades.push_back(aTrade); }
	string GetSymbol() const { return symbol; }
	const vector<DailyTrade>& GetDailyTrade(void) const { return dailyTrades; }
	friend ostream& operator << (ostream& out, const Stock& s)
	{
		out << "Symbol: " << s.symbol << endl;
		for (vector<DailyTrade>::const_iterator itr = s.dailyTrades.begin(); itr != s.dailyTrades.end(); itr++)
			out << *itr;
		return out;
	}
};
