#ifndef BASE_H
#define BASE_H

#include <string>
#include <list>
#include <fstream>
#include <algorithm>

inline bool is_fd(const std::string& str)
  { return str == "posix::fd_t" || str == "fd_t"; }

struct CodePrinterBase
{
  virtual ~CodePrinterBase(){}
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
  std::list<function_descriptor> local_functions;
  std::list<function_descriptor> remote_functions;

  bool file_open(std::string filename)
  {
    out.open(filename, std::ios_base::out | std::ios_base::trunc);
    if(out.is_open())
    {
      size_t slash_pos = filename.find_last_of('/');
      if(slash_pos != std::string::npos)
        filename = filename.substr(slash_pos + 1);
      std::transform(filename.begin(), filename.end(), filename.begin(),
                     [](char l) { l = std::toupper(l); if(l < 'A' || l > 'Z') { l = '_'; } return l; });

      out << "#ifndef " << filename << std::endl
          << "#define " << filename << std::endl;
    }
    return out.is_open();
  }

  void file_close(void)
  {
    out << std::endl << std::endl << "#endif" << std::endl;
    out.close();
  }

  virtual void print_open  (void) = 0;
  virtual void print_close (void) = 0;
  virtual void print_local (void) = 0;
  virtual void print_remote(void) = 0;
};


#endif // BASE_H
