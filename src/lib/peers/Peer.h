//
// Created by Adetunji on 21/01/2025.
//

#ifndef PEER_H
#define PEER_H
#include <string>

class Peer {

  private:
  std::string ip_address;
  uint16_t port;

public:
  Peer(const std::string& ipAddress, uint16_t portNumber);

  explicit Peer(const std::array<unsigned char, 6>& char_bytes);

  [[nodiscard]] std::string getIp() const;
  [[nodiscard]] uint16_t getPort() const;

  [[nodiscard]] std::string toString() const;
};

#endif //PEER_H
