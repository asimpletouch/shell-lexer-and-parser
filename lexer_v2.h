#if !defined(LEXER_V2_H)

enum tokenType {
  WORD,
  FILEPATH,
  REDIRECTION_INPUT,         // <
  REDIRECTION_OUTPUT,        // >
  REDIRECTION_OUTPUT_APPEND, // >>
  PIPE,
  BACKGROUND,
  OPEN_PARENTHESE,
  CLOSE_PARENTHESE,
  SEQUENCE, // ;
  EOF_TOKEN
};

struct Token {
  tokenType type;
  int lexemeCapacity = 64; // really only 63 since need '\0'
  int lexemeSize = 0;
  char lexeme[64]; // todo(victor): how to use lexemeCapacity instead of "64" here?
};

struct TokenArray {
  Token *tokens;
  Token *eTokens;
  int tokenArraySize;
};

static char *whitespaceCharacters = " \t\r\n\v";
static char *wordChararacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789=.,-_";
static char *filePathChararacters = "ABCDEFGHIJKLMNOPQRSTUVWXZabcdefghijklmnopqrstuvwxz0123456789=.,-_/*?";

static char *terminalCharacters = "&;|()><";

static char *wordBoundry = " \t\r\n\v><|&();"; // whitespace and all the terminal symbols of the grammar

#define LEXER_V2_H
#endif