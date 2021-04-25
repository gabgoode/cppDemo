#ifndef DCF_H
#define DCF_H
#include <iostream>
using namespace std;

class dcf
{
public:
    dcf();

    //virtual functions that need to be implemented in derived classes
    virtual double PriceCal()=0;//function that returns the price
};

#endif // DCF_H
