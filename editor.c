// Includes //
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

// Data //
struct termios orig_termios;

// Terminal //
void die(const char *s){
    perror(s); // Print error message
    exit(1); // Exit program
}

void disableRawMode() {
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1){ // Set terminal settings to original settings
        die("tcsetattr"); // Print error message and exit program
    }
}

void enableRawMode() {
   if (isatty(STDIN_FILENO)) {
        if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
            die("tcgetattr");
        }
    } else {
        die("Not a terminal");
    }

    atexit(disableRawMode);  // Register disableRawMode to be called on exit

    struct termios raw = orig_termios;
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

// Init //
int main() {
    enableRawMode();
    
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        
        if (c == 'q') break;
        if (iscntrl(c)) {
        printf("%d\r\n", c);
        } else {
        printf("%d ('%c')\r\n", c, c);
        }
    }
    
    return 0;
}
