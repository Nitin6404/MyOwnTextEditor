#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);  // Restore original terminal settings
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);  // Save original terminal settings
    atexit(disableRawMode);  // Register disableRawMode to be called on exit

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);  // Disable canonical mode and echoing
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);  // Apply the new settings
}

int main() {
    enableRawMode();
    
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        printf("%c", c);
        fflush(stdout);  // Force the output to be flushed immediately
    }
    
    return 0;
}
