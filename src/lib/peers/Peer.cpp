//
// Created by Adetunji on 21/01/2025.
//

#include "Peer.h"

#include <array>
#include <sstream>

Peer::Peer(const std::string& ipAddress, uint16_t portNumber)
    : ip_address(ipAddress), port(portNumber) {}


 Peer::Peer(const std::array<unsigned char, 6>& char_bytes) {

  // the first 4 bytes are Ip- address
  // the last 2 bytes are the port numbers

  ip_address = std::to_string(char_bytes[0]) + "." +
         std::to_string(char_bytes[1]) + "." +
         std::to_string(char_bytes[2]) + "." +
         std::to_string(char_bytes[3]);

  // 2 bytes == 8 bits
  port = char_bytes[4] << 8 | char_bytes[5];
}


std::string Peer::getIp() const {
  return ip_address;
}

uint16_t Peer::getPort() const {
  return port;
}

std::string Peer::toString() const {
  std::ostringstream oss;
  oss << ip_address << ":" << port;
  return oss.str();
}