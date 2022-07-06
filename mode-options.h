#ifndef MODEOPTIONS_H
#define MODEOPTIONS_H

#include <cstdint>

struct InfoOptions{
  const char* file_path;
  InfoOptions(const int argc, const char* argv[]);
};

struct HexOptions
{
  const char* file_path;
  size_t max_print_count;
  HexOptions(const int argc, const char* argv[]);
};

struct TrimOptions
{
  uint32_t start_ms, end_ms;
  const char* outfile_path;
  bool end_flag = false, out_flag = false;
  TrimOptions(const int argc, const char* argv[]);
};

struct FadeOptions
{
  uint32_t start_ms, end_ms;
  double end_lvl_01 = 0.;
  const char* outfile_path;
  bool end_flag = false, out_flag = false;
  FadeOptions(const int argc, const char* argv[]);
};

struct ReverbOptions
{
  uint32_t delay_ms = 1000;
  double decay_01 = 0.1;
  const char* outfile_path;
  bool out_flag = false;
  ReverbOptions(const int argc, const char* argv[]);
};

#endif
