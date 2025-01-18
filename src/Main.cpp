#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;


std::vector<std::string> get_values_as_string(const std::string& encoded_value) {

    std::vector<std::string> result;

    size_t left = 0;
    size_t right = 1;

    bool looking_for_int = encoded_value[0] == 'i';

    while (right < encoded_value.size()) {
        if (!looking_for_int) {
            // Look for the ':' delimiter
            while (left < encoded_value.size() && encoded_value[left] != ':') {
                left += 1;
            }

            if (left < encoded_value.size() && encoded_value[left] == ':') {
                size_t length = std::stoul(encoded_value.substr(0, left));
                if (left + 1 + length <= encoded_value.size()) {
                    result.push_back(encoded_value.substr(left + 1, length));
                    left = left + 1 + length;
                    right = left + 1;
                    if (left < encoded_value.size()) {
                        looking_for_int = encoded_value[left] == 'i';
                    }
                    continue;
                }
            }
        } else {
            // Look for the 'e' delimiter
            if (encoded_value[right] == 'e') {
                std::string num_str = encoded_value.substr(left + 1, right - left - 1);
                result.push_back(num_str);

                left = right + 1;
                if (left < encoded_value.size()) {
                    looking_for_int = encoded_value[left] == 'i';
                }
            }
        }
        right++;
    }

    return result;
}


json decode_bencoded_value(const std::string& encoded_value) {
    if (std::isdigit(encoded_value[0])) {
        // Example: "5:hello" -> "hello"
        size_t colon_index = encoded_value.find(':');
        if (colon_index != std::string::npos) {
            std::string number_string = encoded_value.substr(0, colon_index);
            int64_t number = std::atoll(number_string.c_str());
            std::string str = encoded_value.substr(colon_index + 1, number);
            return json(str);
        } else {
            throw std::runtime_error("Invalid encoded value: " + encoded_value);
        }
    } else if ( encoded_value[0] == 'i' && encoded_value[encoded_value.size() - 1] == 'e') {
        // Example: "i42e" -> 42 , i-42e -> -4
        std::string number_string =  encoded_value.substr(1 ,  encoded_value.size() - 2);
        int64_t number = std::atoll(number_string.c_str());
        return json(number);

    }
    else if ( encoded_value[0] == 'l' && encoded_value[encoded_value.size() - 1] == 'e') {
        std::string list_string = encoded_value.substr(1 ,  encoded_value.size() - 2);
        std::vector<std::string> values = get_values_as_string(list_string);

        json array = json::array();

        for (size_t i = 0; i < values.size(); i++) {
            if (std::isdigit(values[i][0]) || values[i][0] == '-') {
                array.push_back(std::atoll(values[i].c_str()));
            } else {
                array.push_back(values[i]);
            }
        }
        return array;

    }
    else {
        throw std::runtime_error("Unhandled encoded value: " + encoded_value);
    }
}

int main(int argc, char* argv[]) {
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
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }
        // You can use print statements as follows for debugging, they'll be visible when running tests.
        std::cerr << "Logs from your program will appear here!" << std::endl;

        // Uncomment this block to pass the first stage
        std::string encoded_value = argv[2];
        json decoded_value = decode_bencoded_value(encoded_value);
        std::cout << decoded_value.dump() << std::endl;
    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
