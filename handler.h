#include <zen/zstring.h>
#include <zmq.h>
#include <zhelpers.h>

typedef struct{
  char *uuid, *conn, *path, *ip, *cookie, *method, *query;
} request;

request *receive(void *socket){
  char *netstr = s_recv(socket);
  request *r = malloc(sizeof(request));

  slist *t = tokenize(netstr, "\"");
  for(int i = 0; i < t->index; i++){
    if(strcmp(t->entry[i], "x-forwarded-for") == 0)
      r->ip = t->entry[i + 2];
    else if(strcmp(t->entry[i], "METHOD") == 0)
      r->method = t->entry[i + 2];
    else if(strcmp(t->entry[i], "cookie") == 0)
      r->cookie = t->entry[i + 2];     
    else if(strcmp(t->entry[i], "QUERY") == 0)
      r->query = t->entry[i + 2];    
  }
  slist *tt = tokenize(t->entry[0], " ");
  r->uuid = tt->entry[0];
  r->conn = tt->entry[1];
  r->path = tt->entry[2];

  return r;
}

void dump(request *r){
  fprintf(stdout, "uuid   = %s\n", r->uuid);
  fprintf(stdout, "conn   = %s\n", r->conn);
  fprintf(stdout, "path   = %s\n", r->path);
  fprintf(stdout, "ip     = %s\n", r->ip);
  //fprintf(stdout, "cookie = %s\n", r->cookie);
  fprintf(stdout, "method = %s\n", r->method);
  //fprintf(stdout, "query  = %s\n", r->query);
}

void reply(void *socket, request *r, char *body){
  char *reply = malloc(strlen(r->uuid) + strlen(r->conn) + strlen(body) + 1000);
  sprintf(reply, "%s %d:%s, HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s",
          r->uuid, strlen(r->conn), r->conn, strlen(body), body);

  s_send(socket, reply);
}
