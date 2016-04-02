#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <cstdlib>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "network.hpp"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> socket_ptr;
typedef boost::shared_ptr<string> string_ptr;
typedef boost::shared_ptr< list< pair< socket_ptr, time_t > > > clientList_ptr;
typedef boost::shared_ptr< queue<string> > messageQueue_ptr;

const int bufSize = 128; // Find max size of state data
const double heartbeat_time = 3; //Acceptable waiting time?

io_service service;
boost::mutex OutMessageQueue_mtx;
boost::mutex clientList_mtx;
clientList_ptr clientList(new list< pair< socket_ptr, time_t > >);
messageQueue_ptr OutMessageQueue(new queue<string>);


network::network(int port, string ip) : port(port), ip(ip)
{
    new boost::thread(bind(&network::connectionHandler, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(bind(&network::recieve, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(bind(&network::respond, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));        
    new boost::thread(bind(&network::udpBroadcaster, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(bind(&network::heartbeat, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));      
}

void network::connectionHandler(){
    //cout << "Waiting for peers..." << endl;
    tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), port));
    while(true)
    {
        socket_ptr clientSock(new tcp::socket(service));
        acceptor.accept(*clientSock);
        clientList_mtx.lock();
        for(auto& sock : *clientList)
        {
            try{
                if(sock.first->remote_endpoint().address().to_string() == clientSock->remote_endpoint().address().to_string())
                    return; 
            }
            catch(exception& e){}
        }
        clientList->emplace_back(make_pair(clientSock, time(NULL)));
        clientList_mtx.unlock();
        string s = clientSock->remote_endpoint().address().to_string();
        cout << s << " connected sucsessfully!" << endl;
    }
}

void network::heartbeat(){
    while(true){
        if(!clientList->empty())
        {
            clientList_mtx.lock();
            for(auto& clientSock : *clientList)
            {
                double seconds = difftime(time(NULL),clientSock.second);
                if(seconds >= heartbeat_time)
                { 
                    cout << "Client dissconected" << endl;
                    clientList->remove(clientSock);
                    break;
                }
                else
                {
                    char data[3];
                    string ack = "syn";
                    strcpy(data, ack.c_str());
                    try{
                        clientSock.first->write_some(buffer(data));
                    }
                    catch(exception& e){}
                }
            }
            clientList_mtx.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::millisec(heartbeat_time*1000/2));
    }
}

void network::respond(){
    while(true)
    {
        if(!OutMessageQueue->empty())
        {
            char data[bufSize] = {0};
            string message = OutMessageQueue->front();
            strcpy(data, message.c_str());
            clientList_mtx.lock();
            for(auto& clientSock : *clientList)
            {
                try
                {
                    clientSock.first->write_some(buffer(data));
                }
                catch(exception& e){}
            }
            clientList_mtx.unlock();
            OutMessageQueue_mtx.lock();
            OutMessageQueue->pop();
            OutMessageQueue_mtx.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

void network::recieve(){
    while(true)
    {
        if(!clientList->empty())
        {
            clientList_mtx.lock();
            for(auto& clientSock : *clientList)
            {
                if(clientSock.first->available())
                {
                    try{
                        char readBuf[bufSize] = {0};
                        int bytesRead = clientSock.first->read_some(buffer(readBuf, bufSize));
                        string_ptr msg(new string(readBuf, bytesRead));
                        if ((msg->find("syn") == string::npos) && (msg->find("ack") == string::npos))
                        {
                            if(msg->length() > 10){
                                InnboundMessages.push_back(*msg);
                            }
                        }
                        if(msg->find("syn") != string::npos)
                        {
                            cout << "syn received " << endl;

                            while(msg->find("syn") != string::npos){
                                cout << "syn removed" << endl;
                                msg->erase(msg->find("syn"),3);
                            }

                            char data[3];
                            string ack = "ack";
                            strcpy(data, ack.c_str());
                            try{
                                clientSock.first->write_some(buffer(data));
                            }
                            catch(exception& e){}
                            //Guard against concocted messages
                            if(msg->length() > 10){
                                cout << "syn parse" << endl;
                                InnboundMessages.push_back(*msg);
                            }
                        }
                        if(msg->find("ack") != string::npos)
                        {
                            cout << "ack received " << endl;
                            while(msg->find("ack") != string::npos){
                                cout << "ack removed" << endl;
                                msg->erase(msg->find("ack"),3);
                            }
                            clientSock.second = time(NULL);
                            if(msg->length() > 10){
                                cout << "ack parse" << endl;
                                InnboundMessages.push_back(msg->substr(2,msg->length()));
                            }
                        }
                    }
                    catch(exception& e){}
                }
            }
            clientList_mtx.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

void network::send(string msg){
    OutMessageQueue_mtx.lock();
    OutMessageQueue->push(msg);
    OutMessageQueue_mtx.unlock();
}

vector<string> network::get_messages(){
      vector<string> messages = InnboundMessages;
      InnboundMessages = {};
      return messages;
}

void network::udpBroadcaster(){
    //UDP broadcast, "Connect to me!" once on startup
    io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
    socket.set_option(socket_base::broadcast(true));
    udp::endpoint broadcast_endpoint(address_v4::broadcast(), 8888);
    char data[bufSize];
    strcpy(data, ip.c_str());
    socket.send_to(buffer(data), broadcast_endpoint);
    //Listen for incomming broadcast
    udp::socket recieveSocket(io_service, udp::endpoint(udp::v4(), 8888 ));
    udp::endpoint sender_endpoint;
    while(true)
    {
        char data[bufSize] ={0};
        size_t bytes_transferred = recieveSocket.receive_from(buffer(data), sender_endpoint);
        string_ptr msg(new string(data, bytes_transferred));
        bool allreadyConnected = false;
        if(!msg->empty())
        {
            for(auto& sock : *clientList)
            {
                try{
                    if(sock.first->remote_endpoint().address().to_string() == *msg)
                        allreadyConnected = true; 
                }
                catch(exception& e){}
            }
            if(!allreadyConnected){
                try
                {
                    tcp::endpoint ep(address::from_string(*msg), port);
                    socket_ptr sock(new tcp::socket(service));
                    sock->connect(ep);
                    clientList_mtx.lock();
                    clientList->emplace_back(make_pair(sock, time(NULL)));
                    clientList_mtx.unlock();
                    cout << "Broadcast recieved from: " << *msg <<" -> Connected!" << endl;
                }
                catch(exception& e){}
            }
        }
    }
}
