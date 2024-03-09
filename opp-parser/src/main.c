#include "sdh_arena.h"

#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

enum ExprType {
  ExprType_BINOP,
  ExprType_NUMBER,
};

struct Expr;

struct Expr {
  enum ExprType type;
  char op;
  int int_value;
  struct Expr *rhs;
  struct Expr *lhs;
};

typedef struct Expr Expr;

typedef struct {
  int result;
  char op;
  bool is_binary_operator;
  int precedence;
  int int_value;
} TokenInfo;

TokenInfo get_token(stb_lexer *lexer, bool consume) {
  TokenInfo info = {0};
  char *parse_point = lexer->parse_point;
  info.result = stb_c_lexer_get_token(lexer);
  if (!consume) {
    lexer->parse_point = parse_point;
  }

  long token = lexer->token;

  switch (token) {
  case '-':
  case '+': {
    info.is_binary_operator = true;
    info.precedence = 1;
    info.op = token;
  } break;
  case '/':
  case '*': {
    info.is_binary_operator = true;
    info.precedence = 2;
    info.op = token;
  } break;
  default: {
    info.is_binary_operator = false;
    info.int_value = token;
  } break;
  }

  return info;
}

int consume_token(stb_lexer *lexer) { return stb_c_lexer_get_token(lexer); }

Expr *parse_primary(MemoryArena *arena, stb_lexer *lexer) {
  TokenInfo info = get_token(lexer, true);
  assert(info.is_binary_operator == false);
  Expr *expr = pushSize(arena, sizeof(Expr), DEFAULT_ALIGNMENT);
  expr->type = ExprType_NUMBER;
  expr->int_value = lexer->int_number;
  expr->rhs = NULL;
  expr->lhs = NULL;
  return expr;
}

Expr *parse_expression_1(MemoryArena *arena, stb_lexer *lexer, Expr *lhs,
                         int min_precedence) {
  TokenInfo lookahead = get_token(lexer, false);
  if (lookahead.result == 0) {
    return lhs;
  }

  while (lookahead.is_binary_operator &&
         lookahead.precedence >= min_precedence) {
    TokenInfo op = lookahead;
    assert(consume_token(lexer) != 0);
    Expr *rhs = parse_primary(arena, lexer);
    lookahead = get_token(lexer, false);
    while (lookahead.result != 0 && lookahead.is_binary_operator &&
           lookahead.precedence > min_precedence) {
      rhs = parse_expression_1(
          arena, lexer, rhs,
          op.precedence + (lookahead.precedence > op.precedence ? 1 : 0));

      lookahead = get_token(lexer, false);
    }
    Expr *expr = pushSize(arena, sizeof(Expr), DEFAULT_ALIGNMENT);
    expr->type = ExprType_BINOP;
    expr->op = op.op;
    expr->rhs = rhs;
    expr->lhs = lhs;
    lhs = expr;
  }

  return lhs;
}

Expr *parse_expression(MemoryArena *arena, stb_lexer *lexer) {
  return parse_expression_1(arena, lexer, parse_primary(arena, lexer), 0);
}

void compile_expr(Expr *expr) {
  if (expr->type == ExprType_BINOP) {
    printf("(%c ", expr->op);
    compile_expr(expr->lhs);
    printf(" ");
    compile_expr(expr->rhs);
    printf(")");
  } else {
    printf("%d", expr->int_value);
  }
}

int main(void) {
  size_t memory_size = Megabytes(100);
  void *base_address = (void *)(0);
  // NOTE: MAP_ANONYMOUS content initialized to zero
  void *memory_block = mmap(base_address, memory_size, PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  MemoryArena arena;
  initializeArena(&arena, memory_size, memory_block);

  size_t lexer_storage_size = 1000;
  char *lexer_storage = pushSize(&arena, lexer_storage_size, DEFAULT_ALIGNMENT);
  stb_lexer lexer;

  char *text = "1+2*3+4";
  stb_c_lexer_init(&lexer, text, text + strlen(text), lexer_storage,
                   lexer_storage_size);

  Expr *root = parse_expression(&arena, &lexer);
  assert(root != NULL);
  compile_expr(root);
  printf("\n");

  munmap(memory_block, memory_size);
  return 0;
}
