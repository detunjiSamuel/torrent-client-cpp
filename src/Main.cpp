#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

json decode_bencoded_value(const std::string &encoded_value, int &id);

json decode_int(const std::string encoded_value , int &id)
{   //  i-14e -> -14 , i52e --> 52
    id ++;
    std::string result;
    while (encoded_value[id] != 'e')
    {
        result += encoded_value[id];
        id ++;
    }
    id++; // move from e
    return json(std::atoll(result.c_str()));
}

json decode_dict(const std::string &encoded_value, int &id)
{
    id++;
    json result = json::object();

    while ( encoded_value[id] != 'e')
    {
        json key = decode_bencoded_value(encoded_value ,  id );
        // id is a reference , it will be updated with calls
        json value = decode_bencoded_value(encoded_value , id);
        result[key.get<std::string>()] = value;
    }
    id ++;
    return result;
}

json decode_list( const std::string &encoded_value, int &id )
{
    id++;
    json result = json::array();
    while (encoded_value[id] != 'e')
    {
        result.push_back(decode_bencoded_value(encoded_value, id));
    }
    id++;
    return result;
}

json decode_str(const std::string &encoded_value, int &id )
{
    std::string result;
    while (isdigit(encoded_value[id]))
    {
        result += encoded_value[id];
        id++;
    }
    int length = std::atoll(result.c_str());
    result = "";
    id++;
    while (length--)
    {
        result += encoded_value[id];
        id++;
    }
    return result;
}

json decode_bencoded_value(const std::string &encoded_value, int &id)
{

    if (encoded_value[id] == 'i')
    {
        return decode_int(encoded_value, id);
    }
    else if (encoded_value[id] == 'd')
    {
        return decode_dict(encoded_value, id);
    }
    else if (encoded_value[id] == 'l')
    {
        return decode_list(encoded_value, id);
    }
    else if (isdigit(encoded_value[id]))
    {
        return decode_str(encoded_value, id);
    }
    else
    {
        throw std::runtime_error("Invalid encoded value: " + encoded_value + " at index: " + std::to_string(id));
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
        int id = 0;

        decoded_value = decode_bencoded_value(encoded_value , id );

        std::cout << decoded_value.dump() << std::endl;
    }
    else if (command == "info")
    {
        std::string filePath = argv[2];
        std::ifstream file(filePath, std::ios::binary);

        if (!std::filesystem::exists(filePath)) {
            std::cerr << "File does not exist: " << filePath << '\n';
            return 1;
        }


        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filePath << '\n';
            return 1;
        }

        std::string encode_file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        if (file.fail() && !file.eof()) {
            std::cerr << "Error: Failed to read the file " << filePath << '\n';
            return 1;
        }

        int id = 0;

        json decoded_value = decode_bencoded_value(encode_file_content , id);
        std::cout << "Tracker URL: " << decoded_value["announce"].get<std::string>() << std::endl;
        std::cout << "Length: " << decoded_value["info"]["length"].get<int>() << std::endl;
    }
    else
    {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
