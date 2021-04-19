#include "vanilla_call.h"
#include <iostream>
using namespace std;

vanilla_call::vanilla_call()
{
    cout << "vanilla_call's constructor ";
}

double vanilla_call::genPrice(double S, double K, double r, double v, double Tmat)
{
    cout << "vanilla_call's genPrice ";
    return S * bsm::normalCDF(bsm::d_j(1, S, K, r, v, Tmat))-K*exp(-r*Tmat) * bsm::normalCDF(bsm::d_j(2, S, K, r, v, Tmat));
}
