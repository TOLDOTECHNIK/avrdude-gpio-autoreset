// History
// 2016-01-25	init
//              compile with: g++ -std=c++11 ./includes/GPIOClass.cpp avrdude-gpio-autoreset.cpp -avrdude-gpio-autoreset
//              -std=c++11 is required for <regex> and takes longer to compile.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <regex>
#include "./includes/GPIOClass.h"


#define RESET_PIN        "4"                //Pin which do the reset, GPIOx, (double quotes required)
#define TIMEOUT          5.0                //program quits after x seconds
#define CLOCK_CORRECTION 10.0               //is required because of using usleep
#define SEARCH_PATTERN   ".+TIOCM_DTR.+"    //if found in inputLine, reset pin gets triggered

using namespace std;

//function declarations
int getch(void);
void reset();

//reset pin
GPIOClass* gpio = new GPIOClass(RESET_PIN);

//main function
int main(int argc, char *argv[]){
    //console input line
    string inputLine;

    //remember current time
    const clock_t begin_time = clock();

    while (1){
        //check timeout
        if(float(clock() - begin_time) / CLOCKS_PER_SEC * CLOCK_CORRECTION > TIMEOUT){
            printf("autoreset timed out.\n");
            return 0;
        }
        //handle console input
        int chr;
        while((chr = getch()) > -1 ){
            if(chr == 10){      //check for new line
                //printf("Line: %s", inputLine.c_str());
                if(regex_match(inputLine, regex(SEARCH_PATTERN))){
                    reset();
                    return 0;
                }
                inputLine = "";
            }
            else{
                inputLine += chr;
            }
        }
        //save cpu
        usleep(1000);	//1ms
    }
    return 0;
}

//sends a reset to gpio
void reset(){
    gpio->export_gpio();
    gpio->setdir_gpio("out");
    gpio->setval_gpio("1");
    usleep(120000);
    gpio->setval_gpio("0");
    gpio->unexport_gpio();
    printf("Reset sent.");
}

//read console character
int getch(void){
    int ch;
    struct termios oldt, newt;
    long oldf, newf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    newf = oldf | O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, newf);
    ch = getchar();
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
