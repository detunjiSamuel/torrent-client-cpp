//
// Created by Adetunji on 24/01/2025.
//

#ifndef TORRENT_H
#define TORRENT_H
#include <iostream>
#include <string>
#include <vector>

struct TorrentFile {
  std::string trackerUrl;
  size_t length;
  std::string name;
  size_t pieceLength;
  std::vector<std::string> pieces;  // piece hash
  std::string infoHash;             // sha1 of becoded Info in torrent file

  void printInfo() const {
    std::cout << "tracker url:" << trackerUrl << std::endl;
    std::cout << "Length:" << length << std::endl;
    std::cout << "Info Sha-1 Hash:" << infoHash << std::endl;
    std::cout << "Piece length:" << pieceLength << std::endl;
    std::cout << "Piece Hashes:" << std::endl;
    for (auto it : pieces) {
      std::cout << '\t' << it << std::endl;
    }
  }
};

namespace Torrent {

TorrentFile parse_torrent_file(std::string &filePath);

}

#endif  // TORRENT_H
