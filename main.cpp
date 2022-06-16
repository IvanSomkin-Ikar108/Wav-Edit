#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

struct params_test
{
  const string& fname;
  params_test(const string& name): fname(name){};
};

void run_mode_test(params_test& cfg)
{
}

void run_mode_help()
{
  cout << "USAGE:\n"
       << "simple-dsp.exe MODE PARAM1 PARAM2 ...\n\n"

       << "MODE = help\n"
       << "    Will print this help\n\n"

       << "MODE = testmode\n"
       << "    PARAM1 = path to a file\n"
       << "        Will test if the file is exists" << endl;
}

int main(int argc, const char* argv[]) 
{ 
  if (argc > 2)
  {
    cout << "mode = '" << argv[1] << "'" << endl;
    cout << "fname = '" << argv[2] << "'" << endl;

    const string mode = string(argv[1]);
    if ( mode == "help" || mode != "testmode" )
    {
      run_mode_help();
    }
    else
    {
      params_test cfg = params_test(string(argv[2]));
      run_mode_test(cfg);
    }
  }
  else
  {
    std::cout << "Hello, World!";
  }
    return 0;
}
