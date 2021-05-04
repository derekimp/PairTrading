#pragma once
#include <vector>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include "MarketData.h"
#include "Stock.h"

class Stock;

//writing call back function for storing fetched values in memory
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int PullMarketData(const std::string& url_request, std::string& read_buffer) 
{
	//global initiliation of curl before calling a function
	curl_global_init(CURL_GLOBAL_ALL);

	//creating session handle
	CURL* handle;

	// Store the result of CURL�s webpage retrieval, for simple error checking.
	CURLcode result;

	// notice the lack of major error-checking, for brevity
	handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

	//adding a user agent
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

	// send all data to this function 
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function 
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &read_buffer);

	//perform a blocking file transfer
	result = curl_easy_perform(handle);

	// check for errors 
	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
		return -1;
	}
	//std::cout << read_buffer << std::endl;
	
	curl_easy_cleanup(handle);

	return 0;

};

int RetrieveData(std::string start_date, std::string end_date, std::vector<std::string> symbol1, std::vector<std::string> symbol2, std::map<std::string, std::string>& read_buffer1, std::map<std::string, std::string>& read_buffer2)
{
	// Retrieve and populate data
	std::string url_common1 = "https://eodhistoricaldata.com/api/eod/";
	std::string symbol, url_request;
	std::string api_token = "601390a0a753f2.12891709";

	for (std::vector<std::string>::iterator iter = symbol1.begin(); iter != symbol1.end(); iter++)
	{
		symbol = (*iter);

		url_request = url_common1 + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
		PullMarketData(url_request, read_buffer1[symbol]);
		std::cout << "Pulling " << symbol << std::endl;
	}

	for (std::vector<std::string>::iterator iter = symbol2.begin(); iter != symbol2.end(); iter++)
	{
		symbol = (*iter);

		url_request = url_common1 + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
		PullMarketData(url_request, read_buffer2[symbol]);
		std::cout << "Pulling " << symbol << std::endl;
	}

	return 0;
};

int PopulateDailyTrades(const std::string& read_buffer, Stock& stock)
{
	//json parsing
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	Json::Value root;   // will contains the root value after parsing.
	std::string errors;

	bool parsingSuccessful = reader->parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), &root, &errors);
	if (not parsingSuccessful)
	{
		// Report failures and their locations in the document.
		std::cout << "Failed to parse JSON" << std::endl << read_buffer << errors << std::endl;
		return -1;
	}
	std::cout << "\nSucess parsing json\n" << root << std::endl;
	std::string date;
	float open, high, low, close, adjusted_close;
	long volume;

	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++) // iterate the Json list
	{
		date = (*itr)["date"].asString();	// read value from Json Object, it is a C++ map
		open = (*itr)["open"].asFloat();
		high = (*itr)["high"].asFloat();
		low = (*itr)["low"].asFloat();
		close = (*itr)["close"].asFloat();
		adjusted_close = (*itr)["adjusted_close"].asFloat();
		volume = (*itr)["volume"].asInt64();
		DailyTrade aTrade(date.c_str(), open, high, low, close, adjusted_close, volume);
		stock.addDailyTrade(aTrade);

		//cout << stock;

	}
	return 0;

};

int PopulateTable(sqlite3* db, std::vector<std::string> symbols, std::map<std::string, std::string>& read_buffer, std::string TableName)
{
	for (std::vector<std::string>::iterator iter = symbols.begin(); iter != symbols.end(); iter++)
	{
		std::string symbol = (*iter);
		Stock stock(symbol);
		PopulateDailyTrades(read_buffer[symbol], stock);
		InsertTable(db, stock, TableName);
	}

	return 0;
};
