#include "mode-options.h"
#include "readfile.h"

#include <stdexcept>
#include <sstream>
#include <sys/stat.h>

/* Support functions */

int32_t cstr_to_int(const char* cstr);

double cstr_to_double(const char* cstr);

/* Option parsing implementation */

BaseOptions::BaseOptions(const int argc, const char* argv[])
{
  if (argc < 3)
  {
    throw std::invalid_argument("Error: No input file passed.");
  }

  infile_path = argv[2];

  if (!file_exists(infile_path))
  {
    throw std::invalid_argument("Error: Input file '" + std::string(infile_path) + "' does not exist.");
  }
}

InfoOptions::InfoOptions(const int argc, const char* argv[])
  : BaseOptions(argc, argv)
{
}

HexOptions::HexOptions(const int argc, const char* argv[])
  : BaseOptions(argc, argv)
{
  max_print_count = 256;

  if (argc > 4 && std::string(argv[3]) == std::string("-c"))
  {
    int32_t max_print_arg = cstr_to_int(argv[4]);
    if (max_print_arg < 1)
    {
      throw std::invalid_argument("Error: Parameter max_print_count (-с) should be more than 0.");
    }
    else
    {
      max_print_count = (size_t)max_print_arg;
    }
  }
}

TrimOptions::TrimOptions(const int argc, const char* argv[])
  : BaseOptions(argc, argv)
{ 
  int32_t start_arg, end_arg, idx = 3;
  while (idx < argc && argv[idx][0] == '-')
  {
    switch (argv[idx][1])
    {
      case 's':
        start_arg = cstr_to_int(argv[idx + 1]);
        if (start_arg < 0)
        {
          throw std::invalid_argument("Error: Start point (-s) should be positive or zero.");
        }
        start_ms = (uint32_t)start_arg;
        break;

      case 'e':
        end_flag = true;
        end_arg = cstr_to_int(argv[idx + 1]);
        if (end_arg < 0)
        {
          throw std::invalid_argument("Error: End point (-e) should be positive or zero.");
        }
        end_ms = (uint32_t)end_arg;
        break;

      case 'o':
        out_flag = true;
        outfile_path = argv[idx + 1];
        break;

      default:
        throw std::invalid_argument("Error: Invalid option '" + std::string(argv[idx]) + "' for 'trim' mode.");
    }
    idx += 2;
  }
  if (idx != argc)
  {
    throw std::invalid_argument("Error: Invalid options format.");
  }
  if (end_flag && start_arg > end_arg)
  {
    throw std::invalid_argument("Error: Start point (-s) must be lesser than or equal to end point (-e).");
  }
}

FadeOptions::FadeOptions(const int argc, const char* argv[])
  : BaseOptions(argc, argv)
{
  int32_t start_arg, end_arg, idx = 3;
  while (idx < argc && argv[idx][0] == '-')
  {
    switch (argv[idx][1])
    {
      case 's':
        start_arg = cstr_to_int(argv[idx + 1]);
        if (start_arg < 0)
        {
          throw std::invalid_argument("Error: Start point (-s) should be positive or zero.");
        }
        start_ms = (uint32_t)start_arg;
        break;

      case 'e':
        end_flag = true;
        end_arg = cstr_to_int(argv[idx + 1]);
        if (end_arg < 0)
        {
          throw std::invalid_argument("Error: End point (-e) should be positive or zero.");
        }
        end_ms = end_arg;
        break;

      case 'l':
        end_lvl_01 = cstr_to_double(argv[idx + 1]);
        if (end_lvl_01 < 0 || end_lvl_01 > 1)
        {
          throw std::invalid_argument("Error: End volume level (-l) should be a float from 0 to 1.");
        }
        end_ms = (uint32_t)end_arg;
        break;

      case 'o':
        out_flag = true;
        outfile_path = argv[idx + 1];
        break;

      default:
        throw std::invalid_argument("Error: Invalid option '" + std::string(argv[idx]) + "' for 'fade' mode.");
    }
    idx += 2;
  }
  if (idx != argc)
  {
    throw std::invalid_argument("Error: Invalid options format.");
  }
}

ReverbOptions::ReverbOptions(const int argc, const char* argv[])
  : BaseOptions(argc, argv)
{
  int32_t delay_arg, idx = 3;
  while (idx < argc && argv[idx][0] == '-')
  {
    switch (argv[idx][1])
    {
      case 'd':
        delay_arg = cstr_to_int(argv[idx + 1]);
        if (delay_arg < 0)
        {
          throw std::invalid_argument("Error: Delay time (-d) should be positive or zero.");
        }
        delay_ms = delay_arg;
        break;

      case 'k':
        decay_01 = cstr_to_double(argv[idx + 1]);
        if (decay_01 < 0 || decay_01 > 1)
        {
          throw std::invalid_argument("Error: Decay coefficient (-k) should be a float from 0 to 1.");
        }
        break;

      case 'o':
        out_flag = true;
        outfile_path = argv[idx + 1];
        break;

      default:
        throw std::invalid_argument("Error: Invalid option '" + std::string(argv[idx]) + "' for 'reverb' mode.");
    }
    idx += 2;
  }
  if (idx != argc)
  {
    throw std::invalid_argument("Error: Invalid options format.");
  } 
}

/* Support functions implementation */

int32_t cstr_to_int(const char* cstr)
{
  int32_t cstr_int;
  std::stringstream conversion;
  conversion << cstr;
  conversion >> cstr_int;

  if (conversion.fail())
  {
    throw std::invalid_argument("Could not convert " + std::string(cstr) + " to int.");
  }

  return cstr_int;
}

double cstr_to_double(const char* cstr)
{
  double cstr_double;
  std::stringstream conversion;
  conversion << cstr;
  conversion >> cstr_double;

  if (conversion.fail())
  {
    throw std::invalid_argument("Could not convert " + std::string(cstr) + " to double.");
  }

  return cstr_double;
}
