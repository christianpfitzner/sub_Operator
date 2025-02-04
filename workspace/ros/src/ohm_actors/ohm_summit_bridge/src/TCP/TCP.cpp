/**
 * @file  : TCP.cpp
 *
 * @date  : 03.10.2012
 * @author: m1ch1
 */

#include "TCP.h"

namespace apps
{
using boost::asio::ip::tcp;
using namespace std;

TCP::TCP(unsigned int port)		//server
{
	_choosenMode = server;
	_port = port;

	_delay = 2000000;

	_server = new tcp::acceptor(_ioService, tcp::endpoint(tcp::v4(), _port));
	_socket = NULL;

	_targetServer = NULL;


}

TCP::TCP(const char* ip, unsigned int port)		//client
{
	_choosenMode = client;
	_port = port;
	_ip = ip;

	_delay = 2000000;

	_targetServer = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(_ip),_port);
	_socket = NULL;


	_server = NULL;
}

TCP::~TCP()
{
}

unsigned int TCP::read(void* data, unsigned int size)
{
	try{
    _socket->read_some(boost::asio::buffer(data,size));
	}
	catch(boost::system::system_error& e)
    {
	    //get new connection
	    this->connectOnce();
    }
	return 0;
}

unsigned int TCP::connectOnce()
{
    if(_choosenMode == server)
    {
        delete _socket;
        _socket = new tcp::socket(_ioService);
        _server->accept(*_socket);  //wait for connection....
    }
    else if(_choosenMode == client)
    {

        bool ok = true;
        do{
            try{
                delete _socket;
                _socket = new boost::asio::ip::tcp::socket(_ioService);
                _socket->connect(*_targetServer);
                ok = true;
            }
            catch(boost::system::system_error& e)
            {
                ok = false;
                usleep(_delay);
            }
        }while(!ok);
    }
    return 0;
}

unsigned int TCP::write(void* data, unsigned int size)
{
    try{
	_socket->write_some(boost::asio::buffer(data,size));
    }
    catch(boost::system::system_error& e)
    {
        //get new connection
         this->connectOnce();
    }
	return 0;
}

} /* namespace apps */
