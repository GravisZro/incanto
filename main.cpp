// STL
#include <iostream>
#include <memory>
#include <string>
#include <functional>

// PDTK
#include <cxxutils/hashing.h>

// incanto
#include "code_printer/cpp.h"


int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  std::unique_ptr<CodePrinterBase> printer;

  std::string filename = "/tmp/demo-file.h";

  printer = std::make_unique<CppCodePrinter>();

  if(printer != nullptr)
  {

    throw(std::system_error((int)std::errc::invalid_argument, std::generic_category(), "Pointers and references are not allowed with the exception of string literals."));
    throw(std::system_error((int)std::errc::invalid_argument, std::generic_category(), "Only \"fixed width integer types\", STL containers thereof, strings and posix::fd_t types are allowed."));

    printer->local_functions .push_back({"local_example"  , {{"posix::fd_t", "fd"}, {"long", "demo"}, {"std::string", "lol"}}});
    printer->remote_functions.push_back({"remote_example" , {{"std::string", "lulz"}, {"std::vector<int>", "arr"}}});
    printer->local_functions .push_back({"local_example2" , {{"std::string", "lulz"}, {"std::vector<int>", "arr"}}});
    printer->remote_functions.push_back({"remote_example2", {{"long", "demo"}, {"posix::fd_t", "fd"}, {"std::string", "lol"}}});

    try
    {
      printer->file_open(filename);
      printer->print_local();
      printer->print_remote();
      printer->file_close();
      std::cout << "success!" << std::endl;
    }
    catch(std::system_error e) { std::cout << "error: " << e.what() << std::endl; }
    catch(...) { std::cout << "unexpected error!" << std::endl; }
  }

  return 0;
}
