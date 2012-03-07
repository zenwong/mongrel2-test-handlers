#include <zstring.hpp>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace zmq;
using namespace boost;

struct connection{
  context_t ctx;
  socket_t  pull, pub;
  
  connection(const string& sub_addr, const string& pub_addr) 
    : ctx(1), pull(ctx, ZMQ_PULL), pub(ctx, ZMQ_PUB) {
      pull.connect(sub_addr.c_str());
      pub.connect( pub_addr.c_str());
    }
};


struct request{
  string uuid, conn, path, ip, cookie, method, query;
};

request mrecv(socket_t& socket){
  string netstr = s_recv(socket);
  request r;

  vector<string> t;
  split(t, netstr, is_any_of("\""));
  for(int i = 0; i < t.size(); i++){
    if(strcmp(t.at(i).c_str(), "x-forwarded-for") == 0)
      r.ip = t.at(i + 2);
    else if(strcmp(t.at(i).c_str(), "cookie") == 0)
      r.cookie = t.at(i + 2);
    else if(strcmp(t.at(i).c_str(), "METHOD") == 0)
      r.method = t.at(i + 2);
    else if(strcmp(t.at(i).c_str(), "QUERY") == 0)
      r.query = t.at(i + 2);
  }

  vector<string> line;
  split(line, t.at(0), is_any_of(" "));
  r.uuid = line.at(0);
  r.conn = line.at(1);
  r.path = line.at(2);

  return r;
}

void mreply(socket_t& socket, request& r, string body){
  ostringstream resp;
  resp << r.uuid << " " << r.conn.size() << ":" << r.conn << ", ";

  resp << "HTTP/1.1 200 OK\r\n";
  resp << "Content-Length: " << body.size() << "\r\n";
  resp << "\r\n" << body;

  //fprintf(stdout, "%s\n\n", resp.str().c_str());

  s_send(socket, resp.str());
  
}

