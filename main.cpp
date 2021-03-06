// STL
#include <memory>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <functional>

// POSIX
#include <libgen.h>

// PUT
#include <put/cxxutils/hashing.h>

// project files
#include "putgen.h"

std::list<CodePrinterBase::function_descriptor> parser(const std::string& data)
{
  enum class state_e
  {
    origin = 0, // client or server
    iotype,     // in, out, inout, outin

    retSimple,  // simple return type
    retType,    // complex return type
    retName,    // name for complex return value
    retEnd,     // end of return value
    retListEnd, // end of return type list

    funcName,   // function name
    funcNameEnd,// end of function name

    argType,    // argument type
    argName,    // argument name
    argEnd,     // end of argument
    argListEnd, // end of argument list
  };


  std::list<CodePrinterBase::function_descriptor> functions;
  CodePrinterBase::function_descriptor func;
  CodePrinterBase::argument_t arg;
  std::list<CodePrinterBase::argument_t> arguments;
  bool inverted = false;

  const char* begin = data.data();
  const char* end = begin + data.size();

  state_e state = state_e::origin;
  std::string str;

  for(const char* pos = begin; pos < end; ++pos)
  {
    if(pos[0] == '\\' && pos[1] == '\n') // line continuation feature; applicable _ANYWHERE_
    {
      ++pos;
      continue;
    }

    if(str.empty() &&
       (std::isspace(*pos) || *pos == ';'))
      continue;

    switch(state)
    {
      case state_e::origin:
        if(std::isspace(*pos))
        {
          switch(hash(str))
          {
            case "client"_hash:
              inverted = true;
              break;
            case "server"_hash:
              inverted = false;
              break;
            default:
              throw(std::string("unknown modifier: '").append(str).append("'"));
          }
          str.clear();
          state = state_e::iotype;
        }
        else
          str.push_back(*pos);
        break;

      case state_e::iotype:
        if(std::isspace(*pos))
        {
          switch(hash(str))
          {
            case "in"_hash:    func.dir = direction::in;    break;
            case "out"_hash:   func.dir = direction::out;   break;
            case "inout"_hash: func.dir = direction::inout; break;
            case "outin"_hash: func.dir = direction::outin; break;
            default:
              throw(std::string("unknown direction: '").append(str).append("'"));
          }
          str.clear();
          state = state_e::retSimple;
        }
        else
          str.push_back(*pos);
        break;

      case state_e::retSimple:
        switch(*pos)
        {
          case '(':
          case '{':
            if(str.empty())
            {
              state = state_e::retType;
              continue;
            }
          case ')':
          case '}':
          case ',':
          case ';':
            throw(std::string("invalid character in simple return type: '").append(1, *pos).append("'"));

          case ' ':
          case '\f':
          case '\n':
          case '\r':
          case '\t':
          case '\v':
            if(str == "void")
              str.clear();
            if(!str.empty())
            {
              arg.type = str;
              str.clear();
              arguments.push_back(arg);
              arg.clear();
              func.remote_arguments = arguments;
              arguments.clear();
            }
            state = state_e::funcName;
            break;

          default:
            str.push_back(*pos);
        }
        break;

      case state_e::argType:
      case state_e::retType:
        switch(*pos)
        {
          case '(':
          case '{':
            throw(std::string("invalid character in complex return type: '").append(1, *pos).append("'"));

          case ';':
          case ',':
          case ')':
          case '}':
            --pos; // preserve position
          case ' ':
          case '\f':
          case '\n':
          case '\r':
          case '\t':
          case '\v':
            if(str != "void")
              arg.type = str;
            str.clear();
            state = state == state_e::retType ? state_e::retName : state_e::argName;
            break;

          default:
            str.push_back(*pos);
        }
        break;

      case state_e::argName:
      case state_e::retName:
        switch(*pos)
        {
          case '(':
          case '{':
            throw(std::string("invalid character in complex return name: '").append(1, *pos).append("'"));

          case ';':
          case ',':
          case ')':
          case '}':
            --pos; // preserve position
          case ' ':
          case '\f':
          case '\n':
          case '\r':
          case '\t':
          case '\v':
            arg.name = str;
            str.clear();
            state = (state == state_e::retName) ? state_e::retEnd : state_e::argEnd;
            break;

          default:
            str.push_back(*pos);
        }
        break;

      case state_e::argEnd:
      case state_e::retEnd:
        if(!arg.type.empty())
          arguments.push_back(arg);
        arg.clear();
        switch (*pos)
        {
          case ';':
          case ',':
            state = (state == state_e::retEnd) ? state_e::retType : state_e::argType;
            break;

          case ')':
          case '}':
            --pos; // preserve position
            state = (state == state_e::retEnd) ? state_e::retListEnd : state_e::argListEnd;
            break;

          default:
            throw(std::string("unexpected character at end of argument: '").append(1, *pos).append("'"));
        }

        break;

      case state_e::retListEnd:
        func.remote_arguments = arguments;
        arguments.clear();
        state = state_e::funcName;
        break;

      case state_e::argListEnd:
        func.local_arguments = arguments;
        arguments.clear();
        if(inverted)
          func.dir = inverse(func.dir);
        functions.push_back(func);
        func.clear();
        state = state_e::origin;
        break;

      case state_e::funcName:
        switch(*pos)
        {
          case '(':
          case '{':
            --pos; // preserve position
          case ' ':
          case '\f':
          case '\n':
          case '\r':
          case '\t':
          case '\v':
            func.name = str;
            func.name = str;
            str.clear();
            state = state_e::funcNameEnd;
            break;

          case ')':
          case '}':
          case ';':
          case ',':
            throw(std::string("invalid character in function name: '").append(1, *pos).append("'"));

          default:
            str.push_back(*pos);
        }
        break;

      case state_e::funcNameEnd:
        switch(*pos)
        {
          case '(':
          case '{':
            state = state_e::argType;
            break;

          default:
            throw(std::string("unexpected character at end of function name: '").append(1, *pos).append("'"));
        }
        break;
    }
  }

  return functions;
}

#define VERSION "0.1"

void usage(char* filename)
{
  std::cout      << "Usage: " << filename << " -c [-t type] [-n name] [-i] FILE [-o] FILE"
    << std::endl << "       " << filename << " -s [-t type] [-n name] [-i] FILE [-o] FILE"
    << std::endl << "  -c        Generate client code."
    << std::endl << "  -s        Generate server code."
    << std::endl << "  -t type   Type of code to generate. Values: put, qt, c, gtk, python, perl"
    << std::endl << "  -n name   Name of the generated class/struct/module."
    << std::endl << "  -i file   Input filename."
    << std::endl << "  -o file   Output filename."
    << std::endl << "  -v        Print version and copyright information."
    << std::endl << "  -h        Print this help message."
    << std::endl;
}

int main(int argc, char** argv)
{
  const char* input = nullptr;
        char* output = nullptr; // not const due to use of basename()
  const char* type = nullptr;
  const char* name = nullptr;
  int perspective = 0;

  for(int opt = 0; opt != -1; opt = posix::getopt(argc, argv, "vcsht:n:i:o:"))
  {
    switch (opt)
    {
      case 0: break;

      case 'v':

        std::cout
            << "Incanto " << VERSION        << std::endl
            << "Copyright (c) 2018, Gravis" << std::endl
            << "All rights reserved."       << std::endl;
        return EXIT_SUCCESS;

      case 's':
      case 'c': perspective = opt; break;
      case 'i': input  = optarg; break;
      case 'o': output = optarg; break;
      case 't': type = optarg; break;
      case 'n': name = optarg; break;

      case 'h':
        usage(basename(argv[0]));
        return EXIT_SUCCESS;
      default:
        usage(basename(argv[0]));
        return EXIT_FAILURE;
    }
  }

  for(;optind < argc; ++optind)
  {
    if(input == nullptr)
      input = argv[optind];
    else if(output == nullptr)
      output = argv[optind];
  }

  if(!perspective ||
     input == nullptr ||
     output == nullptr)
  {
    usage(basename(argv[0]));
    return EXIT_FAILURE;
  }

  if(type == nullptr) // default output type
    type = "put";
  if(name == nullptr) // default name
    name = perspective == 's' ? "IncantoServer" : "IncantoClient";

  try
  {
    std::unique_ptr<CodePrinterBase> printer;
    switch(hash(type))
    {
      case "put"_hash: printer = std::make_unique<PUTCodePrinter>(); break;
      case "qt"_hash:     throw(std::system_error(int(posix::errc::not_supported), std::generic_category(), "Qt code generation backend is not implemented!"));
      case "c"_hash:      throw(std::system_error(int(posix::errc::not_supported), std::generic_category(), "C code generation backend is not implemented!"));
      case "gtk"_hash:    throw(std::system_error(int(posix::errc::not_supported), std::generic_category(), "GTK+ code generation backend is not implemented!"));
      case "python"_hash: throw(std::system_error(int(posix::errc::not_supported), std::generic_category(), "Python code generation backend is not implemented!"));
      case "perl"_hash:   throw(std::system_error(int(posix::errc::not_supported), std::generic_category(), "Perl code generation backend is not implemented!"));
    }

    printer->is_server = perspective == 's';
    printer->name = name;

    char raw_data[UINT16_MAX] = { 0 };
    std::string data;
    std::fstream file;

    file.open(input, std::ios_base::in);
    if(!file.is_open())
      throw("unable to open input file");

    while(!file.eof())
    {
      file.read(raw_data, UINT16_MAX);
      data.append(raw_data, posix::size_t(file.gcount()));
    }
    file.close();

    if(printer == nullptr)
      throw("unable to allocate code printer");
    printer->functions = parser(data);

    if(!printer->is_server)
      for(auto& func : printer->functions)
        func.dir = inverse(func.dir);
    printer->writeFile(output);
  }
  catch(std::string str) { std::cerr << "parser error: " << str << std::endl; }
  catch(std::system_error e) { std::cerr << "error: " << e.what() << std::endl; }
  catch(...) { std::cerr << "unexpected error!" << std::endl; }

  return EXIT_SUCCESS;
}
