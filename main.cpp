#include "code_printer.h"
#include "pdtk_printer.h"

// STL
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
  CodePrinter* printer = nullptr;

  std::string filename = "/tmp/demo-file.h";

  printer = new PDTKPrinter;

  if(printer != nullptr)
  {
    if(printer->file_open(filename))
    {
      //printer->local_functions.push_back({"local_example", {{"posix::fd_t", "fd"}, {"long", "demo"}, {"std::string", "lol"}}});
      //printer->remote_functions.push_back({"remote_example", {{"std::string", "lulz"}, {"std::vector<int>", "arr"}}});
      printer->local_functions.push_back({"remote_example", {{"std::string", "lulz"}, {"std::vector<int>", "arr"}}});
      printer->print_open();
      printer->print_local();
      printer->print_remote();
      printer->print_close();
      printer->file_close();
      std::cout << "success!" << std::endl;
    }
    else
      std::cout << "failed!" << std::endl;

    delete printer;
    printer = nullptr;
  }

  return 0;
}
