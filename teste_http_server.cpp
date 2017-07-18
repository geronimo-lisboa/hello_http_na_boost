#include <iostream>
#include <sstream>
#include <boost\asio.hpp>
#include <ctime>
#include <map>
#include <sstream>
#include <string>
using namespace std;
using boost::asio::ip::tcp;

class http_headers
{
	std::string method, url, version;
	std::map<std::string, std::string> headers;
public:
	std::string get_response()
	{
		std::stringstream ssOut;
		if (url == "/hello")
		{
			std::string sHTML = "<html><body><h1>Hello World</h1><p>This is a test web server in c++</p></body></html>";
			ssOut << "HTTP/1.1 200 OK" << std::endl;
			ssOut << "content-type: text/html" << std::endl;
			ssOut << "content-length: " << sHTML.length() << std::endl;
			ssOut << std::endl;
			ssOut << sHTML;
		}
		else
		{
			std::string sHTML = "<html><body><h1>404 Not Found</h1><p>There's nothing here.</p></body></html>";
			ssOut << "HTTP/1.1 404 Not Found" << std::endl;
			ssOut << "content-type: text/html" << std::endl;
			ssOut << "content-length: " << sHTML.length() << std::endl;
			ssOut << std::endl;
			ssOut << sHTML;
		}
		return ssOut.str();
	}
	int content_length()
	{
		auto request = headers.find("content-length");
		if (request != headers.end())
		{
			std::stringstream ssLength(request->second);
			int content_length;
			ssLength >> content_length;
			return content_length;
		}
		return 0;
	}
	void on_read_header(std::string line)
	{
		std::stringstream ssHeader(line);
		std::string headerName;
		std::getline(ssHeader, headerName, ':');
		std::string value;
		std::getline(ssHeader, value);
		headers[headerName] = value;
	}
	void on_read_request_line(std::string line)
	{
		std::stringstream ssRequestLine(line);
		ssRequestLine >> method;
		ssRequestLine >> url;
		ssRequestLine >> version;
		std::cout << "request for resource: " << url << std::endl;
	}
};
//
//class session
//{
//public:
//	boost::asio::ip::tcp::socket socket;
//	boost::asio::streambuf buff;
//	http_headers headers;
//	void read_body()
//	{
//		int nBuffer = 1000;
//		std::shared_ptr<std::vector<char>> bufptr = std::make_shared<std::vector<char>>(nBuffer);
//		boost::asio::read(socket, boost::asio::buffer(*bufptr, nBuffer), [](const error_code& e, std::size_t s)
//		{
//		});
//	}
//	void read_next_line()
//	{
//		boost::asio::read_until(socket, buff, '\r');
//
//	}
//};
class session
{
	http_headers headers;
public:
	boost::asio::streambuf buff;
	boost::asio::ip::tcp::socket socket;

	session(boost::asio::io_service& io_service) :socket(io_service)
	{

	}
	static void interact(std::shared_ptr<session> pThis);
	static void readFirstLine(std::shared_ptr<session> pThis);
	static void read_next_line(std::shared_ptr<session> pThis);
};
void session::read_next_line(std::shared_ptr<session> pThis)
{
	boost::asio::async_read_until(pThis->socket, pThis->buff, '\r', [pThis](const boost::system::error_code& e, std::size_t s){
		std::string line, ignore;
		std::istream stream{ &pThis->buff };
		std::getline(stream, line, '\r');
		std::getline(stream, ignore, '\n');

		cout << line << endl;
		cout << ignore << endl;

		pThis->headers.on_read_header(line);
		if (line.length() == 0)
		{
			if (pThis->headers.content_length() == 0)
			{
				std::shared_ptr<std::string> str = std::make_shared<std::string>(pThis->headers.get_response());
				boost::asio::async_write(pThis->socket, boost::asio::buffer(str->c_str(), str->length()), [pThis, str](const boost::system::error_code& e, std::size_t s)
				{
					std::cout << "done" << std::endl;
					
				});
			}
		}
		else
		{
			pThis->read_next_line(pThis);
		}
	});
}
void session::readFirstLine(std::shared_ptr<session> pThis)
{
	boost::asio::async_read_until(pThis->socket, pThis->buff, '\r', [pThis](const boost::system::error_code& e, std::size_t s){
		std::string line, ignore;
		std::istream stream{ &pThis->buff };
		std::getline(stream, line, '\r');
		std::getline(stream, ignore, '\n');
		
		cout << line << endl;
		cout << ignore << endl;

		pThis->headers.on_read_request_line(line);
		pThis->read_next_line(pThis);
	});
}
void session::interact(std::shared_ptr<session> pThis)
{
	readFirstLine(pThis);
}

void acceptConnections(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::io_service& io_service)
{
	std::shared_ptr<session> my_session =  make_shared<session>(io_service);
	acceptor.async_accept(my_session->socket, [my_session, &acceptor, &io_service](const boost::system::error_code& accept_error)
	{
		acceptConnections(acceptor, io_service);
		if (!accept_error)
		{
			std::cout << "aceitou conexão" << endl;
			session::interact(my_session);
		}
		else
		{
			std::cout << "erro " << accept_error.value() << " " << accept_error.message() << std::endl;
		}
	});

}

int main(int argc, char** argv)
{
	try
	{
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 40001));
		acceptor.listen();
		acceptConnections(acceptor, io_service);
		io_service.run();
	}
	catch (std::exception &ex)
	{
		cerr << ex.what() << endl;
	}
	return 0;
}