//
// Created by Adetunji on 20/01/2025.
//

#ifndef BENCODE_H
#define BENCODE_H

#include "../nlohmann/json.hpp"

namespace bencode {
using json = nlohmann::json;

json decode(const std::string& encoded_valuee);

std::string encode(json &decoded_json);
}  // namespace bencode

#endif  // BENCODE_H
