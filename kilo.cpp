/***Include Section***/

#include <iostream> 
#include <unistd.h>
#include <vector>
#include <termios.h>
#include <cstdlib>
#include <cctype>
#include <string>

/***Defines***/
// macro to each value of the ACSCII values - k = 1 is A
#define CTRL_KEY(k) ((k) & 0x1f)

/***Data***/

struct editorConfig{
    struct termios original_termios;

};

struct editorConfig E;


/***Terminal***/

struct termios original_termios;
 

 void terminate(const char *s){

    write(STDOUT_FILENO, "\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
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

char editorKeyRead(){
    int nread;
    char c;
    while((nread = read(STDIN_FILENO,&c, 1)) != 1){
        if(nread == -1 && errno != EAGAIN){
            terminate("read");
        }
    }
    return c; 
}

void editorKeyProcessing(){
    char c = editorKeyRead();
    switch(c){
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J",4);
            write(STDOUT_FILENO,"\x1b[H",3);
            exit(0);
            break;
    }
}
void editorDrawRows(){ 
    for(int y = 0; y < 24; y++){
        write(STDOUT_FILENO, "~\r\n",3);
    }
}
void editorScreenRefresh(){
    write(STDOUT_FILENO, "\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
    editorDrawRows();
    write(STDOUT_FILENO,"\x1b[H",3);
}

/***Init***/
int main(){
    rawModeEnabled();
    //reads 1 byte, in this case a character at a time for from the standard input stream into char c
    while(1){
        editorScreenRefresh();
        editorKeyProcessing();
    }

    return 0;
}