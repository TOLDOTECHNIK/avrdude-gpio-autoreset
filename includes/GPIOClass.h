#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H

#include <string>
using namespace std;
/* GPIO Class
 * Purpose: Each object instantiated from this class will control a GPIO pin
 * The GPIO pin number must be passed to the overloaded class constructor
 */
class GPIOClass
{
public:
    GPIOClass();
    int GPIOExport(int pin);
    int GPIOUnexport(int pin);
    int GPIODirection(int pin, int dir);
    int GPIORead(int pin);
    int GPIOWrite(int pin, int value);
private:

};

#endif
