#include "readfile.h"
#include "writefile.h"
#include "printhex.h"
#include "mode-options.h"
#include "wav-header.h"
#include "sound-effects.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <sys/stat.h>

namespace modes
{
  const std::string help = "help";
  const std::string hex = "hex";
  const std::string info = "info";
  const std::string trim = "trim";
  const std::string fade = "fade";
  const std::string reverb = "reverb";
}

/* Support functions */

// If file already exists, ask user if they want to rewrite it.
// Returns true, if file path is empty or if user confirms rewriting.
// Returns false, if user denies rewriting.
bool check_for_replace_dialogue(const char* file_path);

const char* get_infile_path(const int argc, const char* argv[]);

bool file_exists(const char* file_path);


/* Mode functions */

// Show usage guide.
void run_mode_help();

// Show WAV header information.
void run_mode_info(const char* infile_path, InfoOptions& options);

// Show first bytes of file as hex.
void run_mode_hex(const char* infile_path, HexOptions& options);

// Apply sound effect to file.
// The effect is chosen based on the O type of the options.
template <typename O>
void run_mode_effect(const char* infile_path, O& options);


int main(const int argc, const char* argv[])
{
  if (argc > 1)
  {
    const std::string mode = std::string(argv[1]);
    try
    {
      if (mode == modes::help)
      {
        run_mode_help();
      }
      else if (mode == modes::info)
      {
        const char* infile_path = get_infile_path(argc, argv);
        InfoOptions options = InfoOptions(argc, argv);
        run_mode_info(infile_path, options);
      }
      else if (mode == modes::hex)
      {
        const char* infile_path = get_infile_path(argc, argv);
        HexOptions options = HexOptions(argc, argv);
        run_mode_hex(infile_path, options);
      }
      else if (mode == modes::trim)
      {
        const char* infile_path = get_infile_path(argc, argv);
        TrimOptions options = TrimOptions(argc, argv);
        run_mode_effect<TrimOptions>(infile_path, options);
      }
      else if (mode == modes::fade)
      {
        const char* infile_path = get_infile_path(argc, argv);
        FadeOptions options = FadeOptions(argc, argv);
        run_mode_effect<FadeOptions>(infile_path, options);
      }
      else if (mode == modes::reverb)
      {
        const char* infile_path = get_infile_path(argc, argv);
        ReverbOptions options = ReverbOptions(argc, argv);
        run_mode_effect<ReverbOptions>(infile_path, options);
      }
      else
      {
        std::cerr << "Error: " << mode << " is an invalid mode. See 'wav-edit[.exe] help'.";
      }
    }
    catch (const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
  }
  else
  {
    run_mode_help();
  }
  return 0;
}


/* Mode functions implementation */

void run_mode_help()
{
  std::cout
    << "USAGE:\n"
    << "wav-edit[.exe] MODE [FILEPATH] [OPTIONS]...\n\n"

    << "MODE = help\n"
    << "    Will print this help\n\n"

    << "MODE = info FILEPATH\n"
    << "    Will print header information of WAVE file\n\n"

    << "MODE = hex FILEPATH\n"
    << "    Will print first file bytes as hex numbers\n"
    << "    OPTIONS:\n"
    << "    -c = maximum number of bytes to print (256 by default)\n\n"

    << "MODE = trim FILEPATH\n"
    << "    Will trim WAVE data from start point to end point\n"
    << "    OPTIONS:\n"
    << "    -s = start point of the trimmed fragment in milliseconds (start of data by default)\n"
    << "    -e = end point of the trimmed fragment in milliseconds (end of data by default)\n"
    << "    -o = output file path (same file by default)\n\n"

    << "MODE = fade FILEPATH\n"
    << "    Will add a 'fade away' effect with volume decreasing from start point to end point\n"
    << "    OPTIONS:\n"
    << "    -s = start point of the effect in milliseconds (start of data by default)\n"
    << "    -e = end point of the effect in milliseconds (end of data by default)\n"
    << "    -l = volume level at end point from 0 to 1 (0 by default)\n"
    << "    -o = output file path (same file by default)\n\n"

    << "MODE = reverb FILEPATH\n"
    << "    Will add a 'reverb' effect with selected delay and decay coefficient\n"
    << "    OPTIONS:\n"
    << "    -d = reverb delay in milliseconds (1000 by default)\n"
    << "    -k = reverb decay coefficient from 0 to 1 (0.1 by default)\n"
    << "    -o = output file path (same file by default)\n" << std::endl;
}

void run_mode_info(const char* infile_path, InfoOptions& options)
{
  try
  {
    WavHeader header = WavHeader(infile_path);
    if (header.check_validity())
    {
      std::cout << header.to_string() << "\n";
    }
    else
    {
      throw std::invalid_argument("Error: This is not a correct WAV file.\n");
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

void run_mode_hex(const char* infile_path, HexOptions& options)
{
  try
  {
    std::vector<uint8_t> bytes = readfile(infile_path, options.max_print_count);
    print_as_hex_columns(bytes, 16, options.max_print_count);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

// Trim effect.
void effect(std::vector<uint8_t>& bytes, TrimOptions& options)
{
  effects::trim(bytes, options);
}

// Fade effect.
void effect(std::vector<uint8_t>& bytes, FadeOptions& options)
{
  effects::fade(bytes, options);
}

// Reverb effect.
void effect(std::vector<uint8_t>& bytes, ReverbOptions& options)
{
  effects::reverb(bytes, options);
}

template <typename O>
void run_mode_effect(const char* infile_path, O& options)
{
  try
  {
    const char* outfile_path = options.out_flag ? options.outfile_path : infile_path;
    std::vector<uint8_t> bytes = readfile(infile_path);

    // Effect function is selected based on options type
    effect(bytes, options); 
    
    if (check_for_replace_dialogue(outfile_path))
    {
      writefile(bytes, outfile_path);
      std::cout << "WAVE file succesfully edited and written to " << outfile_path << std::endl;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
};

/* Support functions implementation */

bool check_for_replace_dialogue(const char* file_path)
{
  if (file_exists(file_path))
  {
    std::string answer;
    std::cout << "File " << file_path << " already exists. Would you like to replace it (y/n)?: ";
    std::getline(std::cin, answer);
    if (answer == std::string("n") || answer == std::string("N"))
    {
      return false;
    }
  }
  return true;
}

const char* get_infile_path(const int argc, const char* argv[])
{
  if (argc < 3)
  {
    throw std::invalid_argument("Error: No input file passed.");
  }
  
  const char* infile_path = argv[2];

  if (!file_exists(infile_path))
  {
    throw std::invalid_argument("Error: Input file '" + std::string(infile_path) + "' does not exist.");
  }

  return infile_path;
}

bool file_exists(const char* file_path)
{
    struct stat buf;
    if (stat(file_path, &buf) != -1)
    {
        return true;
    }
    return false;
}
