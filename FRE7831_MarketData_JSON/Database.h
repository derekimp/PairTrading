#pragma once


#include <sqlite3.h>
#include <string>

#include "Marketdata.h"
#include "Stock.h"

string quotesql(const string& s) {
	return string("\"") + s + string("\"");
}
int callback(void* NotUsed, int argc, char** argv,
	char** azColName) {

	NotUsed = 0;

	for (int i = 0; i < argc; i++) {

		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");

	return 0;
}
class Stock;
int OpenDatabase(sqlite3*& db) {
	int rc;
	rc = sqlite3_open("C:\\Users\\User\\Desktop\\lwp\\Spring 2021\\7831\\Lab1_lp2496\\MarketData.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 1;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
		return 0;
	}
	//sqlite3_close(db);
}
int CreateTables(sqlite3*& db) {
	char* zErrMsg = 0;
	int rc;
	string sqlDrop = string("DROP TABLE IF EXISTS DailyTrades; DROP TABLE IF EXISTS IntradayTrades;");

	string sqlCreateDailyTradeTable = string("CREATE TABLE IF NOT EXISTS DailyTrades ")
		+ "(Symbol CHAR(20) NOT NULL,"
		//+ "ID REAL NOT NULL,"
		+ "Date CHAR(20) NOT NULL,"
		+ "Open REAL NOT NULL,"
		+ "High REAL NOT NULL,"
		+ "Low REAL NOT NULL,"
		+ "Close REAL NOT NULL,"
		+ "Adjusted_close REAL NOT NULL,"
		+ "Volume INT NOT NULL,"
		+ "PRIMARY KEY(Symbol, Date),"
		+ "Foreign Key(Symbol) references Stocks(Symbol)\n"
		+ "ON DELETE CASCADE\n"
		+ "ON UPDATE CASCADE"
		+ ");";
	string sqlCreateIntraTradeTable = string("CREATE TABLE IF NOT EXISTS IntradayTrades ")
		+ "(Symbol CHAR(20) NOT NULL,"
		+ "Date CHAR(20) NOT NULL,"
		+ "Timestamp CHAR(20) NOT NULL,"
		+ "Open REAL NOT NULL,"
		+ "High REAL NOT NULL,"
		+ "Low REAL NOT NULL,"
		+ "Close REAL NOT NULL,"
		+ "Volume INT NOT NULL,"
		+ "PRIMARY KEY(Symbol, Date, Timestamp),"
		+ "Foreign Key(Symbol, Date) references DailyTrades(Symbol, Date)\n"
		+ "ON DELETE CASCADE\n"
		+ "ON UPDATE CASCADE\n"
		+ ");";

	rc = sqlite3_exec(db, sqlDrop.c_str(), NULL, 0, &zErrMsg);
	rc = sqlite3_exec(db, sqlCreateDailyTradeTable.c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Daily Trade Table created successfully\n");
	}
	rc = sqlite3_exec(db, sqlCreateIntraTradeTable.c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Intraday Trade Table created successfully\n");
	}

	return 1;

}
int InsertDailyTable(sqlite3* db, map<string, Stock>& dailyMap) {
	char* zErrMsg = 0;
	int rc = 0;
	int ID = 0;
	for (auto const& itr : dailyMap) {

		string symbol = itr.first;
		cout << itr.second.GetDailyTrade().size();
		vector<DailyTrade> dailytrade_vector = itr.second.GetDailyTrade();
		
		string sqlInsertTable;
		for (DailyTrade dailytrade_daily : dailytrade_vector) {
			ID++;
			string date = dailytrade_daily.GetDate();

	

			float open = dailytrade_daily.GetOpen();
			float high = dailytrade_daily.GetHigh();
			float low = dailytrade_daily.GetLow();
			float close = dailytrade_daily.GetClose();
			float adj = dailytrade_daily.GetAdjustedClose();
			int volume = dailytrade_daily.GetVolume();
			
			string value = "(" + quotesql(symbol)  + "," + quotesql(date) + "," + to_string(open) + "," + to_string(high) + "," + to_string(low) + "," + to_string(close) + "," + to_string(adj) + "," + to_string(volume);
			
			//(Symbol, Date, Open, High, Low, Close, Adjusted_close, Volume)
			sqlInsertTable += string("INSERT INTO DailyTrades")
				+ " Values " +value + ");";

			//cout << sqlInsertTable << endl << endl;
		}

		rc = sqlite3_exec(db, sqlInsertTable.c_str(), callback, 0, &zErrMsg);

		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			
		}
		else {
			fprintf(stdout, "Records created successfully\n");
			
		}
		
		
	}
	
	return 0;
	
}

int InsertIntraTable(sqlite3* db, map<string, Stock>& intraMap) {
	char* zErrMsg = 0;
	int rc;



	for (auto const& itr : intraMap) {

		string symbol = itr.first;
		vector<IntradayTrade> dailytrade_vector = itr.second.GetIntradayTrade();
		string sqlInsertTable;
		for (IntradayTrade dailytrade_daily : dailytrade_vector) {
			string date = dailytrade_daily.GetDate();
			string timestamp = dailytrade_daily.GetTimestamp();
			float open = dailytrade_daily.GetOpen();
			float high = dailytrade_daily.GetHigh();
			float low = dailytrade_daily.GetLow();
			float close = dailytrade_daily.GetClose();
			int volume = dailytrade_daily.GetVolume();
			string value = "(" + quotesql(symbol) + "," + quotesql(date) + "," + quotesql(timestamp) + "," + to_string(open) + "," + to_string(high) + "," + to_string(low) + "," + to_string(close) + ","  + to_string(volume);

			sqlInsertTable += string("INSERT INTO IntradayTrades")
				+ " Values " + value + ");";
		}

		rc = sqlite3_exec(db, sqlInsertTable.c_str(), callback, 0, &zErrMsg);

		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);

		}
		else {
			fprintf(stdout, "Records created successfully\n");

		}
	}
	return 0;
}

int DisplayTables(sqlite3* db, std::string sqlSelect) {

	char* err_msg = 0;
	int rc;

	string sql = "SELECT * FROM " + sqlSelect;

	rc = sqlite3_exec(db, sql.c_str(), callback, 0, &err_msg);

	if (rc != SQLITE_OK) {

		fprintf(stderr, "Failed to select data\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);

		return 1;
	}



	return 0;
}
