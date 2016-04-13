#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <cstdlib>
#include <ctime>
#include <tuple>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "network.hpp"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using boost::asio::ip::address_v4;

typedef boost::shared_ptr<tcp::socket> socket_ptr;
typedef boost::shared_ptr<string> string_ptr;
typedef boost::shared_ptr< list< tuple< socket_ptr, time_t, address_v4 > > > clientList_ptr;
typedef boost::shared_ptr< queue<string> > messageQueue_ptr;

const int bufSize = 128; 
const double heartbeat_time = 3; //Acceptable waiting time?

io_service service;
boost::mutex OutMessageQueue_mtx;
boost::mutex clientList_mtx;
boost::mutex innboundMessages_mtx;
boost::mutex connectedPeers_mtx;
clientList_ptr clientList(new list< tuple< socket_ptr, time_t, address_v4 > >);
messageQueue_ptr OutMessageQueue(new queue<string>);


/*

Skille recieve og send ack/syn
udpBrodcaster dele opp (udpListener)
kalle udpBroadcaster hvert nte sek
Sender emptyspace ved ack/syn fikse

*/


network::network(int port, address_v4 ip) : port(port), ip(ip)
{
    new boost::thread(bind(&network::connectionHandler, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(bind(&network::recieve, this));
    boost::this_thread::sleep( boost::posix_time::millisec(100));
    new boost::thread(bind(&network::tcpMessageBroadcaster, this));
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
            if(get<2>(sock) == clientSock->remote_endpoint().address().to_v4())
                return; 

        }
        address_v4 s = clientSock->remote_endpoint().address().to_v4();
        clientList->emplace_back(make_tuple(clientSock, time(NULL), s));
        clientList_mtx.unlock();
        connectedPeers_mtx.lock();
        connectedPeers.emplace_back(make_pair(s,true));
        connectedPeers_mtx.unlock();
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
                double seconds = difftime(time(NULL), get<1>(clientSock));
                if(seconds >= heartbeat_time)
                { 
                    cout << "Client dissconected" << endl;
                    address_v4 s = get<2>(clientSock);
                    connectedPeers_mtx.lock();
                    connectedPeers.emplace_back(make_pair(s,false));
                    connectedPeers_mtx.unlock();
                    clientList->remove(clientSock);
                    break;
                }
                else
                {
                    char data[3];
                    string syn = "syn";
                    strcpy(data,syn.c_str());
                    try{
                        get<0>(clientSock)->write_some(buffer(data));
                    }
                    catch(exception& e){}
                }
            }
            clientList_mtx.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::millisec(heartbeat_time*1000/2));
    }
}

void network::tcpMessageBroadcaster(){
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
                    get<0>(clientSock)->write_some(buffer(data));
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
                if(get<0>(clientSock)->available())
                {
                    try{
                        address_v4 ip = get<0>(clientSock)->remote_endpoint().address().to_v4();
                        char readBuf[bufSize] = {0};
                        int bytesRead = get<0>(clientSock)->read_some(buffer(readBuf, bufSize));
                        string_ptr msg(new string(readBuf, bytesRead));
                        if ((msg->find("syn") == string::npos) && (msg->find("ack") == string::npos))
                        {
                            innboundMessages_mtx.lock();
                            InnboundMessages.push_back(make_pair(ip, *msg));
                            innboundMessages_mtx.unlock();
                        } else {
                            if(msg->find("syn") != string::npos)
                            {
                                //cout << "syn received " << endl;

                                do {
                                    //cout << "syn removed" << endl;
                                    msg->erase(msg->find("syn"),3);
                                } while(msg->find("syn") != string::npos);

                                char data[3];
                                string ack = "ack";
                                strcpy(data,ack.c_str());
                                try{
                                    get<0>(clientSock)->write_some(buffer(data));
                                }
                                catch(exception& e){}
                                //Guard against concocted messages
                                    //cout << "syn parse" << endl;
                                innboundMessages_mtx.lock();
                                InnboundMessages.push_back(make_pair(ip, *msg));
                                innboundMessages_mtx.unlock();
                            }
                            if(msg->find("ack") != string::npos)
                            {
                                //cout << "ack received " << endl;
                                do {
                                    //cout << "ack removed" << endl;
                                    msg->erase(msg->find("ack"),3);
                                } while(msg->find("ack") != string::npos);
                                get<1>(clientSock) = time(NULL);
                                //cout << "ack parse" << endl;
                                innboundMessages_mtx.lock();
                                InnboundMessages.push_back(make_pair(ip, *msg));
                                innboundMessages_mtx.unlock();
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

vector<pair<address_v4, string>> network::get_messages(){
    innboundMessages_mtx.lock();
    vector<pair<address_v4, string>> messages = InnboundMessages;
    InnboundMessages = {};
    innboundMessages_mtx.unlock();
    return messages;
}

vector<pair<address_v4, bool>>  network::get_listofPeers(){
    connectedPeers_mtx.lock();
    vector<pair<address_v4, bool>>  peers = connectedPeers;
    connectedPeers_mtx.unlock();
    connectedPeers = {};
    return peers;    
}


void network::udpBroadcaster(){
    //UDP broadcast, "Connect to me!" once on startup
    io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
    socket.set_option(socket_base::broadcast(true));
    udp::endpoint broadcast_endpoint(address_v4::broadcast(), 8888);
    char data[bufSize];
    strcpy(data, (ip.to_string()).c_str());
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
                if(get<2>(sock) == address_v4::from_string(*msg)) 
                    allreadyConnected = true; 
            }
            if(!allreadyConnected){
                try
                {
                    tcp::endpoint ep(address::from_string(*msg), port);
                    socket_ptr sock(new tcp::socket(service));
                    sock->connect(ep);
                    clientList_mtx.lock();
                    clientList->emplace_back(make_tuple(sock, time(NULL), address_v4::from_string(*msg)));
                    clientList_mtx.unlock();
                    connectedPeers_mtx.lock();
                    connectedPeers.emplace_back(make_pair(address_v4::from_string(*msg),true));
                    connectedPeers_mtx.unlock();
                    cout << "Broadcast recieved from: " << *msg <<" -> Connected!" << endl;
                }
                catch(exception& e){}
            }
        }
    }
}
