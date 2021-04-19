#ifndef VANILLA_PUT_H
#define VANILLA_PUT_H

#include "bsm.h"

class vanilla_put : virtual public bsm
{
public:
    vanilla_put();

    double genPrice(double S, double K, double r, double v, double Tmat);

};

#endif // VANILLA_PUT_H

