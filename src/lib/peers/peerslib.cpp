//
// Created by Adetunji on 19/01/2025.
//

#include "peerslib.h"

#include "../bencode/bencode.h"
#include "../http/httplib.h"
#include "../nlohmann/json.hpp"
#include "Peer.h"
#include "sstream"

namespace peers {

using json = nlohmann::json;

namespace {

std::vector<Peer> decode_pairs(std::string &encoded_peers) {
  // each pair is made up of 6 bytes
  // the first 4 bytes are Ip- address
  // the last 2 bytes are the port numbers
  std::vector<Peer> result;
  for (size_t i = 0; i < encoded_peers.size(); i += 6) {
    // convert byte array
    std::array<unsigned char, 6> peer_byte_array;
    std::copy(encoded_peers.begin() + i, encoded_peers.begin() + i + 6,
              peer_byte_array.begin());

    result.push_back(Peer(peer_byte_array));
  }
  return result;
}

std::array<char, 68> get_peer_handshake_message(std::string &info_hash) {
  /**
   *Handshake message contains
   *  length of the protocol string (BitTorrent protocol) which is 19 (1 byte)
   *  the string BitTorrent protocol (19 bytes)
   *  eight reserved bytes, which are all set to zero (8 bytes)
   *  sha1 infohash (20 bytes) - that's the info_hash passed
   *  peer id (20 bytes) - this random generated
   */

  constexpr size_t MSG_SIZE = 68;

  std::array<char, MSG_SIZE> message;

  message[0] = 19;  // 1 byte

  const char *protocol = "BitTorrent protocol";  // 19 bytes
  std::copy_n(protocol, 19, message.begin() + 1);

  std::fill_n(message.begin() + 20, 8, '\0');  // 8 bytes

  std::copy_n(info_hash.data(), 20, message.begin() + 28);  // 20 bytes

  for (size_t i = 0; i < 20; i++) {
    message[i + 48] = static_cast<uint8_t>(rand());
  }
  return message;
}

std::string url_encode(const std::string &hash) {
  // hash is a 40 byte long
  // hex representation we need is 20 byte
  // we know that a pair of 2 characters represents 1 byte
  std::stringstream result;

  for (size_t i = 0; i < hash.size(); i += 2) {
    result << '%' << hash.substr(i, 2);
  }
  return result.str();
}

std::string hex_encode_info_hash(const std::string &hash) {
  // hash is a 40 byte long
  // hex representation we need is 20 byte
  // we know that a pair of 2 characters represents 1 byte
  std::string binary;
  binary.reserve(hash.length() / 2);

  for (size_t i = 0; i < hash.length(); i += 2) {
    std::string byte = hash.substr(i, 2);
    char chr = static_cast<char>(std::stoi(byte, nullptr, 16));
    binary.push_back(chr);
  }
  return binary;
}

std::tuple<std::string, std::string> split_endpoint_url(
    const std::string &tracker_url) {
  // returns tuple with base url and route
  const size_t idx = tracker_url.find_last_of('/');
  return std::make_tuple(tracker_url.substr(0, idx),
                         tracker_url.substr(idx, tracker_url.size() - idx));
}

void wait_for_bitfield(const TcpConnection &conn) {
  while (true) {
    std::cout << "Awaiting Bitfield..." << std::endl;
    auto buffer = conn.receiveData();
    if (buffer.size() > 0) {
      std::stringstream ss;

      for (auto &it : buffer) ss << static_cast<int>(it);

      std::cout << ss.str() << std::endl;
      break;
    }
  }
}

void send_interested(const TcpConnection &conn) {
}

}  // namespace

std::vector<Peer> get_pairs(TorrentFile torrentFile) {
  std::vector<std::string> result;
  auto [base_url, route] = split_endpoint_url(torrentFile.trackerUrl);

  httplib::Params params{
      {"peer_id", "k4t9b0d7w2r5y8v1m6qz"},  // just random key of len 20
      {"port", "6881"},
      {"uploaded", "0"},
      {"downloaded", "0"},
      {"compact", "1"}};

  params.emplace("left", std::to_string(torrentFile.length));

  httplib::Headers headers{};
  httplib::Client cli(base_url);

  auto res = cli.Get(route + "?info_hash=" + url_encode(torrentFile.infoHash),
                     params, headers);

  std::cout << "Response status:" << res->status << std::endl;

  if (res->status != httplib::StatusCode::OK_200)
    throw std::runtime_error("HTTP error:" + to_string(res.error()));

  std::string response_body = res->body;

  // response body  is benconded dictionary
  json decoded_body = bencode::decode(response_body);

  std::string received_peers = decoded_body["peers"];

  std::vector<Peer> peers = decode_pairs(received_peers);

  return peers;
}
TcpConnection handshake_pair(Peer peer, std::string &info_hash,
                             bool should_close_connection) {
  std::string encoded_hash = hex_encode_info_hash(info_hash);

  std::array<char, 68> handshake_message =
      get_peer_handshake_message(encoded_hash);

  // create Tcp connection
  TcpConnection conn = TcpConnection(peer.getIp(), peer.getPort());

  // send bytes
  conn.sendData(handshake_message.data(), 68);

  // receive result
  auto buffer = conn.receiveData();

  // print hex_representation of peer_id
  std::stringstream ss;

  ss << std::hex;
  for (size_t i = 0; i < 20; ++i)
    ss << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[48 + i]);

  std::cout << ss.str() << std::endl;

  if (should_close_connection) conn.closeConnection();

  return conn;
}

void download_piece(Peer peer, TorrentFile torrent) {
  // continue from handshake
  TcpConnection conn = handshake_pair(peer, torrent.infoHash, false);

  // peermessage : <message_length>(4)<message_id>(1)<payload>()

  // wait for bitfield message - id= 5
  // indicates which pieces it has
  wait_for_bitfield(conn);

  // send "interested" message
  // message id= 2
  // empty payload
  send_interested(conn);

  // wait until receive unchoke message -id= 1
  // empty payload
  // wait_for_unchoke(conn);

  // break pieces into 16 kiB (16 * 1024 bytes) blocks
  // and send request message for each id=6
  // payload should contain : index( index of piece), begin(index offset in
  // actual byte of piece)
  // and lenght of block in bytes

  // wait for a piece message for each block
  // message id=7
  // payload contains : index , begin , block(data of the piece)

  // check integretity by comparing its hash with the hash value of piece in
}

}  // namespace peers