#include "bsm.h"

/*
 Mostly an interface:
To implement some helper functions in order to get the key parameters for a vinilla BSM model
*/
bsm::bsm()
{

}

// This is the normal cumulative distribution function
// Since the gaussian error function and its complementary function is
// related to the normal cumulative distribution function,
// we can use the implemented c-function erfc (complementary error function).
// erfc(x) = 1-erf(x) with M_SQRT1_2 = √0,5
// normalCDF(x) = 0.5 * (1+erf(x/√0,5)
double bsm:: normalCDF(double x)
{
   return 0.5 * erfc(-x * M_SQRT1_2);
}

double bsm:: d_j(int j, double S, double K, double r, double v, double Tmat)
{
    return (log(S/K) + (r + (pow(-1,j-1))*0.5*v*v)*Tmat)/(v*(pow(Tmat,0.5)));
};
