#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <cstdlib>
#include <boost/thread.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Network.hpp"

using namespace boost::asio;

typedef boost::shared_ptr<udp::socket> udpSocket_ptr;
typedef boost::shared_ptr< std::list< std::tuple< tcpSocket_ptr, time_t, address_v4 > > > clientList_ptr;
typedef boost::shared_ptr< std::queue< std::string > > messageQueue_ptr;

const int bufSize = 128; 
const double heartbeatInterval = 3; // Seconds
const int  udpBroadcastInterval = 1; // Seconds

io_service service;
boost::mutex OutMessageQueue_mtx;
boost::mutex clientList_mtx;
boost::mutex innboundMessages_mtx;
boost::mutex connectedPeers_mtx;
clientList_ptr clientList(new std::list< std::tuple< tcpSocket_ptr, time_t, address_v4 > >);
messageQueue_ptr OutMessageQueue(new std::queue< std::string >);

Network::Network(int port, const address_v4 &myIP) : port(port), myIP(myIP)
{
    new boost::thread(std::bind(&Network::connectionHandler, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(std::bind(&Network::tcpMessageReceive, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(std::bind(&Network::tcpMessageBroadcaster, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));        
    new boost::thread(std::bind(&Network::heartbeat, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(std::bind(&Network::udpBroadcaster, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(std::bind(&Network::udpListener, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));      
}

const address_v4 Network::getMyIP() const{
    return myIP;
}

void Network::connectionHandler(){
    tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), port));
    while(true)
    {
        tcpSocket_ptr clientSock(new tcp::socket(service));
        acceptor.accept(*clientSock);
        clientList_mtx.lock();
        for(auto& sock : *clientList)
        {
            try{
                if(std::get<2>(sock) == clientSock->remote_endpoint().address().to_v4()){
                    return; 
                } 
            } catch(...){
                std::cerr << "Could not reach remote endpoint in connectionHandler" << std::endl;
            }
        }
        connectedPeers_mtx.lock();
        try{
            address_v4 ip = clientSock->remote_endpoint().address().to_v4();
            clientList->emplace_back(std::make_tuple(clientSock, time(NULL), ip));
            connectedPeers.emplace_back(std::make_pair(ip,true));
        } catch(...){
            std::cerr << "Could not connect to socket in connectionHandler, remote endpoint" << std::endl;
        }
        connectedPeers_mtx.unlock();
        clientList_mtx.unlock();
    }
}

void Network::heartbeat(){
    while(true){
        if(!clientList->empty())
        {
            clientList_mtx.lock();
            for(auto& clientSock : *clientList)
            {
                double seconds = difftime(time(NULL), std::get<1>(clientSock));
                if(seconds >= heartbeatInterval)
                { 
                    std::get<0>(clientSock)->close();
                    address_v4 ip = std::get<2>(clientSock);
                    connectedPeers_mtx.lock();
                    connectedPeers.emplace_back(std::make_pair(ip,false));
                    connectedPeers_mtx.unlock();
                    clientList->remove(clientSock);
                    break;
                }
                else
                {
                    sendtoSocket(std::get<0>(clientSock), "syn");
                }
            }
            clientList_mtx.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::millisec(heartbeatInterval*1000/2));
    }
}

void Network::sendtoSocket(tcpSocket_ptr clientSock, const std::string &msg){
    char * data = new char[msg.size() + 1];
    std::copy(msg.begin(), msg.end(), data);
    data[msg.size()] = '\0';
    try{
        clientSock->write_some(buffer(data, strlen(data)));
    }
    catch(...){
        std::cerr << "Could not connect to socket in sendtoSocket" << std::endl;
    }
    delete[] data;
}

void Network::tcpMessageBroadcaster(){
    while(true)
    {
        if(!OutMessageQueue->empty())
        {
            clientList_mtx.lock();
            for(auto& clientSock : *clientList)
            {
                try
                {
                    sendtoSocket(std::get<0>(clientSock), OutMessageQueue->front());
                }
                catch(...){
                    std::cerr << "Could not send to socket in tcpMessageBroadcaster" << std::endl;
                }
            }
            clientList_mtx.unlock();
            OutMessageQueue_mtx.lock();
            OutMessageQueue->pop();
            OutMessageQueue_mtx.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

void Network::tcpMessageReceive(){
    while(true)
    {
        if(!clientList->empty())
        {
            clientList_mtx.lock();
            for(auto& clientSock : *clientList)
            {
                if(std::get<0>(clientSock)->available())
                {
                    try{
                        char readBuf[bufSize] = {0};
                        int bytesRead = std::get<0>(clientSock)->read_some(buffer(readBuf, bufSize));
                        string_ptr msg(new std::string(readBuf, bytesRead));
                        messageParser(clientSock, msg);
                    }
                    catch(...){
                        std::cerr << "Could not read from socket in recieve" << std::endl;
                    }
                }
            }
            clientList_mtx.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

void Network::messageParser(std::tuple< tcpSocket_ptr, time_t, address_v4 > &clientSock, string_ptr msg){
    boost::algorithm::trim(*msg);
    if(msg->find("syn") != std::string::npos){
        do {
            msg->erase(msg->find("syn"),3);
        } while(msg->find("syn") != std::string::npos);
        sendtoSocket(std::get<0>(clientSock), "ack");
    }
    if(msg->find("ack") != std::string::npos){
        std::get<1>(clientSock) = time(NULL);
        do {
            msg->erase(msg->find("ack"),3);
        } while(msg->find("ack") != std::string::npos);
    }
    if(!msg->empty()){
        innboundMessages_mtx.lock();
        try{
            address_v4 myIP = std::get<0>(clientSock)->remote_endpoint().address().to_v4();
            InnboundMessages.push_back(std::make_pair(myIP, *msg));
        } catch(...){
            std::cerr << "Could not reach remote endpoint in messageParser" << std::endl;
        }
        innboundMessages_mtx.unlock();
    }
}

void Network::send(const std::string &msg){
    OutMessageQueue_mtx.lock();
    OutMessageQueue->push(msg);
    OutMessageQueue_mtx.unlock();
}

std::vector< std::pair< address_v4, std::string> > Network::get_messages(){
    innboundMessages_mtx.lock();
    std::vector< std::pair< address_v4, std::string > > messages = InnboundMessages;
    InnboundMessages = {};
    innboundMessages_mtx.unlock();
    return messages;
}

std::vector< std::pair< address_v4, bool > >  Network::get_listofPeers(){
    connectedPeers_mtx.lock();
    std::vector< std::pair<address_v4, bool > >  peers = connectedPeers;
    connectedPeers_mtx.unlock();
    connectedPeers = {};
    return peers;    
}

void Network::udpBroadcaster(){
    io_service io_service;
    udpSocket_ptr socket(new udp::socket(io_service, udp::endpoint(udp::v4(), 0)));
    socket->set_option(socket_base::broadcast(true));
    udp::endpoint broadcast_endpoint(address_v4::broadcast(), 8888);
    std::string ip_string = myIP.to_string();
    char * data = new char[ip_string.size() + 1];
    copy(ip_string.begin(), ip_string.end(), data);
    data[ip_string.size()] = '\0';
    bool socketClosed = false;
    while(true){
        if(socketClosed){
            try{
	            socket.reset();
			    socket = udpSocket_ptr (new udp::socket(io_service, udp::endpoint(udp::v4(), 0)));
			    socket->set_option(socket_base::broadcast(true));
                socketClosed = false;
            } catch(...){ 
                std::cerr << "Could not open socket in udpBroadcaster" << std::endl;
            }
        }
        try{
            socket->send_to(buffer(data, strlen(data)), broadcast_endpoint);
        } catch(...){
            socket->close();
            socketClosed = true;
            std::cerr << "Could not connect to socket in udpBroadcaster" << std::endl;
        }
        boost::this_thread::sleep(boost::posix_time::millisec(udpBroadcastInterval*1000));
    }
}

void Network::udpListener(){
    io_service io_service;
    udpSocket_ptr recieveSocket(new udp::socket(io_service, udp::endpoint(udp::v4(), 8888)));
    udp::endpoint sender_endpoint;
    bool socketClosed = false;
    while(true)
    {
        if (socketClosed == true){
            try{
            	recieveSocket.reset();
			    recieveSocket = udpSocket_ptr (new udp::socket(io_service, udp::endpoint(udp::v4(), 8888)));
                socketClosed = false;
            }catch(...){
                std::cerr << "Could not open socket in udpBroadcaster" << std::endl;
            }
        }
        char data[bufSize] ={0};
        size_t bytes_transferred = recieveSocket->receive_from(buffer(data), sender_endpoint);
        string_ptr msg(new std::string(data, bytes_transferred));
        if(!msg->empty())
        {
            bool allreadyConnected = false;
            clientList_mtx.lock();
            for(auto& sock : *clientList)
            {
                if(std::get<2>(sock) == address_v4::from_string(*msg)) {
                    allreadyConnected = true; 
                }
            }
            if(!allreadyConnected && myIP != address_v4::from_string(*msg)){
                connectedPeers_mtx.lock();
                try
                {
                    tcp::endpoint ep(address::from_string(*msg), port);
                    tcpSocket_ptr sock(new tcp::socket(service));
                    sock->connect(ep);
                    clientList->emplace_back(std::make_tuple(sock, time(NULL), address_v4::from_string(*msg)));
                    connectedPeers.emplace_back(std::make_pair(address_v4::from_string(*msg),true));
                }
                catch(...){
                    recieveSocket->close();
                    socketClosed = true;
                    std::cerr << "Could not connect to socket in udpListener" << std::endl;
                }
                connectedPeers_mtx.unlock();
            }
            clientList_mtx.unlock();
        }
    }
    boost::this_thread::sleep(boost::posix_time::millisec(100));
}
