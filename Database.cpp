#pragma once
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "Database.h"
#include "Stock.h"

int CreatePairTable(sqlite3*& db) 
{
	int rc = 0;
	char* error = nullptr;

	// Create the table
	cout << "Creating StockPairs table ..." << endl;
	string sqlCreateTable = string("CREATE TABLE IF NOT EXISTS StockPairs ")
		+ "(id INT NOT NULL,"
		+ "symbol1 CHAR(20) NOT NULL,"
		+ "symbol2 CHAR(20) NOT NULL,"
		+ "volatility FLOAT NOT NULL,"
		+ "profit_loss FLOAT NOT NULL,"
		+ "PRIMARY KEY(symbol1, symbol2)"
		+ ");";

	rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Created StockPairs table." << endl << endl;
	}

	cout << "Creating PairOnePrices table ..." << endl;
	sqlCreateTable = string("CREATE TABLE IF NOT EXISTS PairOnePrices ")
		+ "(symbol CHAR(20) NOT NULL,"
		+ "date CHAR(20) NOT NULL,"
		+ "open REAL NOT NULL,"
		+ "high REAL NOT NULL,"
		+ "low REAL NOT NULL,"
		+ "close REAL NOT NULL,"
		+ "adjusted_close REAL NOT NULL,"
		+ "volume INT NOT NULL,"
		+ "PRIMARY KEY(symbol, date)"
		+ ");";


	rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Created PairOnePrices table." << endl << endl;
	}

	cout << "Creating PairTwoPrices table ..." << endl;
	sqlCreateTable = string("CREATE TABLE IF NOT EXISTS PairTwoPrices ")
		+ "(symbol CHAR(20) NOT NULL,"
		+ "date CHAR(20) NOT NULL,"
		+ "open REAL NOT NULL,"
		+ "high REAL NOT NULL,"
		+ "low REAL NOT NULL,"
		+ "close REAL NOT NULL,"
		+ "adjusted_close REAL NOT NULL,"
		+ "volume INT NOT NULL,"
		+ "PRIMARY KEY(symbol, date)"
		+ ");";


	rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Created PairTwoPrices table." << endl << endl;
	}

	return 0;

};

int PopulatePairTable(sqlite3*& db) 
{
	int rc = 0;
	char* error = nullptr;

	ifstream fp("PairTrading.txt");
	string line;
	char sqlInsert[512];
	int id = 0;
	while (getline(fp, line)) 
	{
		id += 1;
		vector<int> data_line;
		string symbol1, symbol2;
		istringstream readstr(line);
		getline(readstr, symbol1, ',');
		getline(readstr, symbol2);
		sprintf_s(sqlInsert, "INSERT INTO StockPairs (id, symbol1, symbol2, volatility, profit_loss) VALUES (%d, \"%s\", \"%s\", %f, %f)", id, symbol1.c_str(), symbol2.c_str(), 0.0, 0.0);

		rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
		if (rc)
		{
			std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
			sqlite3_free(error);
		}
		else
		{
			std::cout << "Inserted a value into table StockPairs." << std::endl << std::endl;
		}
	}
	return 0;
};

int InsertTable(sqlite3* db, const Stock& stock, string TableName)
{
	int rc = 0;
	char* error = nullptr;

	// Execute SQL
	char sqlInsert[512];

	vector<DailyTrade> dailyTrades = stock.GetDailyTrade();

	for (vector<DailyTrade>::iterator iter = dailyTrades.begin(); iter != dailyTrades.end(); iter++)
	{
		std::cout << "Inserting a value into table " << TableName <<  " ..." << std::endl;
		
		sprintf_s(sqlInsert, "INSERT INTO %s(Symbol, Date, Open, High, Low, Close, Adjusted_close, Volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", TableName.c_str(), stock.GetSymbol().c_str(), (*iter).GetDate().c_str(), (*iter).GetOpen(), (*iter).GetHigh(), (*iter).GetLow(), (*iter).GetClose(), (*iter).GetAdjustedClose(), (*iter).GetVolume());
		rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
		if (rc)
		{
			std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
			sqlite3_free(error);
		}
		else
		{
			std::cout << "Inserted a value into table " << TableName << "."<< std::endl << std::endl;
		}
	}

	return 0;
};

