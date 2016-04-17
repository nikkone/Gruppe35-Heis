#pragma once
#include <tuple>
#include <ctime>
#include <string>
#include <vector>
#include <utility> 
#include <boost/asio.hpp>

using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> tcpSocket_ptr;
typedef boost::shared_ptr<std::string> string_ptr;

class Network {

  std::vector<std::pair<address_v4, std::string>> InnboundMessages;
  std::vector<std::pair<address_v4, bool>> connectedPeers;
  int port;
  address_v4 myIP; 

  private:
  	void connectionHandler();
    void heartbeat();
    void sendtoSocket(tcpSocket_ptr clientSock, const std::string &msg);
  	void tcpMessageBroadcaster();
   	void tcpMessageReceive();
    void messageParser(std::tuple<tcpSocket_ptr, time_t, address_v4> &clientSock, string_ptr msg);
    void udpBroadcaster();
    void udpListener();

  public:
    Network(int port, const address_v4 &myIP);
    const address_v4 getMyIP() const;
  	void send(const std::string &msg);
    std::vector<std::pair<address_v4, std::string>> get_messages();
    std::vector<std::pair<address_v4, bool>> get_listofPeers();
};	
