#include "networkb/binkp.h"
#include "networkb/connection.h"
#include "networkb/socket_connection.h"
#include "networkb/socket_exceptions.h"

#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/strings.h"

using std::clog;
using std::endl;
using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

using wwiv::net::BinkSide;
using wwiv::net::BinkP;
using wwiv::net::Accept;
using wwiv::net::Connect;
using wwiv::net::SocketConnection;
using wwiv::net::socket_error;

using wwiv::strings::starts_with;
using wwiv::strings::SplitString;

template <typename C, typename K>
bool contains(C container, K key) {
  return container.find(key) != end(container);
}

map<string, string> ParseArgs(int argc, char** argv) {
  map<string, string> args;
  for (int i=0; i < argc; i++) {
    const string s(argv[i]);
    if (starts_with(s, "--")) {
      vector<string> delims = SplitString(s, " =");
      string value = (delims.size() > 1) ? delims[1] : "";
      // lame old GCC doesn't have emplace.
      args.insert(std::make_pair(delims[0].substr(2), value));
    }
  }
  return args;
}

int main(int argc, char** argv) {
  map<string, string> args = ParseArgs(argc, argv);

  for (auto arg : args) {
    clog << "k: " << arg.first << "; v: " << arg.second << endl;
  }

  try {
    if (contains(args, "receive")) {
      clog << "BinkP receive" << endl;
      unique_ptr<SocketConnection> c(Accept(24554));
      BinkP binkp(c.get(), BinkSide::ANSWERING, 1, 2);
      binkp.Run();
    } else {
      // send
      unique_ptr<SocketConnection> c(Connect("localhost", 24554));
      BinkP binkp(c.get(), BinkSide::ORIGINATING, 2, 1);
      binkp.Run();
    } 
  } catch (socket_error e) {
    std::clog << e.what() << std::endl;
  }
  
}
