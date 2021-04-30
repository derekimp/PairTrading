#pragma once
#include <string>
#include "json/json.h"
#include "curl/curl.h"
#include "Database.h"
#include "Stock.h"
class Stock;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


int PullDailyMarketData(const std::string& url_request, std::string& readBuffer, string& symbol, map<string, Stock>& dailyMap) {
	//global initiliation of curl before calling a function
	curl_global_init(CURL_GLOBAL_ALL);

	//creating session handle
	CURL* handle;

	// Store the result of CURL’s webpage retrieval, for simple error checking.
	CURLcode result;

	// notice the lack of major error-checking, for brevity
	handle = curl_easy_init();

	//after creating handle we will start transfering webpage
	//curl_easy_setopt is used to tell libcurl how to behave.
	//By setting the appropriate options, the application can change libcurl's behavior.

	curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

	//adding a user agent
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

	// send all data to this function 
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function 
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &readBuffer);

	//perform a blocking file transfer
	result = curl_easy_perform(handle);

	// check for errors 
	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
	}
	else
	{
		//json parsing
		Json::CharReaderBuilder builder;
		Json::CharReader* reader = builder.newCharReader();
		Json::Value root;   // will contains the root value after parsing.
		string errors;

		bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
		if (not parsingSuccessful)
		{
			// Report failures and their locations
			// in the document.
			cout << "Failed to parse JSON" << endl << readBuffer << errors << endl;
			return -1;
		}
		else
		{
			cout << "\nSucess parsing json\n" << root << endl;
			string date;
			float open, high, low, close, adjusted_close;
			long volume;
			Stock myStock(symbol);
			for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
			{
				date = (*itr)["date"].asString();
				open = (*itr)["open"].asFloat();
				high = (*itr)["high"].asFloat();
				low = (*itr)["low"].asFloat();
				close = (*itr)["close"].asFloat();
				adjusted_close = (*itr)["adjusted_close"].asFloat();
				volume = (*itr)["volume"].asInt64();
				DailyTrade aTrade(date, open, high, low, close, adjusted_close, volume);
				myStock.addDailyTrade(aTrade);
			}
			dailyMap[symbol] = myStock;
			//dailyStock.push_back(myStock);
			cout << myStock;
		}
	}
	curl_easy_cleanup(handle);
	return 0;
}

int PullIntraMarketData(const std::string& url_request, std::string& readBuffer, string symbol, map<string, Stock>& intraMap) {

	//global initiliation of curl before calling a function
	curl_global_init(CURL_GLOBAL_ALL);

	//creating session handle
	CURL* handle;

	// Store the result of CURL’s webpage retrieval, for simple error checking.
	CURLcode result;

	// notice the lack of major error-checking, for brevity
	handle = curl_easy_init();

	//after creating handle we will start transfering webpage
	//curl_easy_setopt is used to tell libcurl how to behave.
	//By setting the appropriate options, the application can change libcurl's behavior.

	curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

	//adding a user agent
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

	// send all data to this function 
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function 
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &readBuffer);

	//perform a blocking file transfer
	result = curl_easy_perform(handle);

	// check for errors 
	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
	}
	else
	{
		//json parsing
		Json::CharReaderBuilder builder;
		Json::CharReader* reader = builder.newCharReader();
		Json::Value root;   // will contains the root value after parsing.
		string errors;

		bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
		if (not parsingSuccessful)
		{
			// Report failures and their locations
			// in the document.
			cout << "Failed to parse JSON" << endl << readBuffer << errors << endl;
			return -1;
		}
		else
		{
			cout << "\nSucess parsing json\n" << root << endl;
			string date;
			float open, high, low, close, adjusted_close;
			long volume;
			Stock myStock(symbol);
			for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
			{
				long Feb_1_2021 = 1612155600;
				long trade_timestamp = (*itr)["timestamp"].asInt64();
				if (trade_timestamp < Feb_1_2021)
					continue;
				string datetime = (*itr)["datetime"].asString();
				size_t current, previous = 0;
				current = datetime.find(' ');
				date = datetime.substr(previous, current - previous);
				previous = current + 1;
				string temp = datetime.substr(previous, datetime.length() - current);
				previous = 0;
				current = temp.find(':');
				int hour = stoi(temp.substr(previous, current - previous));
				hour -= 5;// convert to local time
				previous = current + 1;
				string mins_second = temp.substr(previous, datetime.length() - current);
				string timestamp = to_string(hour) + ":" + mins_second;
				open = (*itr)["open"].asFloat();
				high = (*itr)["high"].asFloat();
				low = (*itr)["low"].asFloat();
				close = (*itr)["close"].asFloat();
				volume = (*itr)["volume"].asInt64();
				IntradayTrade aTrade(date.c_str(), timestamp.c_str(), open, high, low, close, volume);
				myStock.addIntradayTrade(aTrade);
			}
			intraMap[symbol] = myStock;
			//intraStock.push_back(myStock);
			cout << myStock;
		}
	}
	curl_easy_cleanup(handle);
	return 0;

}