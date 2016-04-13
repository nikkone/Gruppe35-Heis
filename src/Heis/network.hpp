#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility> 


#include <boost/asio/ip/address_v4.hpp>
using boost::asio::ip::address_v4;
class network {

  std::vector<std::pair<address_v4, std::string>> InnboundMessages;
  std::vector<std::pair<address_v4, bool>> connectedPeers;
  int port;
  address_v4 ip; 

  private:
  	void connectionHandler();
    void heartbeat();
  	void tcpMessageBroadcaster();
   	void recieve();
    void udpBroadcaster();

  public:
  	network(int port, address_v4 ip);
  	void send(std::string msg);
    std::vector<std::pair<address_v4, std::string>> get_messages();
    std::vector<std::pair<address_v4, bool>> get_listofPeers();
};	
