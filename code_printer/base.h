#ifndef BASE_H
#define BASE_H

// STL
#include <string>
#include <list>
#include <fstream>

// POSIX
#include <unistd.h>

// PDTK
#include <cxxutils/posix_helpers.h>

struct CodePrinterBase
{
  struct argument_t
  {
    std::string type;
    std::string name;
  };

  struct function_descriptor
  {
    std::string name;
    std::list<argument_t> arguments;
  };

  std::fstream out;
  std::string relative_filename;
  bool is_server;
  std::list<function_descriptor> local_functions;
  std::list<function_descriptor> remote_functions;

  CodePrinterBase(void) : is_server(false) { out.exceptions(std::ios_base::failbit | std::ios_base::badbit ); }
  virtual ~CodePrinterBase(void) { }

  void file_open(std::string filename)
  {
    if(::access(filename.c_str(), F_OK) == posix::success_response) // if file exist
    {
      if(::access(filename.c_str(), W_OK) == posix::error_response) // if write access is denied
        throw(std::system_error(int(std::errc::permission_denied), std::generic_category()));
    }
    else if(errno == std::errc::permission_denied) // if permission denied to test file existence
      throw(std::system_error(errno, std::generic_category()));
    // else file not existing is ok ;)

    try { out.open(filename, std::ios_base::out | std::ios_base::trunc); }
    catch(...) { throw(std::system_error(int(std::errc::no_such_file_or_directory), std::generic_category())); }

    posix::size_t slash_pos = filename.rfind('/');
    if(slash_pos != std::string::npos)
    {
      relative_filename = filename.substr(slash_pos + 1);
      print_open();
    }
  }

  void file_close(void)
  {
    print_close();
    out.close();
  }

  virtual void print_open  (void) = 0;
  virtual void print_close (void) = 0;
  virtual void print_local (void) = 0;
  virtual void print_remote(void) = 0;
};


#endif // BASE_H
