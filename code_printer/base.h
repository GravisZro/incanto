#ifndef BASE_H
#define BASE_H

#include <string>
#include <list>
#include <fstream>

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
  std::string relative_filename;
  std::list<function_descriptor> local_functions;
  std::list<function_descriptor> remote_functions;

  bool file_open(std::string filename)
  {
    out.open(filename, std::ios_base::out | std::ios_base::trunc);
    if(out.is_open())
    {
      std::size_t slash_pos = filename.rfind('/');
      if(slash_pos != std::string::npos)
      {
        relative_filename = filename.substr(slash_pos + 1);
        print_open();
      }
    }
    return out.is_open();
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
