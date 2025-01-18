#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

std::tuple<std::string, json> decode_bencoded_value(const std::string &encoded_value)
{
    if (std::isdigit(encoded_value[0]))
    {
        // Example: "5:hello" -> "hello"
        size_t colon_index = encoded_value.find(':');
        if (colon_index != std::string::npos)
        {
            std::string number_string = encoded_value.substr(0, colon_index);
            int64_t number = std::atoll(number_string.c_str());
            std::string str = encoded_value.substr(colon_index + 1, number);

            return std::make_tuple(encoded_value.substr(colon_index + 1 + number), json(str));
        }
        else
        {
            throw std::runtime_error("Invalid encoded value: " + encoded_value);
        }
    }
    else if (encoded_value[0] == 'i' && encoded_value[encoded_value.size() - 1] == 'e')
    {
        // Example: "i42e" -> 42 , i-42e -> -4
        size_t e_index = encoded_value.find('e');
        if (e_index == std::string::npos)
        {
            throw std::runtime_error("Invalid encoded value: " + encoded_value);
        }
        std::string number_string = encoded_value.substr(1, e_index - 1);
        int64_t number = std::strtoll(number_string.c_str(), nullptr, 10);
        return std::make_tuple(encoded_value.substr(e_index + 1), json(number));
    }
    else if (encoded_value[0] == 'l' && encoded_value[encoded_value.size() - 1] == 'e')
    {
        json array = json::array();

        std::string rest = encoded_value.substr(1);

        while (rest[0] != 'e')
        {
            json value;
            std::tie(rest, value) = decode_bencoded_value(rest);
            array.push_back(value);
        }
        return std::make_tuple(rest.substr(1), array);
    }
    else if (encoded_value[0] == 'd' && encoded_value[encoded_value.size() - 1] == 'e')
    {
        json obj = json::object();
        std::string rest = encoded_value.substr(1);
        while (rest[0] != 'e')
        {
            json key, value;
            std::tie(rest, key) = decode_bencoded_value(rest);
            std::tie(rest , value) = decode_bencoded_value(rest);
            obj[key.get<std::string>()] = value;
        }
        return std::make_tuple(rest.substr(1), obj);
    }

    else
    {
        throw std::runtime_error("Unhandled encoded value: " + encoded_value);
    }
}

int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }
        // You can use print statements as follows for debugging, they'll be visible
        // when running tests.
        std::cerr << "Logs from your program will appear here!" << std::endl;

        // Uncomment this block to pass the first stage
        std::string encoded_value = argv[2];
        json decoded_value;
        std::string actual_string;
        std::tie(actual_string, decoded_value) = decode_bencoded_value(encoded_value);

        std::cout << actual_string << '\n';

        std::cout << decoded_value.dump() << std::endl;
    }
    else
    {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
