#pragma once
#include <string>
#include <map>
#include <vector>
#include "Database.h"

class Stock;
int PullMarketData(const std::string& url_request, std::string& read_buffer);
int RetrieveData(std::vector<std::string> symbol1, std::vector<std::string> symbol2, std::map<std::string, std::string>& read_buffer1, std::map<std::string, std::string>& read_buffer2);
int PopulateDailyTrades(const std::string& read_buffer, Stock& stock);
int PopulateTable(sqlite3* db, std::vector<std::string> symbols, std::map<std::string, std::string>& read_buffer, std::string TableName);