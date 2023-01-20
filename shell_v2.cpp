#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "lexer_v2.cpp"
#include "parser.cpp"

void printShellPrompt() {
  char const *prompt = "shell-prompt$";

  int const bufferByteCapacity = 256;
  char promptBuffer[bufferByteCapacity] = {};

  // copy prompt over to buffer
  for (int i = 0; *(prompt + i) != '\0'; ++i) {
    promptBuffer[i] = *(prompt + i);
  }

  int const byteOffset = strlen(prompt); // todo(victor): verify this offset
  if (getcwd((promptBuffer + byteOffset), (bufferByteCapacity - byteOffset)) != 0) {
    // print prompt
    printf("%s: ", promptBuffer);
    fflush(stdout);
  }
  else
    printf("getcwd failed\n");
}

int getcmd(char *buffer, int bufferCapacity) {
  // print the shell prompt
  printShellPrompt();

  // clear buffer
  memset(buffer, 0, bufferCapacity);

  // fill buffer
  int bytesRead = read(STDIN_FILENO, buffer, bufferCapacity);
  if (bytesRead == 0) {
    printf("read returned 0, indicates EOF, Means Ctrl + D pressed\n");
    return 0;
  }

  if (buffer[strlen(buffer)-1] == '\n') {
    buffer[strlen(buffer)-1] = '\0';
  }
  else {
    // todo(victor): realloc buffer here
    printf("buffer too small in getcmd\n");
    return 0;
  }

  if (buffer[0] == '\0') // means only a newline was entered
    return 0;

  return 1;
}

int main(int argc, char *argv[], char *envp[]) {
  int cmdBufferCapacity = 100;
  char cmdBuffer[cmdBufferCapacity]; // stdin input buffer

  while(getcmd(cmdBuffer, cmdBufferCapacity)) {
    TokenArray tokensInfo = lexer(cmdBuffer);
    Token *tokens = tokensInfo.tokens;

    // cd command here
    if (tokens[0].lexeme[0] == 'c' && tokens[0].lexeme[1] == 'd' && tokens[0].lexeme[2] == '\0') {
      if (chdir(tokens[1].lexeme) == -1) {
        printf("cannot cd %s\n", tokens[1].lexeme);
        perror("parser");
      }
      continue;
    }

    parser(&tokensInfo);

    /* if (fork() == 0) {
      runcmd(parser(&tokensInfo));
    }
    wait(NULL); */
  }
  exit(0);
}