//
// Created by Adetunji on 22/01/2025.
//

#include "tcplib.h"

#include <unistd.h>

#include <iostream>
#include <vector>

TcpConnection::TcpConnection(const std::string &ip_address, int port) {
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) throw std::runtime_error("Failed to create socket");

  // clear unintialized memory or garbage by setting to zero
  memset(&server_address, 0, sizeof(server_address));

  server_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  if (connect(sock, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    close(sock);
    throw std::runtime_error("Cannot connect to server");
  }
}

void TcpConnection::sendData(const char *data, size_t size) const {
  if (send(sock, data, size, 0) < 0) {
    throw std::runtime_error("Failed to send data");
  }
}

std::vector<char> TcpConnection::receiveData(const size_t buffer_size) const {

  std::vector<char> buffer(buffer_size, 0);

  if ( recv(sock, buffer.data(), buffer_size, 0) < 1 )
    throw std::runtime_error("Failed to receive data");

  return buffer;
}

void TcpConnection::closeConnection() const { close(sock); }
