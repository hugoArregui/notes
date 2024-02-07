#include "precedence.h"
#include "debug.c"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *createNode(char *content) {
  Node *node = malloc(sizeof(Node));
  assert(node != NULL);
  strcpy(node->content, content);
  node->left = NULL;
  node->right = NULL;
  return node;
}

bool is_operator(char c) { return c == '+' || c == '*'; }

int operator_precedence(char c) {
  switch (c) {
  case '+': {
    return 1;
  }
  case '*': {
    return 2;
  }
  default: {
    return -1;
  }
  }
}

char *pop_token(char **s) {
  char *token = malloc(sizeof(char) * MAX_TOKEN_SIZE);
  int token_len = 0;
  while (**s == ' ') {
    (*s)++;
  }

  while (**s != 0) {
    if (**s == ' ') {
      break;
    } else if (is_operator(**s)) {
      if (token_len == 0) {
        token[token_len++] = **s;
        (*s)++;
      }
      break;
    } else {
      token[token_len++] = **s;
      (*s)++;
    }
  }

  token[token_len] = 0;

  return token;
}

char *peek_token(char **s) {
  char *original = *s;
  char *token = pop_token(s);
  *s = original;
  return token;
}

Node *parse_node(char **code) {
  char *token = pop_token(code);

  if (strlen(token) == 0) {
    return NULL;
  }

  Node *leaf = createNode(token);
  free(token);
  return leaf;
}

Node *parse_expression_incremental(char **code) {
  Node *left = parse_node(code);

  char *token = peek_token(code);
  if (strlen(token) == 0) {
    free(token);
    return left;
  }

  bool next_is_operator = is_operator(token[0]);
  free(token);

  if (next_is_operator) {
    Node *next = parse_node(code);
    next->left = left;
    next->right = parse_expression_incremental(code);
    assert(next->right != NULL && "missing right operator");
    return next;
  } else {
    return left;
  }
}

Node *parse_expression_decremental(char **code) {
  Node *left = parse_node(code);
  while (1) {
    char *next = peek_token(code);
    bool next_is_operator = is_operator(next[0]);
    free(next);
    if (next_is_operator) {
      Node *operator= parse_node(code);
      Node *right = parse_node(code);

      operator->left = left;
      operator->right = right;
      left = operator;
    } else {
      break;
    }
  }

  return left;
}

Node *parse_expression(char **code, int min_precedence) {
  Node *left = parse_node(code);
  while (1) {
    char *next = peek_token(code);
    int next_precendence = operator_precedence(next[0]);
    free(next);
    if (next_precendence == -1) {
      break;
    }

    if (next_precendence <= min_precedence) {
      break;
    }

    Node *operator= parse_node(code);
    Node *right = parse_expression(code, next_precendence); // NOTE !!!

    operator->left = left;
    operator->right = right;
    left = operator;
  }

  return left;
}

int main(void) {
  {
    // NOTE: parsing incremental precedence
    char *code = "2 + c * 3";
    Node *tree = parse_expression_incremental(&code);
    print_tree(tree);
  }

  {
    // NOTE: parsing decremental precedence
    // https://youtu.be/fIPO4G42wYE?t=2293
    char *code = "2 * c + 3";
    Node *tree = parse_expression_decremental(&code);
    print_tree(tree);
  }

  {
    // NOTE: parsing both in incremental and decremental precedence
    // https://youtu.be/fIPO4G42wYE?t=2577
    char *code = "2 + c * 3 + d";
    Node *tree = parse_expression(&code, -2);
    print_tree(tree);
  }
  return 0;
}
