#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <sys/stat.h>
#include "readfile.h"
#include "printhex.h"
#include "wav-header.h"
#include "sound-effects.h"

namespace modes
{
  const std::string help = "help";
  const std::string test = "test";
  const std::string printhex = "printhex";
  const std::string printhead = "printhead";
  const std::string cut = "cut";
  const std::string levels = "levels";
  const std::string reverb = "reverb";
}

int32_t cstr_to_int(const char* cstr)
{
  int32_t cstr_int;
  std::stringstream conversion;
  conversion << cstr;
  conversion >> cstr_int;

  if (conversion.fail())
  {
    throw std::invalid_argument("Could not covvert " + std::string(cstr) + " to int.");
  }

  return cstr_int;
}

struct CutFileParams
{
  const char* filePath;
  uint32_t start_ms, end_ms;
  bool end_flag = false;

  CutFileParams(const int argc, const char* argv[])
  {
    int32_t start, end;
    if (argc > 3)
    {
      filePath = argv[2];

      start = cstr_to_int(argv[3]);

      if (start < 0)
      {
        throw std::invalid_argument("Start parameter must be positive or zero.");
      }

      if (argc > 4)
      { 
        end_flag = true;
        end = cstr_to_int(argv[4]);

        if (end < 0)
        {
          throw std::invalid_argument("End parameter must be positive or zero.");
        }

        if (start > end)
        {
          throw std::invalid_argument("Start parameter must be greater than or equal to end parameter.");
        }
      }
      start_ms = start;
      end_ms = end;
    }
    else
    {
      throw std::invalid_argument("Invalid number of arguments for 'cut'. Needs filepath, start, [end].");
    }
  }
};

struct LevelsFileParams{};

struct ReverbFileParams{};

struct TestFileParams
{
  const char* filePath;
  TestFileParams(const int argc, const char* argv[])
  {
    if (argc > 2)
    {
      filePath = argv[2];
    }
    else
    {
      throw std::invalid_argument("Invalid number of arguments for 'test'. Needs filepath.");
    }
  }
};

struct PrintHexFileParams
{
  const char* filePath;
  size_t maxPrintCount;
  PrintHexFileParams(const int argc, const char* argv[])
  {
    if (argc > 2)
    {
      filePath = argv[2];
      maxPrintCount = 256;

      if (argc > 3)
      {
        int argv3Int;
        std::stringstream conversion;
        conversion << argv[3];
        conversion >> argv3Int;

        if (conversion.fail())
        {
          throw std::invalid_argument("Invalid maxPrintCount parameter.");
        }
        else if (argv3Int < 1)
        {
          throw std::invalid_argument("Parameter maxPrintCount should be more than 0.");
        }
        else
        {
          maxPrintCount = (size_t)argv3Int;
        }
      }
    }
    else
    {
      throw std::invalid_argument("Invalid number of arguments for 'printhex'. Needs filepath, [maxPrintCount].");
    }
  }
};

struct PrintHeadFileParams{
  const char* filePath;
  PrintHeadFileParams(const int argc, const char* argv[])
  {
    if (argc > 2)
    {
      filePath = argv[2];
    }
    else
    {
      throw std::invalid_argument("Invalid number of arguments for 'print_head'. Needs filepath.");
    }
  }
};

bool fileExists();

void run_mode_help();

void run_mode_test(TestFileParams& params);

void run_mode_printhex(PrintHexFileParams& params);

void run_mode_printhead(PrintHeadFileParams& params);

void run_mode_cut(CutFileParams& params);

void run_mode_levels(LevelsFileParams& params);

void run_mode_reverb(ReverbFileParams& params);

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
      else if (mode == modes::test)
      {
        TestFileParams params = TestFileParams(argc, argv);
        run_mode_test(params);
      }
      else if (mode == modes::printhex)
      {
        PrintHexFileParams params = PrintHexFileParams(argc, argv);
        run_mode_printhex(params);
      }
      else if (mode == modes::printhead)
      {
        PrintHeadFileParams params = PrintHeadFileParams(argc, argv);
        run_mode_printhead(params);
      }
      else if (mode == modes::cut)
      {
        CutFileParams params = CutFileParams(argc, argv);
        run_mode_cut(params);
      }
      /* else if (mode == modes::levels)
      {
        LevelsFileParams params = LevelsFileParams(argc, argv);
        run_mode_levels(params);
      }
      else if (mode == modes::reverb)
      {
        ReverbFileParams params = ReverbFileParams(argc, argv);
        run_mode_reverb(params);
      }*/
      else
      {
        std::cerr << mode << " is an invalid mode. See 'wav-edit.exe help'.";
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

bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

/*
./wav-edit.exe printhex [-c 200] file1
./wav-edit.exe cut -s 100 [-e 200] file1 [file2]
./wav-edit.exe levels -s 1000 -e 2152 file1 [file2]
./wav-edit.exe reverb -dl 10 -dc 0.1 file1 [file2]
*/

void run_mode_help()
{
  std::cout
    << "USAGE:\n"
    << "simple-dsp.exe MODE PARAM1 PARAM2 ...\n\n"

    << "MODE = help\n"
    << "    Will print this help\n\n"

    << "MODE = test\n"
    << "    PARAM1 = path to a file\n"
    << "        Will test if the file is exists\n\n"

    << "MODE = print_head\n"
    << "    PARAM1 = path to a file\n"
    << "        Print header of .WAV file\n\n"

    << "MODE = cut\n"
    << "    PARAM1 = path to a file\n"
    << "        Print header of .WAV file\n\n"

    << "MODE = levels\n"
    << "    PARAM1 = path to a file\n"
    << "        Print header of .WAV file\n\n"

    << "MODE = reverb\n"
    << "    PARAM1 = path to a file\n"
    << "        Print header of .WAV file\n\n"

    << "MODE = printhex\n"
    << "    PARAM1 = path to a file\n"
    << "    PARAM2 = number of bytes\n"
    << "       Print first PARAM2 bytes as hex numbers\n" << std::endl;
}

void run_mode_test(TestFileParams& params)
{
  std::ifstream file(params.filePath);

  if (file.good())
  {
    std::cout << params.filePath << " exists.";
  }
  else
  {
    std::cout << params.filePath << " does not exist or is not a file.";
  }
}

void run_mode_printhex(PrintHexFileParams& params)
{
  try
  {
    std::vector<uint8_t> bytes = readfile(params.filePath, params.maxPrintCount);
    print_as_hex_columns(bytes, 16, params.maxPrintCount);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

void run_mode_printhead(PrintHeadFileParams& params)
{
  try
  {
    WavHeader header = WavHeader(params.filePath);
    if (header.check_validity())
    {
      std::cout << header.to_string() << "\n";
    }
    else
    {
      throw std::invalid_argument("This is not a correct WAV file.\n");
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

void run_mode_cut(CutFileParams& params)
{
  try
  {
    std::vector<uint8_t> bytes = readfile(params.filePath);

    if (params.end_flag)
    {
      effects::cut(bytes, params.start_ms, params.end_ms);
    }
    else
    {
      effects::cut(bytes, params.start_ms);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
};
/*
void run_mode_levels(LevelsFileParams& params)
{
  try
  {
    WavHeader header = WavHeader(params.filePath);
    if (header.check_validity())
    {
      std::cout << header.to_string() << "\n";
    }
    else
    {
      throw std::invalid_argument("This is not a correct WAV file.\n");
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
};

void run_mode_reverb(ReverbFileParams& params)
{
  try
  {
    WavHeader header = WavHeader(params.filePath);
    if (header.check_validity())
    {
      std::cout << header.to_string() << "\n";
    }
    else
    {
      throw std::invalid_argument("This is not a correct WAV file.\n");
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
};
*/
