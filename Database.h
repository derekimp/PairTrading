#pragma once
#include <sqlite3.h>
#include <string>
class Stock;
int CreatePairTable(sqlite3*& db);
int PopulatePairTable(sqlite3*& db);
int InsertTable(sqlite3* db, const Stock& stock, std::string TableName);
int CreatePairPricesTable(sqlite3*& db);
int CalVol(sqlite3*& db);
void sqlite_power(sqlite3_context* context, int argc, sqlite3_value** argv);
int CalDailyPnL(sqlite3*& db, double k);
int CalSumPnL(sqlite3*& db);
void ManualTest(sqlite3*& db);
void DropAllTable(sqlite3*& db);