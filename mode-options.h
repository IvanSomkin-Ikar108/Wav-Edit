#ifndef MODEOPTIONS_H
#define MODEOPTIONS_H

#include <cstdint>

// TODO: Add comments about possible exceptions

struct BaseOptions
{
  const char* infile_path;
  BaseOptions(const int argc, const char* argv[]);
};

struct InfoOptions : BaseOptions
{
  InfoOptions(const int argc, const char* argv[]);
};

struct HexOptions : BaseOptions
{
  size_t max_print_count;
  HexOptions(const int argc, const char* argv[]);
};

struct TrimOptions : BaseOptions
{
  uint32_t start_ms = 0, end_ms;
  const char* outfile_path;
  bool end_flag = false, out_flag = false;
  TrimOptions(const int argc, const char* argv[]);
};

struct FadeOptions : BaseOptions
{
  uint32_t start_ms = 0, end_ms;
  double end_lvl_01 = 0.;
  const char* outfile_path;
  bool end_flag = false, out_flag = false;
  FadeOptions(const int argc, const char* argv[]);
};

struct ReverbOptions : BaseOptions
{
  uint32_t delay_ms = 1000;
  double decay_01 = 0.1;
  const char* outfile_path;
  bool out_flag = false;
  ReverbOptions(const int argc, const char* argv[]);
};

#endif
