# FlashKVDev

![CMake](https://github.com/joeinman/FlashKVDev/actions/workflows/cmake.yml/badge.svg)

FlashKVDev is a basic demonstration application that allows users to experiment with the [FlashKV](https://github.com/joeinman/FlashKV) library on non-embedded systems. [FlashKV](https://github.com/joeinman/FlashKV) is a key-value store designed for use with flash memory.

## Usage

To run the FlashKVDev application, use the following command:

```
FlashKVDev.exe <FlashKV File>
```

Replace `<FlashKV File>` with the path to your FlashKV binary file.

## Inspecting The Binary File

If you want to inspect the contents of the binary file, you can use the `format-hex` command on Windows. Use the following command to output the contents to a text file:

```
format-hex <FlashKV File> >> FlashKV.txt
```

Replace `<FlashKV File>` with the path to your FlashKV binary file.