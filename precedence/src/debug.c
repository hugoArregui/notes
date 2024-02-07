#include "precedence.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *prints_tree(Node *node) {
  char *s = malloc(1000);
  if (node->left && node->right) {
    char *left = prints_tree(node->left);
    char *right = prints_tree(node->right);
    sprintf(s, "(%s %s %s)", node->content, left, right);
    free(left);
    free(right);
  } else if (node->left) {
    char *left = prints_tree(node->left);
    sprintf(s, "(%s %s NULL)", node->content, left);
    free(left);
  } else if (node->right) {
    char *s = malloc(1000);
    char *right = prints_tree(node->right);
    sprintf(s, "(%s NULL %s)", node->content, right);
    free(right);
  } else {
    strcpy(s, node->content);
  }
  return s;
}

void print_tree(Node *node) {
  char *s_tree = prints_tree(node);
  printf("%s\n", s_tree);
  free(s_tree);
}
