#ifndef MODEOPTIONS_H
#define MODEOPTIONS_H

#include <cstdint>

// Throws std::invalid_argument exception if input file is not passed or is not exist.;
struct BaseOptions
{
  const char* infile_path;
  BaseOptions(const int argc, const char* argv[]);
};

// Throws std::invalid_argument exception if input file is not passed or is not exist.;
struct InfoOptions : BaseOptions
{
  InfoOptions(const int argc, const char* argv[]);
};

// Throws std::invalid_argument exception if input file is not passed or is not exist.;
// Throws std::invalid_argument exception if invalid option or options value was passed.;
// In particular, "-c" parameter should be more than 0.
struct HexOptions : BaseOptions
{
  size_t max_print_count;
  HexOptions(const int argc, const char* argv[]);
};

// Throws std::invalid_argument exception if input file is not passed or is not exist.;
// Throws std::invalid_argument exception if invalid option or options value was passed.;
// In particular, "-s" and "-e" parameters should be positive or zero and "-s" lesser than or equal to "-e".
struct TrimOptions : BaseOptions
{
  uint32_t start_ms = 0, end_ms;
  const char* outfile_path;
  bool end_flag = false, out_flag = false;
  TrimOptions(const int argc, const char* argv[]);
};

// Throws std::invalid_argument exception if input file is not passed or is not exist.;
// Throws std::invalid_argument exception if invalid option or options value was passed.;
// In particular, "-s" and "-e" parameters should be positive or zero and "-l" parameter should be float from 0 to 1.
struct FadeOptions : BaseOptions
{
  uint32_t start_ms = 0, end_ms;
  double end_lvl_01 = 0.;
  const char* outfile_path;
  bool end_flag = false, out_flag = false;
  FadeOptions(const int argc, const char* argv[]);
};

// Throws std::invalid_argument exception if input file is not passed or is not exist.;
// Throws std::invalid_argument exception if invalid option or options value was passed.;
// In particular, "-d" parameter should be positive or zero and "-k" parametr should be float from 0 to 1.
struct ReverbOptions : BaseOptions
{
  uint32_t delay_ms = 1000;
  double decay_01 = 0.1;
  const char* outfile_path;
  bool out_flag = false;
  ReverbOptions(const int argc, const char* argv[]);
};

#endif
