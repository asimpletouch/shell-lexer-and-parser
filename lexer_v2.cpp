#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "lexer_v2.h"

void PrintToken(tokenType type) {
  if (type == WORD) {
    printf("WORD\n");
  }
  else if (type == FILEPATH) {
    printf("FILEPATH\n");
  }
  else if (type == REDIRECTION_INPUT) {
    printf("REDIRECTION_INPUT\n");
  }
  else if (type == REDIRECTION_OUTPUT) {
    printf("REDIRECTION_OUTPUT\n");
  }
  else if (type == REDIRECTION_OUTPUT_APPEND) {
    printf("REDIRECTION_OUTPUT_APPEND\n");
  }
  else if (type == PIPE) {
    printf("PIPE\n");
  }
  else if (type == BACKGROUND) {
    printf("BACKGROUND\n");
  }
  else if (type == OPEN_PARENTHESE) {
    printf("OPEN_PARENTHESE\n");
  }
  else if (type == CLOSE_PARENTHESE) {
    printf("CLOSE_PARENTHESE\n");
  }
  else if (type == SEQUENCE) {
    printf("SEQUENCE\n");
  }
  else if (type == EOF_TOKEN) {
    printf("EOF_TOKEN\n");
  }
}

void NullTerminateToken(Token *mytoken) {
  if (mytoken->lexemeSize >= 1) {
    mytoken->lexeme[mytoken->lexemeSize] = '\0';
  }
  else {
    printf("NullTerminateToken error\n");
  }
}

void TokenInsertCharacterValue(Token *mytoken, char c) {
  // need to leave space for '\0'
  if (mytoken->lexemeSize < (mytoken->lexemeCapacity - 1)) {
    mytoken->lexeme[mytoken->lexemeSize] = c;
    mytoken->lexemeSize += 1;
  }
  else {
    printf("TokenInsertCharacterValue overflow\n");
    printf("Capacity: %d\n", mytoken->lexemeCapacity);
    printf("Size: %d\n", mytoken->lexemeSize);
  }
}

bool IsCharacter(char currentCharacter, char *charList, bool checkNull = false) {
  if (checkNull && currentCharacter == '\0')
    return true;
  
  for (; *charList; ++charList) {
    if (*charList == currentCharacter)
      return true;
  }
  return false;
}

void ConsumeChar(char **commandLineBufferPointer, char *commandLineBufferEndPointer) {
  if (*commandLineBufferPointer < commandLineBufferEndPointer)
    *commandLineBufferPointer += 1;
  else {
      printf("GetChar beyond EOL\n");
  }
}

bool canGetWordToken(char *commandLineBuffer, char *commandLineBufferEndPointer) {
  int wordSize = 0;

  while (commandLineBuffer < commandLineBufferEndPointer && IsCharacter(*commandLineBuffer, wordChararacters)) {
      commandLineBuffer += 1;
      wordSize += 1; 
  }

  if (IsCharacter(*commandLineBuffer, wordBoundry, true) && (wordSize >= 1)) {
    return true;
  }

  return false;
}

bool canGetFilepathToken(char *commandLineBuffer, char *commandLineBufferEndPointer) {
  int filepathSize = 0;

  while (commandLineBuffer < commandLineBufferEndPointer && IsCharacter(*commandLineBuffer, filePathChararacters)) {
      commandLineBuffer += 1;
      filepathSize += 1; 
  }

  if (IsCharacter(*commandLineBuffer, wordBoundry, true) && (filepathSize >= 1)) {
    return true;
  }

  return false;
}

Token GetWordToken(char **commandLineBufferPointer, char *commandLineBufferEndPointer) {
  Token wordToken;
  wordToken.type = WORD;
  char *commandLineBuffer = *commandLineBufferPointer;

  while (IsCharacter(*commandLineBuffer, wordChararacters)) {
    TokenInsertCharacterValue(&wordToken, *commandLineBuffer);
    // dont have to check for boundry condition since this function will only be called if canGetWordToken returned true
    commandLineBuffer += 1;
  }

  *commandLineBufferPointer = commandLineBuffer;
  return wordToken;
}

Token GetFilepathToken(char **commandLineBufferPointer, char *commandLineBufferEndPointer) {
  Token filepathToken;
  filepathToken.type = FILEPATH;
  char *commandLineBuffer = *commandLineBufferPointer;

  while (IsCharacter(*commandLineBuffer, filePathChararacters)) {
    TokenInsertCharacterValue(&filepathToken, *commandLineBuffer);
    // dont have to check for boundry condition since this function will only be called if canGetFilepathToken returned true
    commandLineBuffer += 1;
  }

  *commandLineBufferPointer = commandLineBuffer;
  return filepathToken;
}

Token GetToken(char **cmdBufferPointer, char *cmdBufferEndPointer, char *cmdBufferStartPointer) {
  Token myToken;
  char *cmdBuffer = *cmdBufferPointer;

  // remove leading whitespace
  while (IsCharacter(*cmdBuffer, whitespaceCharacters)) {
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
  }

  // try to get a word token
  if (canGetWordToken(cmdBuffer, cmdBufferEndPointer)) {
    myToken = GetWordToken(&cmdBuffer, cmdBufferEndPointer);
    NullTerminateToken(&myToken);
  }

  // try to get a filepath token
  else if (canGetFilepathToken(cmdBuffer, cmdBufferEndPointer)) {
    myToken = GetFilepathToken(&cmdBuffer, cmdBufferEndPointer);
    NullTerminateToken(&myToken);
  }

  // ">" and ">>"
  else if (*cmdBuffer == '>') {
    bool printNoWhiteSpaceError = false;

    // check previous character for whitespace
    if (cmdBuffer > cmdBufferStartPointer && !IsCharacter(*(cmdBuffer-1), whitespaceCharacters))
      printNoWhiteSpaceError = true;

    // consume '>'
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
    myToken.type = REDIRECTION_OUTPUT;

    // check for ">>"
    if (*cmdBuffer == '>') {
      ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
      myToken.type = REDIRECTION_OUTPUT_APPEND;

      // check next character for whitespace
      if (printNoWhiteSpaceError || !IsCharacter(*cmdBuffer, whitespaceCharacters, true)) {
        printf(">> token must be surrounded by whitespace\n");
        exit(1);
      }
    }
    else if (printNoWhiteSpaceError || !IsCharacter(*cmdBuffer, whitespaceCharacters, true)) {
      printf("> token must be surrounded by whitespace\n");
      exit(1);
    }

  }

  else if (*cmdBuffer == '<') {
    // check previous character for whitespace
    if (cmdBuffer > cmdBufferStartPointer && !IsCharacter(*(cmdBuffer-1), whitespaceCharacters)) {
      printf("< token must be surrounded by whitespace\n");
      exit(1);
    }

    // consume '<'
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
    myToken.type = REDIRECTION_INPUT;

    // check next char for whitespace
    if (!IsCharacter(*cmdBuffer, whitespaceCharacters, true)) {
      printf("< token must be surrounded by whitespace\n");
      exit(1);
    }
  }

  else if (*cmdBuffer == '|') {
     // check previous character for whitespace
    if (cmdBuffer > cmdBufferStartPointer && !IsCharacter(*(cmdBuffer-1), whitespaceCharacters)) {
      printf("| token must be surrounded by whitespace\n");
      exit(1);
    }

    // consume '|'
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
    myToken.type = PIPE;

    // check next char for whitespace
    if (!IsCharacter(*cmdBuffer, whitespaceCharacters, true)) {
      printf("| token must be surrounded by whitespace\n");
      exit(1);
    }
  }

  else if (*cmdBuffer == '&') {
     // check previous character for whitespace
    if (cmdBuffer > cmdBufferStartPointer && !IsCharacter(*(cmdBuffer-1), whitespaceCharacters)) {
      printf("& token must be surrounded by whitespace\n");
      exit(1);
    }

    // consume '&'
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
    myToken.type = BACKGROUND;

    // check next char for whitespace
    if (!IsCharacter(*cmdBuffer, whitespaceCharacters, true)) {
      printf("& token must be surrounded by whitespace\n");
      exit(1);
    }
  }
  
  else if (*cmdBuffer == '(') {
    // consume '('
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
    myToken.type = OPEN_PARENTHESE;
  }
  
  else if (*cmdBuffer == ')') {
    // consume ')'
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
    myToken.type = CLOSE_PARENTHESE;
  }
  
  else if (*cmdBuffer == ';') {
    // consume ';'
    ConsumeChar(&cmdBuffer, cmdBufferEndPointer);
    myToken.type = SEQUENCE;
  }
  
  else if (*cmdBuffer == '\0') {
    myToken.type = EOF_TOKEN;
  }
  else {
    // no known token
    printf("Unmatched character: %c\n", *cmdBuffer);
    exit(1);
  }

  // checks?
  *cmdBufferPointer = cmdBuffer;
  return myToken;
}

// note(victor): might need to be double pointer
TokenArray lexer(char *cmdBuffer) {

  assert(cmdBuffer[strlen(cmdBuffer)] == '\0');

  char *cmdBufferEnd = cmdBuffer + strlen(cmdBuffer);

  int tokenArrayCapacity = 100;
  Token *tokenArray = (Token *)malloc(sizeof(Token) *tokenArrayCapacity);
  int i = 0;

  while (i < tokenArrayCapacity) {
    Token myToken = GetToken(&cmdBuffer, cmdBufferEnd, cmdBuffer);
    tokenArray[i] = myToken;

    if (myToken.type == EOF_TOKEN) {
      i += 1;
      break;
    }

    i += 1;
  }

  for (int j = 0; j < i; ++j)
    PrintToken(tokenArray[j].type);

  TokenArray ret = {
    .tokens = tokenArray,
    .eTokens = tokenArray + i, // todo(victor): verifiy this points to the end
    .tokenArraySize = i
  };

  return ret;
}