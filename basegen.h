#ifndef BASEGEN_H
#define BASEGEN_H

// STL
#include <string>
#include <list>
#include <fstream>

// POSIX
#include <unistd.h>

// PDTK
#include <cxxutils/posix_helpers.h>

enum class direction
{
  none = 0,
  in,
  out,
  inout,
  outin,
};

direction inverse(direction dir)
{
  switch (dir)
  {
    case direction::in:    return direction::out;
    case direction::out:   return direction::in;
    case direction::inout: return direction::outin;
    case direction::outin: return direction::inout;
    default: return direction::none;
  }
}

std::string func_name(std::string name, direction dir, bool is_local)
{
  switch (dir)
  {
    case direction::inout:
      return name + (is_local ? "Call" : "Return");
    case direction::outin:
      return name + (is_local ? "Return" : "Call");
    default: return name;
  }
}


struct CodePrinterBase
{
  struct argument_t
  {
    std::string type;
    std::string name;

    void clear(void)
    {
      type.clear();
      name.clear();
    }
  };

  struct function_descriptor
  {
    std::list<argument_t> remote_arguments;
    std::list<argument_t> local_arguments;

    std::string name;
    direction dir;

    void clear(void)
    {
      dir = direction::none;
      name.clear();
      remote_arguments.clear();
      local_arguments.clear();
    }
  };

  std::string remote_name(function_descriptor func)
  { return func_name(func.name, func.dir, false); }

  std::string local_name(function_descriptor func)
  { return func_name(func.name, func.dir, true); }

  std::fstream out;
  std::string relative_filename;
  bool is_server;
  const char* name;
  std::list<function_descriptor> functions;

  CodePrinterBase(void)
    : is_server(false), name(nullptr)
  { out.exceptions(std::ios_base::failbit | std::ios_base::badbit ); }
  virtual ~CodePrinterBase(void) { }

  bool have_local(void)
  {
    for(auto& func : functions)
      if(func.dir != direction::out)
        return true;
    return false;
  }

  bool have_remote(void)
  {
    for(auto& func : functions)
      if(func.dir != direction::in)
        return true;
    return false;
  }

  void writeFile(char* filename)
  {
    if(posix::access(filename, F_OK)) // if file exist
    {
      if(!posix::access(filename, W_OK)) // if write access is denied
        throw(std::system_error(errno, std::system_category()));
    }
    else if(errno == posix::errc::permission_denied) // if permission denied to test file existence
      throw(std::system_error(errno, std::system_category()));
    // else file not existing is ok ;)

    out.open(filename, std::ios_base::out | std::ios_base::trunc);
    if(out.fail())
      throw(std::system_error(int(std::errc::no_such_file_or_directory), std::system_category()));

    relative_filename = ::basename(filename);
    print();
    out.close();
  }

  virtual void print(void) = 0;
};


#endif // BASEGEN_H
