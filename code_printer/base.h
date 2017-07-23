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
/*
    void print(void)
    {
      switch(dir)
      {
        case direction::in:    std::cout << "in    "; break;
        case direction::out:   std::cout << "out   "; break;
        case direction::inout: std::cout << "inout "; break;
        case direction::outin: std::cout << "outin "; break;
        default:
          throw("oh shit");
      }

      if(name.empty() || remote_arguments.empty())
        std::cout << "void ";
      else
      {
        std::cout << "{ ";
        for(auto& arg : remote_arguments)
          std::cout << arg.type << " "  << arg.name << "; ";
        std::cout << "} ";
      }

      std::cout << name << " ";

      if(name.empty() || local_arguments.empty())
        std::cout << "(void)";
      else
      {
        std::cout << "( ";
        for(auto& arg : local_arguments)
          std::cout << arg.type << " "  << arg.name << ", ";
        std::cout << ")";
      }
      std::cout << std::endl;
    }
*/
  };

  std::string remote_name(function_descriptor func)
  { return func_name(func.name, func.dir, false); }

  std::string local_name(function_descriptor func)
  { return func_name(func.name, func.dir, true); }

  std::fstream out;
  std::string relative_filename;
  bool is_server;
  std::list<function_descriptor> functions;

  CodePrinterBase(void) : is_server(false) { out.exceptions(std::ios_base::failbit | std::ios_base::badbit ); }
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

  void writeFile(std::string filename)
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
      print();
    }
    out.close();
  }

  virtual void print(void) = 0;
};


#endif // BASE_H
