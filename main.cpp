#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "readfile.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::stringstream;

namespace modes
{
  const string help = "help";
  const string testmode = "testmode";
  const string printhex = "printhex";
}

struct TestFileParams
{
  const string& filePath;
  TestFileParams(const string& filePath):
    filePath(filePath)
  {}
};

struct ReadFileParams
{
  const string& filePath;
  const size_t& maxPrintCount;
  ReadFileParams(const string& filePath, const size_t& maxPrintCount):
    filePath(filePath),
    maxPrintCount(maxPrintCount)
  {}
};

void cout_as_hex(uint8_t byte, char separator);

void print_as_hex_columns(std::vector<uint8_t>& bytes, size_t columnsCount = 16, size_t maxPrintCount = 256);

void run_mode_help();

void run_mode_testmode(TestFileParams& params);

void run_mode_printhex(ReadFileParams& params);

int main(int argc, const char* argv[]) 
{ 
  if (argc > 1)
  {
    const string mode = string(argv[1]);

    if (mode == modes::help)
    {
      run_mode_help();
    }
    else if (mode == modes::testmode)
    {
      if (argc > 2)
      {
        const string filePath = string(argv[2]);
        TestFileParams params = TestFileParams(filePath);
        run_mode_testmode(params);
      }
      else
      {
        cout << "Invalid number of arguments for 'printhex'. Needs filepath.";
      }
    }
    else if (mode == modes::printhex)
    {
      if (argc > 2)
      {
        const string filePath = string(argv[2]);
        size_t maxPrintCount = 256; // int
        bool goodPrintCount = true;

        if (argc > 3)
        {
          int argv3Int;
          stringstream conversion;
          conversion << argv[3];
          conversion >> argv3Int;

          if (conversion.fail())
          {
            cout << "Invalid maxPrintCount parameter.";
            goodPrintCount = false;
          }
          else if (argv3Int < 1)
          {
            cout << "Parameter maxPrintCount should be more than 0.";
            goodPrintCount = false;
          }
          else
          {
            maxPrintCount = (size_t)argv3Int;
          }
        }

        if (goodPrintCount) 
        {
          ReadFileParams params = ReadFileParams(filePath, maxPrintCount);
          run_mode_printhex(params); 
        }
      }
      else
      {
        cout << "Invalid number of arguments for 'printhex'. Needs filepath.";
      }
    }
    else
    {
      cout << mode << " is an invalid mode. See 'wav-edit.exe help'.";
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
  cout << "USAGE:\n"
       << "simple-dsp.exe MODE PARAM1 PARAM2 ...\n\n"

       << "MODE = help\n"
       << "    Will print this help\n\n"

       << "MODE = testmode\n"
       << "    PARAM1 = path to a file\n"
       << "        Will test if the file is exists\n\n"

       << "MODE = printhex\n"
       << "    PARAM1 = path to a file\n"
       << "    PARAM2 = number of bytes\n"
       << "       Print first PARAM2 bytes as hex numbers\n" << endl;
}

void run_mode_testmode(TestFileParams& params)
{
  ifstream file(params.filePath.c_str());

  if (file.good())
  {
    cout << params.filePath << " exists.";
  }
  else
  {
    cout << params.filePath << " does not exist.";
  }
}

void run_mode_printhex(ReadFileParams& params)
{
  try
  {
    std::vector<uint8_t> bytes = readfile(params.filePath, params.maxPrintCount);
    print_as_hex_columns(bytes, 16, params.maxPrintCount);
    cout << "File read succesfully.\n";
  }
  catch (std::exception& e)
  {
    cerr << e.what() << endl;
    cout << "File read unsuccesfully.\n";
  }
}

void cout_as_hex(uint8_t byte, char separator)
{
  cout << std::hex << std::setfill('0') << std::setw(2) << (int)byte << separator;
}

void print_as_hex_columns(std::vector<uint8_t>& bytes, size_t columnsCount, size_t maxPrintCount)
{
    int printLength = maxPrintCount;
    if (bytes.size() < maxPrintCount)
    {
      printLength = bytes.size();
    }

    int columns = columnsCount;
    int rows = printLength / columns;
    int rest = printLength % columns;
    int i, j, readLength = 0;

    for (i = 0; i < rows; i++)
    {
      for (j = 0; j < columns - 1; j++)
      {
        cout_as_hex(bytes[readLength], ' ');
        readLength++;
      }
      cout_as_hex(bytes[readLength], '\n');
    }
    if (rest != 0) 
    {
      for (j = 0; j < rest - 1; j++)
      {
        cout_as_hex(bytes[readLength], ' ');
        readLength++;
      }
      cout_as_hex(bytes[readLength], '\n');
    }
}
