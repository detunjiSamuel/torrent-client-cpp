//
// Created by Adetunji on 24/01/2025.
//

#include "torrent.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "../bencode/bencode.h"
#include "../nlohmann/json.hpp"
#include "../sha1.h"


namespace Torrent {

using json = nlohmann::json;


namespace {

std::string read_file(const std::string& filePath) {

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

std::string apply_sha1_on_torrent_info(json &info_object) {
  // becode the info dict
  std::string bencoded_info = bencode::encode(info_object);
  // calc the sha-1 of the becoded info
  SHA1 checksum;
  checksum.update(bencoded_info);
  const std::string hash = checksum.final();
  return hash;
}

std::vector<std::string> covert_pieces_to_hex(std::string pieces) {
  std::vector<std::string> result;

  for (std::size_t i = 0; i < pieces.length(); i += 20) {
    std::string piece = pieces.substr(i, 20);
    std::stringstream str_stream;

    for (unsigned char byte : piece) {
      str_stream << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    result.push_back(str_stream.str());
  }
  return result;
}

}  // namespace

TorrentFile parse_torrent_file(std::string& filePath) {
  TorrentFile result;

  std::string encoded_content = read_file(filePath);

  // extract the info
  json decoded_content = bencode::decode(encoded_content);
  auto hash = apply_sha1_on_torrent_info(decoded_content["info"]);
  auto pieces = covert_pieces_to_hex(decoded_content["info"]["pieces"]);


  result.trackerUrl = decoded_content["announce"];
  result.length = decoded_content["info"]["length"];
  result.infoHash = hash;
  result.pieceLength =decoded_content["info"]["piece length"];
  result.pieces = pieces;

  return result;
}

}  // namespace Torrent