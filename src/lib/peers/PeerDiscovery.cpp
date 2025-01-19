//
// Created by Adetunji on 19/01/2025.
//

#include "PeerDiscovery.h"

#include "../http/httplib.h"
#include "sstream";

namespace peers {
std::string encode_info_hash(const std::string &hash) {
  // hash is a 40 byte long
  // hex representation we need is 20 byte
  // we know that a pair of 2 characters represents 1 byte
  std::stringstream result;

  for (size_t i = 0; i < hash.size(); i += 2) {
    result << '%' << hash.substr(i, 2);
  }
  return result.str();
}

std::tuple<std::string, std::string> split_endpoint_url(
    const std::string &tracker_url) {
  // returns tuple with base url and route
  const size_t idx = tracker_url.find_last_of('/');
  return std::make_tuple(tracker_url.substr(0, idx),
                         tracker_url.substr(idx, tracker_url.size() - idx));
}

std::vector<std::string> get_pairs(const std::string &tracker_url,
                                   const std::string &info_hash,
                                   const std::string &length) {

  std::vector<std::string> result;
  auto [base_url, route] = split_endpoint_url(tracker_url);

  httplib::Params params{{"peer_id", "k4t9b0d7w2r5y8v1m6qz"},//just random key of len 20
                         {"port", "6881"},
                         {"uploaded", "0"},
                         {"downloaded", "0"},
                         {"compact", "1"}};

  params.emplace("left", length);

  httplib::Headers headers{};

  httplib::Client cli(base_url);

  auto res = cli.Get(route + "?info_hash=" + encode_info_hash(info_hash), params, headers);

  std::cout << "Response status:" << res->status << std::endl;
  std::cout << "Response body:" << res->body << std::endl;

  return result;

}
}  // namespace peers