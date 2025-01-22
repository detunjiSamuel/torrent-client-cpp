//
// Created by Adetunji on 22/01/2025.
//

#ifndef TCPLIB_H
#define TCPLIB_H

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string>

class TcpConnection {
private:
  int sock;
  struct sockaddr_in server_address;
public:

  TcpConnection(const std::string& ip_address , int port);

  void sendData(const char* data, size_t size) const;

  std::vector<char> receiveData(size_t buffer_size = 68) const;

  void closeConnection() const;

};

#endif //TCPLIB_H
