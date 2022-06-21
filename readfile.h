#ifndef READFILE_H
#define READFILE_H

#include <vector>
#include <cstdint>
#include <string>


// Reads file from filePath into std::vector of bytes.
// If maxByteRead is more than 0 and less than file size, only reads maxByteRead bytes.
//
// Throws std::invalid_argument exception if filePath does not exist
// Throws std::runtime_error if error while reading file
std::vector<uint8_t> readfile(const char* filePath, size_t maxByteRead = 0);

#endif
