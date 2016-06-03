#ifndef PDTK_PRINTER_H
#define PDTK_PRINTER_H

#include "code_printer.h"

class PDTKPrinter : public CodePrinter
{
public:
  void print_open(void)
  {
    out << std::endl << "#ifndef RPC_H"
        << std::endl << "#define RPC_H"
        << std::endl
        << std::endl << "// STL"
        << std::endl << "#include <iostream>"
        << std::endl << "#include <vector>"
        << std::endl
        << std::endl << "// PDTK"
        << std::endl << "#include <asocket.h>"
        << std::endl << "#include <cxxutils/hashing.h>"
        << std::endl
        << std::endl << "class RPC : public SingleSocket"
        << std::endl << "{"
        << std::endl << "public:"
        << std::endl << "  template<typename... Args>"
        << std::endl << "  RPC(Args... args) : SingleSocket(args...)"
        << std::endl << "  { Object::connect(readFinished, this, &RPC::receive); }";
  }

  void print_close(void)
  {
    out << std::endl << "};"
        << std::endl
        << std::endl << "#endif";
  }

  void print_remote(void)
  {
    out << std::endl << "public:";

    for(function_descriptor& func : functions)
    {
      out << std::endl << "bool " << func.name << "(";

      for(auto pos = func.arguments.begin(); pos != func.arguments.end(); ++pos)
      {
        if(pos != func.arguments.begin())
          out << ", ";
        out << pos->type << " " << pos->name;
      }

      if(func.descriptor)
      {
        if(!func.arguments.empty())
          out << ", ";
        out << "posix::fd_t fd";
      }

      out << ") { return call(\""
          << func.name
          << "\", "
          << (func.descriptor ? "fd" : "posix::invalid_descriptor")
          << ", ";

      for(auto& arg : func.arguments)
        out << ", " << arg.type << " " << arg.name;
      out << "); }";
    }

    out << std::endl << "private:"
        << std::endl << "  template<typename... ArgTypes>"
        << std::endl << "  bool call(const char* func_name, posix::fd_t fd, ArgTypes&... args)"
        << std::endl << "  {"
        << std::endl << "    vqueue data;"
        << std::endl << "    data.serialize(\"RPC\", func_name, signature, args...);"
        << std::endl << "    return write(data, fd);"
        << std::endl << "  }";
  }

  void print_local(void)
  {
    out << std::endl << "public:";
    for(function_descriptor& func : functions)
    {
      out << std::endl << "  signal<";

      for(auto pos = func.arguments.begin(); pos != func.arguments.end(); ++pos)
      {
        if(pos != func.arguments.begin())
          out << ", ";
        out << pos->type;
      }
      out << "> " << func.name << ";";
    }

    out << std::endl << "private:"
        << std::endl << "  void receive(message_t msg)"
        << std::endl << "  {"
        << std::endl << "    std::string tmpstr;"
        << std::endl << "    if(!(msg.buffer >> tmpstr).hadError() && tmpstr == \"RPC\")"
        << std::endl << "    {"
        << std::endl << "      msg.buffer >> tmpstr;"
        << std::endl << "      switch(hash(tmpstr))"
        << std::endl << "      {";

    for(function_descriptor& func : functions)
    {
      out << std::endl << "        case \"" << func.name << "\"_hash:";

      if(func.arguments.empty())
        out << std::endl << "        Object::enqueue(" << func.name << ");";
      else
      {
        out << std::endl << "        struct { ";
        for(auto& arg : func.arguments)
          out << arg.type << " " << arg.name << "; ";
        out << " } val;"
            << std::endl << "msg.buffer ";
        for(auto& arg : func.arguments)
          out << " >> " << arg.name;
        out << std::endl << "        if(!msg.buffer.hadError())"
            << std::endl << "          Object::enqueue(" << func.name;

        for(auto& arg : func.arguments)
          out << ", val." << arg.name;
        out << ");";
      }
      out << std::endl << "        break;";
    }
    out << std::endl << "      }"
        << std::endl << "    }"
        << std::endl << "  }";
  }
};

#endif // PDTK_PRINTER_H
