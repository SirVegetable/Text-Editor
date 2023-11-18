#include <iostream> 
#include <unistd.h>
#include <vector>
#include <termios.h>
#include <cstdlib>
#include <cctype>
#include <string> 

struct termios original_termios;
 

 void terminate(const char *s){
    std::perror(s);
    std::exit(1);

 }
 // function to disable the raw mode
void rawModeDisabled(){

    if(tcsetattr(STDERR_FILENO,TCSAFLUSH,&original_termios) == -1){
        terminate("tcsetattr");
    }
}
void rawModeEnabled(){
    if(tcgetattr(STDIN_FILENO,&original_termios)== -1){
        terminate("tcgetattr");
    }
    // causes specified func to be called when program terminates
    std::atexit(rawModeDisabled); 
    
    struct termios raw = original_termios;
    //turn off echo,canonical mode (ICANON) is an input flag, turn off software flow controls
    raw.c_lflag &= ~(ECHO | ICANON | ISIG |IEXTEN ); 
    raw.c_iflag &= ~( ICRNL | IXON | BRKINT | ISTRIP | INPCK);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1; 

    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw)== -1){
        terminate("tcsetattr");
    }
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
        if(std::iscntrl(array[i])){
            std::cout << static_cast<int>(array[i]) << "\n";
        }
        else{
            std::cout << array[i] << "\n";
        }
        
    }
    return 0;
}