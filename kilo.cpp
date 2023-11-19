/***Include Section***/

#include <iostream> 
#include <unistd.h>
#include <vector>
#include <termios.h>
#include <cstdlib>
#include <cctype>
#include <string>
#include <sys/ioctl.h>
#include <sstream>

/***Defines***/
// macro to each value of the ACSCII values - k = 1 is A
#define CTRL_KEY(k) ((k) & 0x1f)

/***Data***/

struct editorConfig{
    int screenrows;
    int screencols; 
    struct termios original_termios;

};

struct editorConfig E;


/***Terminal***/
 

 void terminate(const char *s){

    write(STDOUT_FILENO, "\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
    std::perror(s);
    std::exit(1);

 }
 // function to disable the raw mode
void rawModeDisabled(){

    if(tcsetattr(STDERR_FILENO,TCSAFLUSH,&E.original_termios) == -1){
        terminate("tcsetattr");
    }
}
void rawModeEnabled(){
    if(tcgetattr(STDIN_FILENO,&E.original_termios)== -1){
        terminate("tcgetattr");
    }
    // causes specified func to be called when program terminates
    std::atexit(rawModeDisabled); 
    
    struct termios raw = E.original_termios;
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
    for(int y = 0; y < E.screenrows; y++){
        write(STDOUT_FILENO, "~\r\n",3);
    }
}
int getCursorLocation(int *rows, int* cols){
    char buf[32];
    unsigned int i = 0 ; 
    if(write(STDOUT_FILENO, "\x1b[6n",4) != 4){
        return -1;
    }

    while( i < sizeof(buf) -1){
        if(read(STDIN_FILENO,&buf[i],i) != 1){
            break; 
        }
        if(buf[i] == 'R'){
            break; 
        }
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '['){
        return -1;
    }
    std::istringstream iss(&buf[2]);
    if(!(iss >> *rows >> *cols)|| iss.get() != ';'){
        return -1;
    }
    editorKeyRead();
    return -1;
    
}
int getWindowSize(int* rows, int* cols){
    struct winsize windsize;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ, &windsize)== -1 || windsize.ws_col == 0){
        if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B",12) != 12){
            return getCursorLocation(rows,cols);
        }
        editorKeyRead();
        return -1;
    }
    else{
        *cols = windsize.ws_col;
        *rows = windsize.ws_row;
        return 0; 
    }
}

void editorScreenRefresh(){
    write(STDOUT_FILENO, "\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
    editorDrawRows();
    write(STDOUT_FILENO,"\x1b[H",3);
}

/***Init***/
void initEditor(){
    if (getWindowSize(&E.screenrows,&E.screencols) == -1){
        terminate("getWindowSize");
    }
}
int main(){
    rawModeEnabled();
    initEditor();
    //reads 1 byte, in this case a character at a time for from the standard input stream into char c
    while(1){
        editorScreenRefresh();
        editorKeyProcessing();
    }

    return 0;
}