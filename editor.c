// Includes //
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>

// Defines //
#define CTRL_KEY(k) ((k) & 0x1f)

// Data //
struct editorConfig{
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;

// Prototypes //
void editorRefreshScreen(void);
void editorProcessKeypress(void);

// Terminal //
void die(const char *s) {
    editorRefreshScreen(); // Clear screen
    perror(s); // Print error message
    exit(1); // Exit program
}

void disableRawMode() {
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1){ // Set terminal settings to original settings
        die("tcsetattr"); // Print error message and exit program
    }
}

void enableRawMode() {
   if (isatty(STDIN_FILENO)) {
        if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) {
            die("tcgetattr");
        }
    } else {
        die("Not a terminal");
    }

    atexit(disableRawMode);  // Register disableRawMode to be called on exit

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // Disable Ctrl-C, Ctrl-V, Ctrl-S, Ctrl-Q, Ctrl-O
    raw.c_oflag &= ~(OPOST); // Disable output processing
    raw.c_cflag |= (CS8);  // Set character size to 8 bits per byte
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);  // Disable echo, canonical mode, signals, and implementation-defined input processing
    raw.c_cc[VMIN] = 0; // Set the minimum number of bytes of input needed before read() can return
    raw.c_cc[VTIME] = 1; // Set the maximum amount of time read() can wait before returning

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1){ // Set terminal settings to raw settings
        die("tcsetattr"); // Print error message and exit program
    }
}

// Read Key //
char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

// Get Window Size //
int getWindowSize(int *rows, int *cols){
    struct winsize ws;
    
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

// Draw ~ //
void editorDrawRows() {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    write(STDOUT_FILENO, "~\r\n", 3);
  }
}

// Output //
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear screen
    write(STDOUT_FILENO, "\x1b[H", 3); // Reposition cursor

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3); // Reposition cursor
}

// Input //
void editorProcessKeypress() {
    char c = editorReadKey();
    
    switch (c) {
        case CTRL_KEY('q'):
            editorRefreshScreen();
            exit(0);
            break;
  }
}

// Init Editor //
void initEditor() {
    if(getWindowSize(&E.screenrows, &E.screencols) == -1){
        die("getWindowSize");
    }
}

// Init //
int main() {
    enableRawMode();
    initEditor();
    
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
    
    return 0;
}
