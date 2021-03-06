// SimpleServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
# include <sys/time.h>
#endif

/////////////////////////////////
//  A Simple Web Application with C++ REST SDK
//  We can use PostMan Or Curl to test the Server
//
//
using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;
/////////////////////////////
// SimpleServer is a Wrapper over 
// http_listener class available with C++ REST SDK
//
//
class SimpleServer
{
public:

	SimpleServer(utility::string_t url);
	~SimpleServer() {}
	pplx::task<void> Open() { return m_listener.open(); }
	pplx::task<void> Close() { return m_listener.close(); }

private:
	//--- Handlers for HTTP verbs
    void HandleGet(http_request message);
	void HandlePut(http_request message);
	void HandlePost(http_request message);
	void HandleDelete(http_request message);
	//--------------- The  HTTP listener class
	http_listener m_listener;
};


//////////////////////////////////
// The Constructor Binds HTTP verbs to instance methods
// Based on the naming convention, we can infer what is happening
SimpleServer::SimpleServer(utility::string_t url) : m_listener(url)
{
	m_listener.support(methods::GET, std::bind(&SimpleServer::HandleGet, this, std::placeholders::_1));
	m_listener.support(methods::PUT, std::bind(&SimpleServer::HandlePut, this, std::placeholders::_1));
	m_listener.support(methods::POST, std::bind(&SimpleServer::HandlePost, this, std::placeholders::_1));
	m_listener.support(methods::DEL, std::bind(&SimpleServer::HandleDelete, this, std::placeholders::_1));

}
/////////////////////////////////////
// For this implementation, what we do is 
// spit the HTTP request details on the Server Console
// and return 200 OK and a String which indicates 
// Success of Operations 
//
void SimpleServer::HandleGet(http_request message)
{
	ucout << message.to_string() << endl;
	message.reply(status_codes::OK,L"GET Operation Succeeded");
};

void SimpleServer::HandlePost(http_request message)
{
	ucout << message.to_string() << endl;
	message.reply(status_codes::OK, L"POST Operation Succeeded");
};

void SimpleServer::HandleDelete(http_request message)
{
	ucout << message.to_string() << endl;
	message.reply(status_codes::OK, L"DELETE Operation Succeeded");
}

void SimpleServer::HandlePut(http_request message)
{
	ucout << message.to_string() << endl;
	message.reply(status_codes::OK, L"PUT Operation Succeeded");
};

////////////////////////////////
// A Smart Pointer for Server Instance...
//
std::unique_ptr<SimpleServer> g_http;

//////////////////////////////////////////////////
// STart the Server with the Given URL
//
void StartServer(const string_t& address)
{
	// Build our listener's URI from the address given
	// We just append DBDEMO/ to the base URL
	uri_builder uri(address);
	uri.append_path(U("DBDEMO/"));
	auto addr = uri.to_uri().to_string();
	/////////////////////////////////
	// Create an Instance of the Server and Invoke Wait to 
	// start the Server...
	g_http = std::unique_ptr<SimpleServer>(new SimpleServer(addr));
	g_http->Open().wait();
	//---- Indicate the start and spit URI to the Console
	ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;

	return;
}

////////////////////////////////////////
// Simply Closes the Connection... Close returns 
// pplx::task<void> ...we need to Call wait to invoke the 
// operation...
void ShutDown()
{
	g_http->Close().wait();
	return;
}

///////////////////////////////
//
//
//

int wmain(int argc, wchar_t *argv[])
{
	utility::string_t port = U("34567");
	if (argc == 2)
	{
		port = argv[1];
	}
	//--- Create the Server URI base address
	utility::string_t address = U("http://localhost:");
	address.append(port);

	StartServer(address);
	std::cout << "Press ENTER to exit." << std::endl;
	//--- Wait Indefenintely, Untill some one has 
	// pressed a key....and Shut the Server down
	std::string line;
	std::getline(std::cin, line);
	ShutDown();
	return 0;
}


