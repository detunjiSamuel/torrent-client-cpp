//
// Created by Adetunji on 20/01/2025.
//

#include "bencode.h"

namespace bencode {

namespace {
json decode_bencoded_value(const std::string& encoded_value, int& id);

json decode_int(const std::string& encoded_value, int& id) {
  id++;
  std::string result;
  while (encoded_value[id] != 'e') {
    result += encoded_value[id];
    id++;
  }
  id++;  // move from e
  return json(std::atoll(result.c_str()));
}

json decode_dict(const std::string& encoded_value, int& id) {
  id++;
  json result = json::object();

  while (encoded_value[id] != 'e') {
    json key = decode_bencoded_value(encoded_value, id);
    json value = decode_bencoded_value(encoded_value, id);
    result[key.get<std::string>()] = value;
  }
  id++;
  return result;
}

json decode_list(const std::string& encoded_value, int& id) {
  id++;
  json result = json::array();
  while (encoded_value[id] != 'e') {
    result.push_back(decode_bencoded_value(encoded_value, id));
  }
  id++;
  return result;
}

json decode_str(const std::string& encoded_value, int& id) {
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

json decode_bencoded_value(const std::string& encoded_value, int& id) {
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
      }
      throw std::runtime_error("Invalid encoded value: " + encoded_value +
                               " at index: " + std::to_string(id));
  }
}
}  // namespace

json decode(const std::string& encoded_value) {
  int id = 0;
  return decode_bencoded_value(encoded_value, id);
}

std::string encode(json& decoded_json) {
  std::string result;

  if (decoded_json.is_array()) {
    result += 'l';
    for (const json& item : decoded_json) {
      result += item;
    }
    result += 'e';
  } else if (decoded_json.is_object()) {
    result += 'd';
    for (auto& el : decoded_json.items()) {
      result += (std::to_string(el.key().size()) + ':' + el.key() +
                 encode(el.value()));
    }
    result += 'e';
  } else if (decoded_json.is_number_integer()) {
    result += ('i' + std::to_string(decoded_json.get<int>()) + 'e');
  } else if (decoded_json.is_string()) {
    const std::string& value = decoded_json.get<std::string>();
    result += (std::to_string(value.size()) + ':' + value);
  }
  return result;
}
}  // namespace bencode