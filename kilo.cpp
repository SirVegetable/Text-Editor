#include <iostream> 
#include <unistd.h>
#include <vector>
#include <termios.h>

void rawModeEnabled(){
    struct termios raw;
    tcgetattr(STDIN_FILENO,&raw);
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