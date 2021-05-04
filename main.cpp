#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include "Stock.h"  // added for 1, 2, 3, 4
#include "MarketData.h" // added for 1, 2, 3, 4
#include "Database.h" // added for 1, 2, 3, 4, 5, 6, 7
#include <map>


using namespace std;

int OpenDatabase(sqlite3*& db, string name) {
	int rc;
	rc = sqlite3_open(name.c_str(), &db);
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

int main(void)
{
	sqlite3* db = NULL;
	sqlite3* db1 = NULL;
	//OpenDatabase needs to be defined
	if (OpenDatabase(db, "Data.db") != 0)	  return -1;
	if (OpenDatabase(db1, "Data_2021.db") != 0)	  return -1;
	bool bCompleted = false;
	char* zErrMsg1 = 0;
	int rc1 = 0;

	map<string, string> readBuffer1; //buffer needed for 2
	map<string, string> readBuffer2; //buffer needed for 2

	while (!bCompleted)
	{
		string str;
		double k = 1.0;
		cout << endl;
		cout << "Pick a choice from the list:" << endl << endl

			//Read and store the pairs of stocks' name from PairTrading.txt into a map
			//Create two tables - PairOnePrices and PairTwoPrices in PairTrading.db
			<< "1.Create and Populate Pair Table" << endl

			//Retrieve data using libcurl and store them into PairOnePrices and PairTwoPrices
			<< "2.Retrieve and Populate Historical Data for Each Stock" << endl

			//Create a table PairPrices
			//From PairOnePrices and PairTwoPrices extract the open and close price for each
			//pair of stock each date and store them into PairPrices
			<< "3.Create PairPrices Table" << endl

			//Create StockPairs Table, extract the pair from PairPrices and calculate their volatility
			<< "4.Create StockPairs Table and Calculate Volatility" << endl

			//According to PairPrices, implement the algorighm and calculate the P/L after each transaction
			<< "5.Back Test" << endl

			//Summarize the P/L and write them into StockPairs
			<< "6.Calculate Profit and Loss for Each Pair" << endl

			//Show user the option of pairs and let them choose
			//Extract and print the P/L for the chosen pair
			<< "7.Mannual Testing" << endl

			//Delete PairOnePrices, PairTwoPrices, PairPrices and StockPairs
			<< "8.Drop All the Tables" << endl
			<< "0.Exit" << endl;
		cin >> str;

		int answer;
		answer = std::stoi(str);
		switch (answer)
		{
		case 1:
		{
			CreatePairTable(db); // Create table StockPairs, PairOnePrices and PairTwoPrices
			PopulatePairTable(db); // Read PairTrading.txt and save it to StockPairs

			CreatePairTable(db1); // Create table StockPairs, PairOnePrices and PairTwoPrices
			PopulatePairTable(db1); // Read PairTrading.txt and save it to StockPairs

			break;
		}
		case 2:
		{
			// get symbols from StockPairs
			vector<string> symbol1, symbol2;

			string sql_select = string("SELECT StockPairs.symbol1 as symbol1,StockPairs.symbol2 as symbol2 From StockPairs");
			int rc = 0;
			char* error = nullptr;

			cout << "Retrieving values in table StockPairs ..." << endl;
			char** results = NULL;
			int rows, columns;
			sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);
			if (rc)
			{
				cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
				sqlite3_free(error);
			}
			else
			{
				// save symbols to vectors symbol1 and symbol2
				for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
				{
					for (int colCtr = 0; colCtr < columns; ++colCtr)
					{
						int cellPosition = (rowCtr * columns) + colCtr;
						if (colCtr == 0) symbol1.push_back(results[cellPosition]);
						else symbol2.push_back(results[cellPosition]);
					}
				}
			}
			sqlite3_free_table(results);

			// Table name
			string Table1 = string("PairOnePrices");
			string Table2 = string("PairTwoPrices");

			string option;
			cout << "Retrieve data before 2021? y for yes" << endl;
			cin >> option;
			if (option == "y")
			{
				// Retrieve data using libcurl
				RetrieveData("2011-01-01", "2020-12-31", symbol1, symbol2, readBuffer1, readBuffer2);
				// store data into PairOnePrices and PairTwoPrices
				PopulateTable(db, symbol1, readBuffer1, Table1);
				PopulateTable(db, symbol2, readBuffer2, Table2);
				readBuffer1.clear();
				readBuffer2.clear();
			}

			cout << "Retrieve data in 2021? y for yes" << endl;
			cin >> option;
			if (option == "y")
			{
			// Retrieve data using libcurl
			RetrieveData("2021-01-01", "2021-06-01", symbol1, symbol2, readBuffer1, readBuffer2);

			// store data into PairOnePrices and PairTwoPrices
			PopulateTable(db1, symbol1, readBuffer1, Table1);
			PopulateTable(db1, symbol2, readBuffer2, Table2);
			}

			break;
		}
		case 3:
		{
			CreatePairPricesTable(db);

			break;
		}
		case 4:
		{
			CalVol(db);

			break;
		}
		case 5:
		{
			cout << "Please Enter K (default 1.0):" << endl;
			cin >> k;
			CalDailyPnL(db, k);
			break;

		}
		case 6:
		{
			CalSumPnL(db);
			break;
		}
		case 7:
		{
			ManualTest(db);
			break;
		}
		case 8:
		{
			DropAllTable(db);
			break;
		}
		case 0:
			sqlite3_close(db);
			bCompleted = !bCompleted;
			break;
		default:
			system("cls");
			break;
		}
	}
}