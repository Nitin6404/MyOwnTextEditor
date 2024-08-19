#include <stdio.h>
#include <unistd.h>


int main(){
    char c;
    printf("Hello there this is me, Nitin!!!!");
    while(read(STDIN_FILENO, &c, 1) == 1);
    return 0;
}
