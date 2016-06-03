#ifndef CODE_PRINTER_H
#define CODE_PRINTER_H

#include <string>
#include <list>
#include <iostream>

class CodePrinter
{
public:
  struct argument_t
  {
    std::string type;
    std::string name;
  };

  struct function_descriptor
  {
    std::string name;
    std::list<argument_t> arguments;
    bool descriptor;
  };

  std::ostream out;
  std::list<function_descriptor> functions;

  virtual void print_open  (void) = 0;
  virtual void print_close (void) = 0;
  virtual void print_local (void) = 0;
  virtual void print_remote(void) = 0;
};

#endif // CODE_PRINTER_H
