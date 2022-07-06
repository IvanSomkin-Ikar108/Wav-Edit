#ifndef READFILE_H
#define READFILE_H

#include <vector>
#include <cstdint>
#include <string>

bool file_exists(const char* file_path);

// Reads file from file_path into std::vector of bytes.
// If max_byte_read is more than 0 and less than file size, only reads max_byte_read bytes.
//
// Throws std::invalid_argument exception if file_path does not exist
// Throws std::runtime_error if error while reading file
std::vector<uint8_t> readfile(const char* file_path, size_t max_byte_read);

// Reads full file from file_path into std::vector of bytes.
//
// Throws std::invalid_argument exception if file_path does not exist
// Throws std::runtime_error if error while reading file
std::vector<uint8_t> readfile(const char* file_path);

#endif
