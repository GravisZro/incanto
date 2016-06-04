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
      printer->functions.push_back({"example", {{"posix::fd_t", "fd"}, {"long", "demo"}}});
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
