#ifndef __H_PRECEDENCE

#define MAX_TOKEN_SIZE 12

struct Node;

struct Node {
  char content[MAX_TOKEN_SIZE];
  struct Node *left;
  struct Node *right;
};

typedef struct Node Node;

#define __H_PRECEDENCE
#endif
