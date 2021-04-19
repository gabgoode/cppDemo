#include "vanilla_put.h"
#include <iostream>
using namespace std;

vanilla_put::vanilla_put()
{
    cout << "vanilla_put's constructor ";
}

double vanilla_put::genPrice(double S, double K, double r, double v, double Tmat)
{
    cout << "vanilla_put's genPrice ";
    return -S*bsm::normalCDF(-(bsm::d_j(1, S, K, r, v, Tmat)))+K*exp(-r*Tmat) * bsm::normalCDF(-(bsm::d_j(2, S, K, r, v, Tmat)));
}
