#ifndef PDTKGEN_H
#define PDTKGEN_H

#include "basegen.h"
#include <algorithm>
#include <system_error>

#if 0
#define SWAPIF(x, y)
#else
#define SWAPIF(x, y) \
  if(func.dir == x || func.dir == y) \
    std::swap(func.local_arguments, func.remote_arguments);
#endif

struct PdtkCodePrinter : CodePrinterBase
{
  std::string classname;
  static inline bool is_fd(const std::string& str)
    { return str == "posix::fd_t" || str == "fd_t"; }

  void print(void)
  {
    std::string header_name = relative_filename;
    std::transform(header_name.begin(), header_name.end(), header_name.begin(),
                   [](char l) { l = std::toupper(l); if(l < 'A' || l > 'Z') { l = '_'; } return l; });

    out << std::endl << "#ifndef " << header_name
        << std::endl << "#define " << header_name
        << std::endl
        << std::endl << "// POSIX"
        << std::endl << "#include <sys/types.h>"
        << std::endl
        << std::endl << "// STL"
        << std::endl << "#include <vector>"
        << std::endl << "#include <cstdint>"
        << std::endl
        << std::endl << "// PDTK"
        << std::endl << "#include <object.h>"
        << std::endl << "#include <socket.h>"
        << std::endl << "#include <cxxutils/vfifo.h>"
        << std::endl << "#include <cxxutils/hashing.h>"
        << std::endl << "#include <cxxutils/posix_helpers.h>"
        << std::endl;
    if(is_server)
    {
      classname = "IncantoServer";
      out << std::endl << "class " << classname << " : public ServerSocket"
          << std::endl << "{"
          << std::endl << "public:"
          << std::endl << "  " << classname << "(void) noexcept"
          << std::endl << "  {"
          << std::endl << "    Object::connect(newPeerRequest, this, &" << classname << "::request);"
          << std::endl << "    Object::connect(newPeerMessage, this, &" << classname << "::receive);"
          << std::endl << "  }"
          << std::endl
          << std::endl << "  bool peerChooser(posix::fd_t socket, const proccred_t& cred) noexcept;";
    }
    else
    {
      classname = "IncantoClient";
      out << std::endl << "class " << classname << " : public ClientSocket"
          << std::endl << "{"
          << std::endl << "public:"
          << std::endl << "  " << classname << "(void) noexcept";
      if(have_remote())
        out << std::endl << "  { Object::connect(newMessage, this, &" << classname << "::receive); }";
      else
        out << " { }";
    }

    print_incantations();
    out << std::endl
        << std::endl << "private:";
    print_declarations();

    out << std::endl;

    if(have_local())
      out << std::endl << "  void receive(posix::fd_t socket, vfifo buffer, posix::fd_t fd) noexcept;";
    out << std::endl << "  void request(posix::fd_t socket, posix::sockaddr_t addr, proccred_t cred) noexcept;"
        << std::endl << "};"
        << std::endl;


    print_implementation();
    out << std::endl << "#endif" << std::endl;
  }

  void print_incantations(void)
  {
    if(!have_remote())
      return;
    out << std::endl;

    for(function_descriptor& func : functions)
    {
      if(func.dir == direction::in)
        continue;
      SWAPIF(direction::out, direction::outin)

      out << std::endl << "  bool " << remote_name(func) << "(";

      if(is_server)
        func.remote_arguments.push_front({"posix::fd_t", "socket"});

//      std::cout << (is_server ? "server " : "client ");
//      func.print();

      for(auto pos = func.remote_arguments.begin(); pos != func.remote_arguments.end(); ++pos)
      {
        if(pos != func.remote_arguments.begin())
          out << ", ";
        out << "const " << pos->type;
        if(pos->type.find("<") != std::string::npos || // if template OR
           pos->type.find("string") != std::string::npos) // if string
          out << "&";
        out << " " << pos->name;
      }

      out << ") const noexcept { return write(" << (is_server ? "socket, " : "") << "vfifo(\"RPC\", \"" << remote_name(func) << "\"";

      if(is_server)
        func.remote_arguments.pop_front();

      for(auto& arg : func.remote_arguments)
        if(!is_fd(arg.type))
          out << ", " << arg.name;

      out << "), ";

      int count = 0;
      for(auto& arg : func.remote_arguments)
      {
        if(is_fd(arg.type))
        {
          if(count)
            throw(std::system_error(int(std::errc::invalid_argument), std::generic_category(), "Only one file descriptor can be passed per incantation."));
          ++count;
          out << arg.name;
        }
      }
      if(!count)
        out << "posix::invalid_descriptor";

      out << "); }";

      SWAPIF(direction::out, direction::outin)
    }
  }

  void print_in_signals(void)
  {
    for(function_descriptor& func : functions)
    {
      if(func.dir == direction::out)
        continue;
      SWAPIF(direction::out, direction::outin)

      out << std::endl << "  signal<" << (is_server ? "posix::fd_t" : "");
      for(auto pos = func.local_arguments.begin(); pos != func.local_arguments.end(); ++pos)
      {
        if(is_server || pos != func.local_arguments.begin())
          out << ", ";
        out << pos->type;
      }
      out << "> " << local_name(func) << ";";

      SWAPIF(direction::out, direction::outin)
    }
  }

  void print_declarations(void)
  {
    for(function_descriptor& func : functions)
    {
      if(func.dir == direction::out)
        continue;
      SWAPIF(direction::out, direction::outin)

      out << std::endl << "  void " << local_name(func) << "(";
      if(is_server)
        out << "posix::fd_t socket";
      for(auto pos = func.local_arguments.begin(); pos != func.local_arguments.end(); ++pos)
      {
        if(is_server || pos != func.local_arguments.begin())
          out << ", ";
        out << pos->type << "& " << pos->name;
      }
      out << ") noexcept;";

      SWAPIF(direction::out, direction::outin)
    }
  }

  void print_implementation(void)
  {
    for(function_descriptor& func : functions)
    {
      if(func.dir == direction::out)
        continue;
      SWAPIF(direction::out, direction::outin)

      out << std::endl << "void " << classname << "::" << local_name(func) << "(";
      if(is_server)
        out << "posix::fd_t socket";
      for(auto pos = func.local_arguments.begin(); pos != func.local_arguments.end(); ++pos)
      {
        if(is_server || pos != func.local_arguments.begin())
          out << ", ";
        out << pos->type << "& " << pos->name;
      }
      out << ") noexcept"
          << std::endl << "{";
      if(func.dir == direction::inout)
      {
        for(auto& arg : func.remote_arguments)
          out << std::endl << "  " << arg.type << " " << arg.name << ";";

        out << std::endl << std::endl << "  " << remote_name(func) << "(socket";

        for(auto& arg : func.remote_arguments)
          out << ", " << arg.name;
        out << ");";
      }
      out << std::endl << "}" << std::endl;

      SWAPIF(direction::out, direction::outin)
    }

    if(is_server)
    {
      out << std::endl
          << std::endl << "bool " << classname << "::peerChooser(posix::fd_t socket, const proccred_t& cred) noexcept"
          << std::endl << "{"
          << std::endl << "  return true;"
          << std::endl << "}"
          << std::endl
          << std::endl << "void " << classname << "::request(posix::fd_t socket, posix::sockaddr_t addr, proccred_t cred) noexcept"
          << std::endl << "{"
          << std::endl << "  (void)addr;"
          << std::endl << "  if(peerChooser(socket, cred))"
          << std::endl << "    acceptPeerRequest(socket);"
          << std::endl << "  else"
          << std::endl << "    rejectPeerRequest(socket);"
          << std::endl << "}";
    }

    if(!have_local())
      return;

    out << std::endl
        << std::endl << "void " << classname << "::receive(posix::fd_t socket, vfifo buffer, posix::fd_t fd) noexcept"
        << std::endl << "{"
        << std::endl << "  (void)fd;"
        << std::endl << "  std::string str;"
        << std::endl << "  if(!(buffer >> str).hadError() && str == \"RPC\")"
        << std::endl << "  {"
        << std::endl << "    buffer >> str;"
        << std::endl << "    switch(hash(str))"
        << std::endl << "    {";

    for(function_descriptor& func : functions)
    {
      if(func.dir == direction::out)
        continue;
      SWAPIF(direction::out, direction::outin)

      out << std::endl << "      case \"" << local_name(func) << "\"_hash:"
          << std::endl << "      {";

      if(func.local_arguments.empty())
      {
        out << std::endl << "        " << local_name(func) << "(";
        if(is_server)
          out << "socket";
        out << ");";
      }
      else
      {
        out << std::endl << "        struct { ";
        for(auto& arg : func.local_arguments)
          if(!is_fd(arg.type))
            out << arg.type << " " << arg.name << "; ";
        out << "} val;"
            << std::endl << "        buffer";
        for(auto& arg : func.local_arguments)
          if(!is_fd(arg.type))
            out << " >> val." << arg.name;
        out << ";"
            << std::endl << "        if(!buffer.hadError())"
            << std::endl << "          " << local_name(func) << "(";
        if(is_server)
          out << "socket";

        for(auto pos = func.local_arguments.begin(); pos != func.local_arguments.end(); ++pos)
        {
          if(is_server || pos != func.local_arguments.begin())
            out << ", ";
          if(is_fd(pos->type))
            out << "fd";
          else
            out << "val." << pos->name;
        }
        out << ");";
      }
      out << std::endl << "      }"
          << std::endl << "      break;";

      SWAPIF(direction::out, direction::outin)
    }
    out << std::endl << "    }"
        << std::endl << "  }"
        << std::endl << "}";
  }
};

#endif // PDTKGEN_H
