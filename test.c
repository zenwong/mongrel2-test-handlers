#include <mongrel/handler.h>
#include <zhelpers.h>
#include <zmq.h>

int main(int argc, char **argv){

  mtx *c = init("tcp://*:2222", "tcp://*:2223");

  while(1){

    request *r = mrecv(c->pull);

    mreply(c->pub, r, "hello world");

  }


  sleep(1);
  return 0;
}
