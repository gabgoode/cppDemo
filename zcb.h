#ifndef ZCB_H
#define ZCB_H

//zero coupon bond calculation

#include "fixdcf.h"

class zcb : public fixdcf
{
public:
    zcb();
    double DFConstruction();
    double CashflowCal();
};

#endif // ZCB_H
