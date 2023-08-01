# FlashKVDev

FlashKVDev is a basic demonstration application that allows users to experiment with the FlashKV library on non-embedded systems. FlashKV is a key-value store designed for use with flash memory.

## Usage

To run the FlashKVDev application, use the following command:

```
FlashKVDev.exe <FlashKV File>
```

Replace `<FlashKV File>` with the path to your FlashKV binary file.

## Inspecting the Binary File

If you want to inspect the contents of the binary file, you can use the `format-hex` command on Windows. This command will output the contents of the binary file in hexadecimal format.

Use the following command to output the contents to a text file:

```
format-hex <FlashKV File> >> FlashKV.txt
```

Replace `<FlashKV File>` with the path to your FlashKV binary file. The `>>` operator will append the output to `FlashKV.txt`.