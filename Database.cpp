#pragma once
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "Database.h"
#include "Stock.h"

using namespace std;

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
		std::cout << "Inserting a value into table " << TableName << " ..." << std::endl;

		sprintf_s(sqlInsert, "INSERT INTO %s(Symbol, Date, Open, High, Low, Close, Adjusted_close, Volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", TableName.c_str(), stock.GetSymbol().c_str(), (*iter).GetDate().c_str(), (*iter).GetOpen(), (*iter).GetHigh(), (*iter).GetLow(), (*iter).GetClose(), (*iter).GetAdjustedClose(), (*iter).GetVolume());
		rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
		if (rc)
		{
			std::cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << std::endl << std::endl;
			sqlite3_free(error);
		}
		else
		{
			std::cout << "Inserted a value into table " << TableName << "." << std::endl << std::endl;
		}
	}

	return 0;
};

int CreatePairPricesTable(sqlite3*& db)
{
	int rc = 0;
	char* error = nullptr;

	// Create PairPrices table
	cout << "Creating PairPrices table ..." << endl;
	string sqlCreateTable = string("CREATE TABLE IF NOT EXISTS PairPrices ")
		+ "(symbol1 CHAR(20) NOT NULL,"
		+ "symbol2 CHAR(20) NOT NULL,"
		+ "date CHAR(20) NOT NULL,"
		+ "open1 REAL NOT NULL,"
		+ "close1 REAL NOT NULL,"
		+ "open2 REAL NOT NULL,"
		+ "close2 REAL NOT NULL,"
		+ "profit_loss FLOAT NOT NULL,"
		+ "PRIMARY KEY(symbol1, symbol2, date)"
		+ ");";

	rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Created PairPrices table." << endl << endl;
	}

	//fullfill the table
	cout << "Extracting data from other tables ..." << endl;
	string sqlExtract = string("Insert into PairPrices ")
		+ "Select StockPairs.symbol1 as symbol1, StockPairs.symbol2 as symbol2, "
		+ "PairOnePrices.date as date, PairOnePrices.open as open1, PairOnePrices.close as close1, "
		+ "PairTwoPrices.open as open2, PairTwoPrices.close as close2, 0 as profit_loss "
		+ "From StockPairs, PairOnePrices, PairTwoPrices "
		+ "Where (((StockPairs.symbol1 = PairOnePrices.symbol) and (StockPairs.symbol2 = PairTwoPrices.symbol)) and "
		+ "(PairOnePrices.date = PairTwoPrices.date)) ORDER BY symbol1, symbol2;";

	rc = sqlite3_exec(db, sqlExtract.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "PairPrices table done." << endl << endl;
	}

	return 0;
};

int CalVol(sqlite3*& db)
{
	int rc = 0;
	char* error = nullptr;
	string back_test_start_date = "2021-01-01";

	// Calculate Vol
	cout << "Calculating volatility ..." << endl;
	string calculate_volatility_for_pair = string("Update StockPairs SET volatility =")
		+ "(SELECT(AVG((Close1/Close2)*(Close1/Close2)) - AVG(Close1/Close2)*AVG(Close1/Close2)) as variance "
		+ "FROM PairPrices "
		+ "WHERE StockPairs.symbol1 = PairPrices.symbol1 AND StockPairs.symbol2 = PairPrices.symbol2 AND PairPrices.date <= \'"
		+ back_test_start_date + "\');";

	rc = sqlite3_exec(db, calculate_volatility_for_pair.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Volatility done." << endl << endl;
	}

	return 0;
};

int CalDailyPnL(sqlite3*& db, double k)
{
	int rc = 0;
	char* error = nullptr;
	string back_test_start_date = "2021-01-01";
	string k_string = to_string(k);

	// Calculate Vol
	cout << "Calculating Daily PnL ..." << endl;
	string create_temp = string("CREATE TABLE IF NOT EXISTS temp_ as ")
		+ "SELECT PairPrices.symbol1, PairPrices.symbol2, PairPrices.date, CASE WHEN (ABS(lag(PairPrices.close1,1)over(ORDER BY PairPrices.ROWID)/lag(PairPrices.close2,1)over(ORDER BY PairPrices.ROWID) - PairPrices.open1/PairPrices.open2) > 1.0 * StockPairs.volatility) "
		+ "THEN (10000*(PairPrices.open1-PairPrices.close1) + 10000*PairPrices.open1/PairPrices.open2*(PairPrices.close2 - PairPrices.open2)) "
		+ "ELSE (10000*(PairPrices.close1-PairPrices.open1) + 10000*PairPrices.open1/PairPrices.open2*(PairPrices.open2 - PairPrices.close2)) END AS pnl "
		+ "FROM PairPrices "
		+ "INNER JOIN StockPairs on StockPairs.symbol1 = PairPrices.symbol1 ";
	rc = sqlite3_exec(db, create_temp.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Temporary table created." << endl << endl;
	}


	string cal_Daily_PnL = string("UPDATE PairPrices SET profit_loss = (")
		+ "SELECT pnl FROM temp_ WHERE temp_.ROWID = PairPrices.ROWID)";
	error = nullptr;
	rc = sqlite3_exec(db, cal_Daily_PnL.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Daily PnL done." << endl << endl;
	}


	string drop_temp = string("DROP TABLE temp_");
	error = nullptr;
	rc = sqlite3_exec(db, cal_Daily_PnL.c_str(), NULL, NULL, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Drop temporary table." << endl << endl;
	}

	return 0;
};

int CalSumPnL(sqlite3*& db)
{
	int rc = 0;
	char* error = nullptr;
	string back_test_start_date = "2021-01-01";

	// Calculate Vol
	cout << "Calculating Sum PnL ..." << endl;
	string calculate_volatility_for_pair = string("UPDATE StockPairs set profit_loss = ")
		+ "(Select sum(PairPrices.profit_loss)"
		+ "FROM PairPrices "
		+ "WHERE PairPrices.date >= \'"
		+ back_test_start_date + "\' "
		+ "and Pairprices.symbol1 = StockPairs.symbol1 "
		+ "GROUP by PairPrices.symbol1)";

	rc = sqlite3_exec(db, calculate_volatility_for_pair.c_str(), NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Sum PnL done." << endl << endl;
	}

	return 0;
};

void ManualTest(sqlite3*& db)
{
	string symbol1, symbol2;
	string trade_date;
	//'close1' is the close price for stock1, 'd1' stands for the day d-1 
	float close1d1, close2d1, open1d2, open2d2, close1d2, close2d2;
	int k;
	
	cout << "Please input parameters for test pair trade:" << endl;    
	cout << "Please enter stock1 symbol:" << endl;
	cin >> symbol1;
	cout << "Please enter stock2 symbol:" << endl;
	cin >> symbol2;
	cout << "Please enter the close price on day d-1 for stock1:" << endl;
	cin >> close1d1;
	cout << "Please enter the close price on day d-1 for stock2:" << endl;
	cin >> close2d1;
	cout << "Please enter the open price on day d for stock1:" << endl;
	cin >> open1d2;
	cout << "Please enter the open price on day d for stock2:" << endl;
	cin >> open2d2;
	cout << "Please enter the close price on day d for stock1:" << endl;
	cin >> close1d2;
	cout << "Please enter the close price on day d for stock2:" << endl;
	cin >> close2d2;
	cout << "Please enter the date for trade(YYYY-MM-DD):" << endl;
	cin >> trade_date;
	cout << "Please enter K:" << endl;
	cin >> k;
	
	//look for the pair trade in PairPrices table and extract data from the table
	float variance;
	float profit_loss;
	int rc = 0;
	char* error = nullptr;
	char** result1 = nullptr;
	char** result2 = nullptr;

	//Extract variance data
	string search_for_pair_trade = string("SELECT volatility ")
		+ "FROM StockPairs "
		+ "WHERE symbol1 = \'"
		+ symbol1 + "\' and symbol2 = \'"
		+ symbol2 + "\'";
	int col = 0, row = 0;
	rc = sqlite3_get_table(db, search_for_pair_trade.c_str(), &result1, &row, &col, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Data extract succesful." << endl << endl;
	}
	variance = atof(result1[1]);
	sqlite3_free_table(result1);

	//Extract profit_loss data
	string search_for_profit_loss = string("SELECT profit_loss ")
		+ "FROM PairPrices "
		+ "WHERE symbol1 = \'"
		+ symbol1 + "\' and symbol2 = \'"
		+ symbol2 + "\' and date = \'"
		+ trade_date + "\'";
	rc = sqlite3_get_table(db, search_for_profit_loss.c_str(), &result2, &row, &col, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Data extract succesful." << endl << endl;
	}
	profit_loss = atof(result2[1]);
	sqlite3_free_table(result2);

	//calculate N1, N2 and profitloss manually
	int N1 = 0, N2 = 0;
	if (abs(close1d1 / close2d1 - open1d2 / open2d2) > k * sqrt(variance))
	{
		N1 = -10000;
		N2 = -int(N1 * (open1d2 / open2d2));
	}
	else
	{
		int N1 = 10000;
		N2 = -int(N1 * (open1d2 / open2d2));
		float cal_profit_loss = -N1 * (open1d2 - close1d2) - N2 * (open2d2 - close2d2);
		cout << "The manual test result for pair (" << symbol1 << ", " << symbol2 << "):" << endl;
		cout << "variance = " << variance << endl;
		cout << "N1 = " << N1 <<"  "<< "N2 = " << N2 << endl;
		cout << "profit_loss = " << cal_profit_loss << endl;
		cout << "profit_loss in PairPrices Table: " << profit_loss << endl;
	}
}

void DropAllTable(sqlite3*& db)
{
	int rc = 0;
	char* error = nullptr;

	// Drop all the tables
	cout << "Dropping all tables ..." << endl;

	string sqlDropPairPrices = "DROP TABLE IF EXISTS PairPrices;";
	string sqlDropPairOnePrices = "DROP TABLE IF EXISTS PairOnePrices;";
	string sqlDropPairTwoPrices = "DROP TABLE IF EXISTS PairTwoPrices;";
	string sqlDropStockPairs = "DROP TABLE IF EXISTS StockPairs;";

	rc = sqlite3_exec(db, sqlDropPairPrices.c_str(), NULL, NULL, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Succesfully Drop PairPrices." << endl << endl;
	}

	rc = sqlite3_exec(db, sqlDropPairOnePrices.c_str(), NULL, NULL, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Succesfully Drop PairOnePrices." << endl << endl;
	}

	rc = sqlite3_exec(db, sqlDropPairTwoPrices.c_str(), NULL, NULL, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Succesfully Drop PairTwoPrices." << endl << endl;
	}

	rc = sqlite3_exec(db, sqlDropStockPairs.c_str(), NULL, NULL, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Succesfully Drop StockPairs." << endl << endl;
	}
}