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
#include "lib/sha1.h"

using json = nlohmann::json;

std::string read_file(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::binary);

  if (!std::filesystem::exists(filePath)) {
    throw std::runtime_error("File does not exist:" + filePath);
  }

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file : " + filePath);
  }

  std::string encode_file_content((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());

  if (file.fail() && !file.eof()) {
    throw std::runtime_error("Failed to read the file :" + filePath);
  }

  return encode_file_content;
}

void parse_torrent(const std::string &file_path) {
  std::string encoded_content = read_file(file_path);
  // extract the info
  json decoded_content = bencode::decode(encoded_content);
  // becode the info dict
  std::string bencoded_info = bencode::encode(decoded_content["info"]);
  // calc the sha-1 of the becoded info
  SHA1 checksum;
  checksum.update(bencoded_info);
  const std::string hash = checksum.final();

  std::cout << "Tracker URL: " << decoded_content["announce"] << std::endl;
  std::cout << "Length: " << decoded_content["info"]["length"] << std::endl;
  std::cout << "Info SHA-1 Hash:" << hash << std::endl;

  std::cout << "Pieces : " << std::endl;

  std::string pieces = decoded_content["info"]["pieces"];

  for (std::size_t i = 0; i < pieces.length(); i += 20) {
    std::string piece = pieces.substr(i, 20);

    std::stringstream str_stream;

    for (unsigned char byte : piece) {
      str_stream << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << str_stream.str() << std::endl;
  }
}

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

    parse_torrent(filePath);
  } else if (command == "peers") {
    // TODO: update this to get data from parsed torrent instead of hard coding
    const std::string tracker =
        "http://bittorrent-test-tracker.codecrafters.io/announce";
    std::string hash = "d69f91e6b2ae4c542468d1073a71d4ea13879a7f";
    const std::string file_length = "92063";

    std::vector<Peer> tracker_peers =
        peers::get_pairs(tracker, hash, file_length);

    std::cout << "peers:" << std::endl;

    for (const auto &it : tracker_peers) {
      std::cout << it.toString() << std::endl;
      peers::handshake_pair(it, hash);
    }

  } else if (command == "handshake") {
    // move hand shake from above later
    return 1;
  } else {
    std::cerr << "unknown command: " << command << std::endl;
    return 1;
  }

  return 0;
}
