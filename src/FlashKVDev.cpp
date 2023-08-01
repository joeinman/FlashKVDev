#include <fstream>
#include <iostream>
#include <optional>
#include <iomanip>
#include <vector>

#include <FlashKV/FlashKV.h>

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

    // Load The Store From File
    int loadStatus = flashKV.loadStore();
    if (loadStatus == 0)
        std::cout << "FlashKV Store Loaded Successfully" << std::endl;
    else if (loadStatus == 1)
        std::cout << "No FlashKV Store Found. A New One Will Be Created Upon Saving" << std::endl;
    else
        std::cout << "Error Occurred While Loading The FlashKV Store" << std::endl;

    // Try To Read A Value From The Store
    std::string key = "test";
    std::optional<std::vector<uint8_t>> value = flashKV.readKey(key);
    if (value)
    {
        std::cout << "Read Key: [" << key << "], Value: [";
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
        std::cout << "Key [" << key << "] Not Found." << std::endl;
    }

    // Write A Value To The Store
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    flashKV.writeKey(key, data);

    // Write Another Value To The Store
    flashKV.writeKey("test2", {0x05, 0x06, 0x07, 0x08});

    std::cout << "Writing Key: [" << key << "], Value: [";
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        if (it != data.begin())
            std::cout << " ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)*it;
    }
    std::cout << "]" << std::endl;

    std::cout << "All Keys:" << std::endl;
    auto allKeys = flashKV.getAllKeys();
    for (auto key : allKeys)
    {
        std::cout << "Key: [" << key << "], Value: [";
        std::optional<std::vector<uint8_t>> value = flashKV.readKey(key);
        if (value)
        {
            for (auto it = value->begin(); it != value->end(); ++it)
            {
                if (it != value->begin())
                    std::cout << " ";
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)*it;
            }
        }
        else
        {
            std::cout << "Not Found";
        }
        std::cout << "]" << std::endl;
    }

    // Save The Store To Flash
    if (flashKV.saveStore())
        std::cout << "FlashKV Store Saved Successfully" << std::endl;
    else
        std::cout << "Error Occurred While Saving The FlashKV Store" << std::endl;
}