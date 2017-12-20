# Merkle Root
This program can be used to verify transactions using Merkle root of a block over the Bitcoin blockchain using C++. The program fetches block information from an API over the Bitcoin blockchain in JSON format. Then parses through it and calculates the Merkle Root of the block. Later, the information is compared with the derived value to check for inconsistencies.
___
## Prerequisites
The program utilizes the following external libraries:

- [JsonCpp](https://github.com/open-source-parsers/jsoncpp "JsonCpp Source")

- [libcurl](https://curl.haxx.se/libcurl/ "libcurl download")

- [OpenSSL](https://github.com/openssl/openssl "OpenSSL Source")

## Recommendations
This program has only been tested using the [*insight*](https://insight.bitpay.com/ "insight API") API and [Block Explorer](https://blockexplorer.com/ "Block Explorer API") API. In the long term, the Block Explorer API has turned out to be more stable out of the two. This program is dependent on the JSON format response of the block information from the API such that the appropriate libraries can calculate the Merkle Root. A sample format has been provided above for the [block.json](block.json) file.

## Compiling
The program compiles with the following flags:
> -ljsoncpp  -lcrypto  -lcurl

## Debugging
The program allows debugging and view of under the hood SHA-256 and Merkle Trees in the Bitcoin implementation. In order to debug, set the debug variable to true.

```C++
\\ debug
bool print = true;
```
## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE) file for more details
