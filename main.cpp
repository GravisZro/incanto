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
    printer->is_server = true;
    printer->local_functions .push_back({"setValueCall"   , {{"std::string", "key"}, {"std::string", "value"}}});
    printer->local_functions .push_back({"getValueCall"   , {{"std::string", "key"}}});

    printer->remote_functions.push_back({"setValueReturn" , {{"int", "errcode"}}});
    printer->remote_functions.push_back({"getValueReturn" , {{"std::string", "value"}}});

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
