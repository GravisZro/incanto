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
    printer->functions.push_back({
                                   { },
                                   { "configUpdated", {}},
                                   direction::out,
                                 });

    printer->functions.push_back({
                                   { "set", {{"std::string", "key"}, {"std::string", "value"}}},
                                   { "set", {{"int", "errcode"}}},
                                   direction::outin,
                                 });

    printer->functions.push_back({
                                   { "get", {{"std::string", "key"}}},
                                   { "get", {{"std::string", "value"}}},
                                   direction::outin,
                                 });

    printer->functions.push_back({
                                   { "unset", {{"std::string", "key"}}},
                                   { },
                                   direction::in,
                                 });

    if(printer->is_server)
    {
      for(auto& func : printer->functions)
      {
        std::swap(func.local, func.remote);
        func.dir = inverse(func.dir);
      }
    }

    try
    {
      printer->writeFile(filename);
      std::cout << "success!" << std::endl;
    }
    catch(std::system_error e) { std::cout << "error: " << e.what() << std::endl; }
    catch(...) { std::cout << "unexpected error!" << std::endl; }
  }

  return 0;
}
