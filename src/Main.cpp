#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "lib/nlohmann/json.hpp"
#include "lib/peers/PeerDiscovery.h"
#include "lib/sha1.h"

using json = nlohmann::json;

json decode_bencoded_value(const std::string &encoded_value, int &id);

json decode_int(const std::string encoded_value,
                int &id) {  //  i-14e -> -14 , i52e --> 52
  id++;
  std::string result;
  while (encoded_value[id] != 'e') {
    result += encoded_value[id];
    id++;
  }
  id++;  // move from e
  return json(std::atoll(result.c_str()));
}

json decode_dict(const std::string &encoded_value, int &id) {
  id++;
  json result = json::object();

  while (encoded_value[id] != 'e') {
    json key = decode_bencoded_value(encoded_value, id);
    // id is a reference , it will be updated with calls
    json value = decode_bencoded_value(encoded_value, id);
    result[key.get<std::string>()] = value;
  }
  id++;
  return result;
}

json decode_list(const std::string &encoded_value, int &id) {
  id++;
  json result = json::array();
  while (encoded_value[id] != 'e') {
    result.push_back(decode_bencoded_value(encoded_value, id));
  }
  id++;
  return result;
}

json decode_str(const std::string &encoded_value, int &id) {
  std::string result;
  while (isdigit(encoded_value[id])) {
    result += encoded_value[id];
    id++;
  }
  int length = std::atoll(result.c_str());
  result = "";
  id++;
  while (length--) {
    result += encoded_value[id];
    id++;
  }
  return result;
}

json decode_bencoded_value(const std::string &encoded_value, int &id) {
  char first_char = encoded_value[id];

  switch (first_char) {
    case 'i':
      return decode_int(encoded_value, id);
    case 'd':
      return decode_dict(encoded_value, id);
    case 'l':
      return decode_list(encoded_value, id);
    default:
      if (isdigit(first_char)) {
        return decode_str(encoded_value, id);
      } else {
        throw std::runtime_error("Invalid encoded value: " + encoded_value +
                                 " at index: " + std::to_string(id));
      }
  }
}

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

std::string json_to_bencode(json &decoded_json) {
  // the json is is still a reference but i will not edit it
  std::string result;

  if (decoded_json.is_array()) {
    result += 'l';
    for (const json &item : decoded_json) {
      result += item;
    }
    result += 'e';
  } else if (decoded_json.is_object()) {
    result += 'd';
    for (auto &el : decoded_json.items()) {
      result += (std::to_string(el.key().size()) + ':' + el.key() +
                 json_to_bencode(el.value()));
    }
    result += 'e';
  }

  else if (decoded_json.is_number_integer()) {
    result += ('i' + std::to_string(decoded_json.get<int>()) + 'e');
  } else if (decoded_json.is_string()) {
    const std::string &value = decoded_json.get<std::string>();
    result += (std::to_string(value.size()) + ':' + value);
  }
  return result;
}

void parse_torrent(const std::string &file_path) {
  int id = 0;

  std::string encoded_content = read_file(file_path);
  // extract the info
  json decoded_content = decode_bencoded_value(encoded_content, id);
  // becode the info dict
  std::string bencoded_info = json_to_bencode(decoded_content["info"]);
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

    // Uncomment this block to pass the first stage
    std::string encoded_value = argv[2];
    json decoded_value;
    int id = 0;

    decoded_value = decode_bencoded_value(encoded_value, id);

    std::cout << decoded_value.dump() << std::endl;
  } else if (command == "info") {
    std::string filePath = argv[2];

    parse_torrent(filePath);
  } else if (command == "peers") {
    // TODO: update this to get data from parsed torrent instead of hard coding
    const std::string tracker =
        "http://bittorrent-test-tracker.codecrafters.io/announce";
    const std::string hash = "d69f91e6b2ae4c542468d1073a71d4ea13879a7f";
    const std::string file_length = "92063";

    std::vector<Peer> tracker_peers =
        peers::get_pairs(tracker, hash, file_length);

    std::cout << "peers:" << std::endl;

    for (const auto &it : tracker_peers) {
      std::cout << it.toString() << std::endl;
    }

  } else {
    std::cerr << "unknown command: " << command << std::endl;
    return 1;
  }

  return 0;
}
