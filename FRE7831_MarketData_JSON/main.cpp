#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>

using namespace std;

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

int main(void)
{
	sqlite3* db = NULL;
	//OpenDatabase needs to be defined
	if (OpenDatabase(db) != 0)	  return -1;
	bool bCompleted = false;
	char* zErrMsg1 = 0;
	int rc1 = 0;
	while (!bCompleted)
	{
		string str;
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

		}
		case 2:
		{

		}
		case 3:
		{

		}
		case 4:
		{

		}
		case 5:
		{

		}
		case 6:
		{

		}
		case 7:
		{

		}
		case 8:
		{

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

