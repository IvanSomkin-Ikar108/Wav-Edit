#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include "readfile.h"
#include "printhex.h"
#include "wav-header.h"

namespace modes
{
  const std::string help = "help";
  const std::string test = "test";
  const std::string printhex = "printhex";
  const std::string print_head = "print_head";
}

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

void run_mode_help();

void run_mode_test(TestFileParams& params);

void run_mode_printhex(PrintHexFileParams& params);

void run_mode_print_head(PrintHeadFileParams& params);

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
      else if (mode == modes::print_head)
      {
        PrintHeadFileParams params = PrintHeadFileParams(argc, argv);
        run_mode_print_head(params);
      }
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
    std::cout << "File read succesfully.\n";
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    std::cout << "File read unsuccesfully.\n";
  }
}

void run_mode_print_head(PrintHeadFileParams& params)
{
  try
  {
    WavHeader header = WavHeader(params.filePath);
    if (header.check_validity())
    {
      std::cout << header.to_string();
    }
    else
    {
      std::cout << "It isn't correct .wav file.\n";
    }
    std::cout << "File read succesfully.\n";
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    std::cout << "File read unsuccesfully.\n";
  }
}
