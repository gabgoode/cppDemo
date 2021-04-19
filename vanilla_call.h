#ifndef VANILLA_CALL_H
#define VANILLA_CALL_H

#include "bsm.h"

class vanilla_call : virtual public bsm
{
public:
    vanilla_call();

    double genPrice(double S, double K, double r, double v, double Tmat);
};

#endif // VANILLA_CALL_H
