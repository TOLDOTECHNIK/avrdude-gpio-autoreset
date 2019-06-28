// History
// 2016-01-25	init
//              compile with: g++ -std=c++11 ./includes/GPIOClass.cpp avrdude-gpio-autoreset.cpp -avrdude-gpio-autoreset
//              -std=c++11 is required for <regex> and takes longer to compile.
// 2018-01-17   GPIOClass reimplementation
// 2019-06-28   code cleanup

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <iostream>
#include <string>
#include <regex>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "./includes/GPIOClass.h"

#define RESET_PIN                       4                  //Pin which do the reset, GPIOx
#define TIMEOUT                         5.0                //program quits after x seconds
#define CLOCK_CORRECTION                10.0               //is required because of using usleep
#define AVRDUDE_OUTPUT_SEARCH_PATTERN   ".+TIOCM_DTR.+"    //if found in avrdude output (inputLine), reset pin gets triggered

using namespace std;

//function declarations
int getch(void);
void reset();

//reset pin
GPIOClass* gpio = new GPIOClass();

//main function
int main(int argc, char *argv[]){

    //console input line
    string inputLine;

    //remember current time
    const clock_t begin_time = clock();

    printf("waiting for avrdude reset signal...\n");

    while (1){
        //check timeout
        if(float(clock() - begin_time) / CLOCKS_PER_SEC * CLOCK_CORRECTION > TIMEOUT){
            printf("avrdude-gpio-autoreset timed out.\n");
            return 0;
        }
        //handle console input
        int chr;
        while((chr = getch()) > -1 ){
            if(chr == 10){      //check for new line
                //printf("Line: %s", inputLine.c_str());
                if(regex_match(inputLine, regex(AVRDUDE_OUTPUT_SEARCH_PATTERN))){
                    //printf("========== DO RESET ==========\n");
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
    gpio->GPIOExport(RESET_PIN);
    gpio->GPIODirection(RESET_PIN, 1);
    gpio->GPIOWrite(RESET_PIN, 1);
    usleep(120000);
    gpio->GPIOWrite(RESET_PIN, 0);
    gpio->GPIOUnexport(RESET_PIN);
    printf("Reset on GPIO done.");
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
