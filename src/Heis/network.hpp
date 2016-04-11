#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility> 

class network {

  std::vector<std::pair<std::string, std::string>> InnboundMessages;
  std::vector<std::pair<std::string, bool>> connectedPeers;
  int port;
  std::string ip; 

  private:
  	void connectionHandler();
    void heartbeat();
  	void respond();
   	void recieve();
    void udpBroadcaster();

  public:
  	network(int port, std::string ip);
  	void send(std::string msg);
    std::vector<std::pair<std::string, std::string>> get_messages();
    std::vector<std::pair<std::string, bool>> get_listofPeers();
};	
