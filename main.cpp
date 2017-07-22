// STL
#include <iostream>
#include <memory>
#include <string>
#include <functional>

// PDTK
#include <cxxutils/hashing.h>

// incanto
#include "code_printer/cpp.h"

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
            case "in"_hash:
              func.dir = direction::in;
              break;
            case "out"_hash:
              func.dir = direction::out;
              break;
            case "inout"_hash:
              func.dir = direction::inout;
              break;
            case "outin"_hash:
              func.dir = direction::outin;
              break;
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
              func.remote.arguments.push_back(arg);
              arg.clear();
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
        func.remote.arguments = arguments;
        arguments.clear();
        state = state_e::funcName;
        break;

      case state_e::argListEnd:
        func.local.arguments = arguments;
        arguments.clear();
        if(inverted)
        {
          std::swap(func.local, func.remote);
          func.dir = inverse(func.dir);
          inverted = false;
        }
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
            func.remote.name = str;
            func.local.name = str;
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



int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  std::unique_ptr<CodePrinterBase> printer;

  std::string filename = "/tmp/demo-file.h";

  printer = std::make_unique<CppCodePrinter>();

  try
  {
    if(printer == nullptr)
      throw("unable to allocate code printer");
    printer->is_server = true;
    std::string data = "  server out   void configUpdated(void);\n"
                       "  server inout {int errcode} unset(std::string key);\n"
                       "  server inout {int errcode} set(std::string key, std::string value);\n"
                       "  server inout {int errcode, std::string value} get(std::string key);\n";
    printer->functions = parser(data);

    if(printer->is_server)
    {
      for(auto& func : printer->functions)
        if(func.local.arguments.empty())
          func.local.name.clear();
    }
    else
    {
      for(auto& func : printer->functions)
      {
        std::swap(func.local, func.remote);
        func.dir = inverse(func.dir);
      }

      for(auto& func : printer->functions)
        if(func.remote.arguments.empty())
          func.remote.name.clear();
    }

    printer->writeFile(filename);
    std::cout << "success!" << std::endl;
  }
  catch(std::string str) { std::cout << "parser error: " << str << std::endl; }
  catch(std::system_error e) { std::cout << "error: " << e.what() << std::endl; }
  catch(...) { std::cout << "unexpected error!" << std::endl; }
/*
  if(printer != nullptr)
  {
    printer->is_server = false;
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
*/
  return 0;
}
