/**
 * @file  : TCP.h
 *
 * @date  : 03.10.2012
 * @author: m1ch1
 */

#ifndef TCP_H_
#define TCP_H_

#include <iostream>
#include <string>
#include <boost/asio.hpp>


namespace apps
{
using boost::asio::ip::tcp;

class TCP
{
private:	//data elements

	enum _mode {server,client};
	_mode _choosenMode;

	unsigned int _delay;

	boost::asio::io_service _ioService;
	unsigned int _port;
	std::string _ip;

	boost::asio::ip::tcp::endpoint* _targetServer;	//used when client

	boost::asio::ip::tcp::socket* _socket;

	tcp::acceptor* _server;							//used when server


private:	//functions
public:
	TCP(unsigned int port);						//server
	TCP(const char* ip, unsigned int port);		//client
	virtual ~TCP();

	unsigned int connectOnce();

	unsigned int read(void* data, unsigned int size);
	unsigned int write(void* data, unsigned int size);
};

} /* namespace apps */
#endif /* TCP_H_ */
