#include <iostream> 
#include <unistd.h>
#include <vector>
#include <termios.h>
#include <cstdlib>

struct termios original_termios;

void rawModeDisabled(){
    tcsetattr(STDERR_FILENO,TCSAFLUSH,&original_termios);
}
void rawModeEnabled(){
    tcgetattr(STDIN_FILENO,&original_termios);
    std::atexit(rawModeDisabled);
    
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ECHO); //turn off echo
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw);
}
int main(){
    rawModeEnabled();
    std::vector<char> array; 
    char c;
    //reads 1 byte, in this case a character at a time for from the standard input stream into char c
    while(read(STDIN_FILENO,&c,1)==1 && c != 'q'){
        array.push_back(c);
        
    }
    for(std::vector<char>::size_type i=0; i< array.size(); i++){
        std::cout << array[i];
    }
    return 0;
}