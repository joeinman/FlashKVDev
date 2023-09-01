// Includes
#include <FlashKV/FlashKV.h>
#include <optional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>

void printHelp()
{
    std::cout << "[INFO] Available Commands:" << std::endl
              << "[INFO] read <key> - Reads A Key-Value Pair From The Store" << std::endl
              << "[INFO] readall - Reads All Key-Value Pairs In The Store" << std::endl
              << "[INFO] write <key> <value> - Writes A Key-Value Pair To The Store, Where The Value Should Be A Sequence Of Bytes In Hexadecimal Format" << std::endl
              << "[INFO] erase <key> - Erases A Key-Value Pair From The Store" << std::endl
              << "[INFO] eraseall - Erases All Key-Value Pairs From The Store" << std::endl
              << "[INFO] save - Saves The Store To Flash Memory" << std::endl
              << "[INFO] exit - Exits The Program" << std::endl;
}

int main(int argc, char *argv[])
{
    // Check If The FlashKV File Path Was Provided
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <FlashKV File>" << std::endl;
        return 1;
    }

    // File Path For The FlashKV Store
    std::string flashKVFilePath = argv[1];

    // Create a FlashKV object.
    FlashKV::FlashKV flashKV(
        [flashKVFilePath](uint32_t flashAddress, const uint8_t *data, size_t count) -> bool
        {
            std::fstream file(flashKVFilePath, std::ios::binary | std::ios::out | std::ios::in);
            if (!file)
            {
                file.open(flashKVFilePath, std::ios::binary | std::ios::out);
                if (!file)
                    return false;
            }
            file.seekp(flashAddress, std::ios::beg);
            file.write(reinterpret_cast<const char *>(data), count);
            return true;
        },
        [flashKVFilePath](uint32_t flashAddress, uint8_t *data, size_t count) -> bool
        {
            std::fstream file(flashKVFilePath, std::ios::binary | std::ios::in);
            if (!file)
            {
                file.open(flashKVFilePath, std::ios::binary | std::ios::out);
                if (!file)
                    return false;
            }
            file.seekg(flashAddress, std::ios::beg);
            file.read(reinterpret_cast<char *>(data), count);
            return true;
        },
        [flashKVFilePath](uint32_t flashAddress, size_t count) -> bool
        {
            // In a file-based implementation, erasing is equivalent to writing zeros.
            std::vector<uint8_t> zeros(count, 0);
            std::fstream file(flashKVFilePath, std::ios::binary | std::ios::out | std::ios::in);
            if (!file)
            {
                file.open(flashKVFilePath, std::ios::binary | std::ios::out);
                if (!file)
                    return false;
            }
            file.seekp(flashAddress, std::ios::beg);
            file.write(reinterpret_cast<const char *>(zeros.data()), count);
            return true;
        },
        512,
        4096,
        0,
        8192);

    // Load The Map From File
    int loadStatus = flashKV.loadMap();
    if (loadStatus == 1)
        std::cout << "[INFO] FlashKV Map Loaded Successfully" << std::endl;
    else if (loadStatus == 2)
        std::cout << "[INFO] No FlashKV Map Found. A New One Will Be Created Upon Saving" << std::endl;
    else if (loadStatus == 0)
    {
        std::cout << "[ERROR] Error Occurred While Loading The FlashKV Map" << std::endl;
        return 1;
    }

    std::string command;
    while (1)
    {
        std::cout << "> ";
        std::getline(std::cin, command);

        std::istringstream iss(command);
        std::string op;
        iss >> op;

        if (op == "read")
        {
            std::string key;
            while (iss >> key)
            {
                std::optional<std::vector<uint8_t>> value = flashKV.readKey(key);
                if (value)
                {
                    std::cout << "[INFO] Read Key: \"" << key << "\", Value: [";
                    for (auto it = value->begin(); it != value->end(); ++it)
                    {
                        if (it != value->begin())
                            std::cout << " ";
                        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)*it;
                    }
                    std::cout << "]" << std::endl;
                }
                else
                {
                    std::cout << "[ERROR] Key \"" << key << "\" Not Found" << std::endl;
                }
            }
        }
        else if (op == "readall")
        {
            auto allKeys = flashKV.getAllKeys();
            std::cout << "[INFO] Found " << allKeys.size() << (allKeys.size() == 1 ? " Key" : " Keys") << std::endl;

            for (auto key : allKeys)
            {
                std::cout << "[INFO] Key: \"" << key << "\", Value: [";
                std::optional<std::vector<uint8_t>> value = flashKV.readKey(key);
                for (auto it = value->begin(); it != value->end(); ++it)
                {
                    if (it != value->begin())
                        std::cout << " ";
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)*it;
                }
                std::cout << "]" << std::endl;
            }
        }
        else if (op == "write")
        {
            std::string key;
            if (!(iss >> key))
            {
                std::cout << "[Error] No Key Provided\n";
                continue;
            }

            std::vector<uint8_t> value;
            std::string byteStr;
            bool invalidByteFound = false;
            while (iss >> byteStr)
            {
                if (byteStr.size() != 2)
                {
                    std::cout << "[ERROR] Invalid Byte: " << byteStr << std::endl;
                    invalidByteFound = true;
                    break;
                }
                unsigned int byte = std::stoul(byteStr, nullptr, 16);
                value.push_back(static_cast<uint8_t>(byte));
            }

            if (invalidByteFound)
            {
                std::cout << "[ERROR] Invalid Byte Encountered, Key \"" << key << "\" Was Not Written" << std::endl;
            }
            else
            {
                if (flashKV.writeKey(key, value))
                    std::cout << "[INFO] Key \"" << key << "\" Written Successfully" << std::endl;
                else
                    std::cout << "[ERROR] Unable To Write Key \"" << key << "\"" << std::endl;
            }
        }
        else if (op == "erase")
        {
            std::string key;
            while (iss >> key)
            {
                if (flashKV.eraseKey(key))
                    std::cout << "[INFO] Key \"" << key << "\" Erased Successfully" << std::endl;
                else
                    std::cout << "[Error] Key \"" << key << "\" Doesn't Exist" << std::endl;
            }
        }
        else if (op == "eraseall")
        {
            auto allKeys = flashKV.getAllKeys();
            for (auto key : allKeys)
            {
                if (flashKV.eraseKey(key))
                    std::cout << "[INFO] Key \"" << key << "\" Erased Successfully" << std::endl;
                else
                    std::cout << "[Error] Key \"" << key << "\" Doesn't Exist" << std::endl;
            }
        }
        else if (op == "save")
        {
            // Save The Map To Flash
            if (flashKV.saveMap())
                std::cout << "[INFO] FlashKV Map Saved Successfully" << std::endl;
            else
                std::cout << "[ERROR] Error Occurred While Saving The FlashKV Map" << std::endl;
        }
        else if (op == "help")
        {
            printHelp();
        }
        else if (op == "exit")
        {
            // Save The Map To Flash
            if (flashKV.saveMap())
                std::cout << "[INFO] FlashKV Map Saved Successfully" << std::endl;
            else
                std::cout << "[ERROR] Error Occurred While Saving The FlashKV Map" << std::endl;
            break;
        }
        else
        {
            std::cout << "[INFO] Unknown command: \"" << op << "\". Type 'help' For A List 0f Commands.\n";
        }
    }

    return 0;
}
