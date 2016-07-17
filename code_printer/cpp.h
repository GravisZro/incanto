#ifndef CPP_H
#define CPP_H

#include "base.h"

struct CodePrinter : CodePrinterBase
{
  void print_open(void)
  {
    out << std::endl << "// STL"
        << std::endl << "#include <iostream>"
        << std::endl << "#include <vector>"
        << std::endl
        << std::endl << "// PDTK"
        << std::endl << "#include <asocket.h>"
        << std::endl << "#include <cxxutils/hashing.h>"
        << std::endl << "#include <cxxutils/posix_helpers.h>"
        << std::endl
        << std::endl << "class RPC : public SingleSocket"
        << std::endl << "{"
        << std::endl << "public:"
        << std::endl << "  template<typename... Args>"
        << std::endl << "  RPC(Args... args) : SingleSocket(args...)";
    if(!remote_functions.empty())
      out << std::endl << "  { Object::connect(readFinished, this, &RPC::receive); }";
    else
      out << " { }";
  }

  void print_close(void)
  {
    out << std::endl << "};";
  }

  void print_remote(void)
  {
    if(remote_functions.empty())
      return;
    out << std::endl << "public:";

    for(function_descriptor& func : remote_functions)
    {
      out << std::endl << "  bool " << func.name << "(";
      for(auto pos = func.arguments.begin(); pos != func.arguments.end(); ++pos)
      {
        if(pos != func.arguments.begin())
          out << ", ";
        out << "const " << pos->type;
        if(pos->type.find("<") != std::string::npos || // if template OR
           pos->type.find("string") != std::string::npos) // if string
          out << "&";
        out << " " << pos->name;
      }

      out << ") { return call(\"" << func.name << "\", ";

      std::string fd;
      for(auto& arg : func.arguments)
        if(is_fd(arg.type))
          fd = arg.name;

      out << (fd.empty() ? "posix::invalid_descriptor" : fd);

      for(auto& arg : func.arguments)
        if(!is_fd(arg.type))
          out << ", " << arg.name;
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
    if(local_functions.empty())
      return;
    out << std::endl << "public:";
    for(function_descriptor& func : local_functions)
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
        << std::endl << "    std::string str;"
        << std::endl << "    if(!(msg.buffer >> str).hadError() && str == \"RPC\")"
        << std::endl << "    {"
        << std::endl << "      msg.buffer >> str;"
        << std::endl << "      switch(hash(str))"
        << std::endl << "      {";

    for(function_descriptor& func : local_functions)
    {
      out << std::endl << "        case \"" << func.name << "\"_hash:";

      if(func.arguments.empty())
        out << std::endl << "          Object::enqueue(" << func.name << ");";
      else
      {
        out << std::endl << "          struct { ";
        for(auto& arg : func.arguments)
          if(!is_fd(arg.type))
            out << arg.type << " " << arg.name << "; ";
        out << "} val;"
            << std::endl << "          msg.buffer";
        for(auto& arg : func.arguments)
          if(!is_fd(arg.type))
            out << " >> val." << arg.name;
        out << ";";
        out << std::endl << "          if(!msg.buffer.hadError())"
            << std::endl << "            Object::enqueue(" << func.name;

        for(auto& arg : func.arguments)
        {
          if(is_fd(arg.type))
            out << ", msg.fd_buffer";
          else
            out << ", val." << arg.name;
        }
        out << ");";
      }
      out << std::endl << "          break;";
    }
    out << std::endl << "      }"
        << std::endl << "    }"
        << std::endl << "  }";
  }
};

#endif // CPP_H
