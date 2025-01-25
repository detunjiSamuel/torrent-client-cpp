#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "lib/bencode/bencode.h"
#include "lib/nlohmann/json.hpp"
#include "lib/peers/peerslib.h"
#include "lib/torrent/torrent.h"

using json = nlohmann::json;


int main(int argc, char *argv[]) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
    return 1;
  }

  std::string command = argv[1];

  if (command == "decode") {
    if (argc < 3) {
      std::cerr << "Usage: " << argv[0] << " decode <encoded_value>"
                << std::endl;
      return 1;
    }
    // You can use print statements as follows for debugging, they'll be visible
    // when running tests.
    std::cerr << "Logs from your program will appear here!" << std::endl;

    std::string encoded_value = argv[2];
    json decoded_value = bencode::decode(encoded_value);

    std::cout << decoded_value.dump() << std::endl;

  } else if (command == "info") {
    std::string filePath = argv[2];

    TorrentFile torrentFile = Torrent::parse_torrent_file(filePath);
    torrentFile.printInfo();

  } else if (command == "peers") {
    std::string filePath = argv[2];

    TorrentFile torrentFile = Torrent::parse_torrent_file(filePath);

    std::vector<Peer> tracker_peers =
        peers::get_pairs(torrentFile);
    std::cout << "peers:" << std::endl;

    for (const auto &it : tracker_peers) {
      std::cout << it.toString() << std::endl;
      peers::handshake_pair(
        it,
        torrentFile.infoHash,
        true
        );

      break;//TODO: remove this break -> left it here for testing
    }

    peers::download_piece(tracker_peers[0] , torrentFile);

  } else if (command == "handshake") {
    // move hand shake from above later
    return 1;
  } else {
    std::cerr << "unknown command: " << command << std::endl;
    return 1;
  }

  return 0;
}
