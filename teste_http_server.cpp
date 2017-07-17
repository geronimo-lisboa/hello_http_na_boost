#include <iostream>
#include <sstream>
#include <boost\asio.hpp>
#include <ctime>
using namespace std;
using boost::asio::ip::tcp;
int main(int argc, char** argv)
{
	try
	{
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 40001));
		while (true)
		{
			tcp::socket socket(io_service);
			acceptor.accept(socket);
			//Aceitou uma conexao
			time_t now = time(0);
			stringstream ssHtml;
			ssHtml << "HELLO WORLD ";
			stringstream ssHttp;
			ssHttp << "HTTP/1.1 200 OK\r\n";
			ssHttp << "Connection: close\r\n";
			ssHttp << "Content-Type: text/html\r\n";
			ssHttp << "Content-Length: " << ssHtml.str().size() <<"\r\n";
			ssHttp << ssHtml.str();
			ssHttp << "\r\n\r\n";
			boost::system::error_code ignored_error;
			cout << ssHttp.str();
			boost::asio::write(socket, boost::asio::buffer(ssHttp.str()), ignored_error);
		}
	}
	catch (std::exception &ex)
	{
		cerr << ex.what() << endl;
	}
	return 0;
}