#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer_v2.h"

bool parser(TokenArray *tokensInfo);
bool parseLine(Token **tokens, Token *eTokens);
bool parsePipe(Token **tokens, Token *eTokens);
bool parseExec(Token **tokens, Token *eTokens);
bool parseExecPrime(Token **tokens, Token *eTokens);
bool parseRedirection(Token **tokens, Token *eTokens);
bool parseBlock(Token **tokens, Token *eTokens);

Token consumeToken(Token **tokens, Token *eTokens) {
  Token *sTokens = *tokens;
  Token ret;

  if (sTokens < eTokens) {
    ret = *sTokens;
    sTokens += 1;
  }
  else if (sTokens == eTokens) { // eof token
    return *sTokens;
  }

  *tokens = sTokens;
  return ret;
}

bool peek(Token **tokens, Token *eTokens, tokenType *searchTokens) {
  
  return true;
}

// runs the actual command, will not return if successful
void runcmd(cmd *command) {
  
}

// builds a parse tree, returns the root
bool parser(TokenArray *tokensInfo) {
  if (parseLine(&(tokensInfo->tokens), tokensInfo->eTokens)) {
    Token currentToken = consumeToken(&tokensInfo->tokens, tokensInfo->eTokens);
    if (currentToken.type == EOF_TOKEN) {
      printf("success!\n");
      return true;
    }
    else {
      printf("leftover tokens\n");
      exit(1);
    }
  }
  else {
    printf("no line found\n");
    exit(1);
  }
}

bool parseLine(Token **tokens, Token *eTokens) {
  if (parsePipe(tokens, eTokens)) {

    // { "&" }*
    while ((*tokens)->type == BACKGROUND) {
      consumeToken(tokens, eTokens);
    }

    // [ ";" line ]
    if ((*tokens)->type == SEQUENCE) {
      consumeToken(tokens, eTokens);
      return parseLine(tokens, eTokens);
    }

    return true;
  }
  else {
    printf("no pipe found\n");
    exit(1);
  }
}

bool parsePipe(Token **tokens, Token *eTokens) {
  if (parseExec(tokens, eTokens)) {
    if ((*tokens)->type == PIPE) {
      consumeToken(tokens, eTokens);
      return parsePipe(tokens, eTokens);
    }

    return true;
  }

  printf("unreachable code");
  assert(false);
}

bool parseExec(Token **tokens, Token *eTokens) {
  parseRedirection(tokens, eTokens);
  return parseExecPrime(tokens, eTokens);
}

bool parseExecPrime(Token **tokens, Token *eTokens) {

  //   "(" block
  // | { word redirection }*
  // | ε
  if ((*tokens)->type == OPEN_PARENTHESE) {
    consumeToken(tokens, eTokens);
    return parseBlock(tokens, eTokens);
  }
  else {
    while ((*tokens)->type == WORD || (*tokens)->type == FILEPATH) {
      consumeToken(tokens, eTokens);
      parseRedirection(tokens, eTokens);
    }
  }

  return true;
}

bool parseRedirection(Token **tokens, Token *eTokens) {

  //   { "<"  word }*
  // | { ">"  word }*
  // | { ">>" word }*
  // | ε
  while (((*tokens)->type == REDIRECTION_INPUT) ||
         ((*tokens)->type == REDIRECTION_OUTPUT) ||
         ((*tokens)->type == REDIRECTION_OUTPUT_APPEND)) {

    // consumeToken(tokens, eTokens);

    if ((*tokens)->type == REDIRECTION_INPUT) {
      consumeToken(tokens, eTokens); // consumes REDIRECTION_INPUT
      if ((*tokens)->type == WORD || (*tokens)->type == FILEPATH) {
        consumeToken(tokens, eTokens);
        return true;
      }
      else {
        printf("< must be followed by a word or filepath token\n");
        exit(1);
      }
    }
    else if ((*tokens)->type == REDIRECTION_OUTPUT) {
      consumeToken(tokens, eTokens); // consumes REDIRECTION_OUTPUT
      if ((*tokens)->type == WORD || (*tokens)->type == FILEPATH) {
        consumeToken(tokens, eTokens);
        return true;
      }
      else {
        printf("> must be followed by a word or filepath token\n");
        exit(1);
      }
    }
    else if ((*tokens)->type == REDIRECTION_OUTPUT_APPEND) {
      consumeToken(tokens, eTokens); // consumes REDIRECTION_OUTPUT_APPEND
      if ((*tokens)->type == WORD || (*tokens)->type == FILEPATH) {
        consumeToken(tokens, eTokens);
        return true;
      }
      else {
        printf(">> must be followed by a word or filepath token\n");
        exit(1);
      }
    }
  }

  // ε
  return true;
}

bool parseBlock(Token **tokens, Token *eTokens) {
  parseLine(tokens, eTokens);
  if ((*tokens)->type == CLOSE_PARENTHESE) {
    consumeToken(tokens, eTokens);
    return true;
  }
  else {
    printf("unmatched parentheses\n");
    exit(1);
  }
}