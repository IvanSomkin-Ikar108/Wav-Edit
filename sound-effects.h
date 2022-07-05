#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include <vector>
#include <cstdint>

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
//add commentaries about errors
namespace effects
{
  void trim(std::vector<uint8_t>& bytes, TrimOptions& options);

  void fade(std::vector<uint8_t>& bytes, FadeOptions& options);

  void reverb(std::vector<uint8_t>& bytes, ReverbOptions& options);
}

#endif
