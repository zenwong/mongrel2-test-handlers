#include <zen/bsingle.h>
#include <zmq.h>
#include <zhelpers.h>

typedef struct{
  bstring uuid, conn, path, ip, cookie, method, query;
} request;

request *receive(void *socket){
  bstring netstr = bfromcstr(s_recv(socket));
  request *r = malloc(sizeof(request));

  struct bstrList *t = bsplits(netstr, bfromcstr(" \""));
  for(int i = 0; i < t->qty; i++){
    if(biseqcstrcaseless(t->entry[i], "x-forwarded-for") == 1)
      r->ip = t->entry[i + 2];
    else if(biseqcstrcaseless(t->entry[i], "METHOD") == 1)
      r->method = t->entry[i + 2];
    else if(biseqcstrcaseless(t->entry[i], "cookie") == 1){
      r->cookie = t->entry[i + 2]; 
      //fprintf(stdout, "cookie = %s\n", t->entry[i + 2]->data);
    }
    else if(biseqcstrcaseless(t->entry[i], "QUERY") == 1){
      r->query = t->entry[i + 2];  
      //fprintf(stdout, "query  = %s\n", t->entry[i + 2]->data);
    }
  }
  r->uuid = t->entry[0];
  r->conn = t->entry[1];
  r->path = t->entry[2];

  return r;
}

void reply(void *socket, request *r, bstring body){
  bstring http  = bformat("HTTP/1.1 200 OK\r\nContent-Length: %d", body->slen);
  bstring reply = bformat("%s %d:%s, %s\r\n\r\n%s", 
      r->uuid->data, blength(r->conn), r->conn->data, http->data, body->data);

  s_send(socket, reply->data);
}
