#ifndef BSM_H
#define BSM_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
class bsm
{
public:
    bsm();

    //functions to get parameters used for BSM

    //NOTE TO MYSELF: NOT SURE HOW TO DEAL WITH DATE HERE, NEED TO ASK GLORIA

    /*
    virtual  float genT_set(QDate sys_date) = 0; //Tset是期权的结算期限，以年为单位；
    virtual  float genT_mat(QDate sys_date) = 0; //Tmat是期权的剩余期限，以年为单位；
    virtual double genVol() = 0; //generate volatility
    */

    // parameters used for BSM
    static double normalCDF(double x); //generate N(x), cumulative distribution function
    static double d_j(int i, double S, double K, double r, double v, double Tmat); //generate d1 and d2

    //Functions below are inplemented in subclasses


    virtual double genPrice(double S, double K, double r, double v, double Tmat) = 0;
    /*
    virtual float genDelta() = 0 ;
    virtual float genVega() = 0;
    virtual float genTheta() = 0;
    virtual float genRho() = 0;
    */

private:
    // The parameter list
    // *****NOTE*******: THESE ARE HARDCODED VALUES; NEED TO BE DYNAMIC IN THE FUTURE
    double S = 100.0;  // Underlying price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate
    double v = 0.2;    // Volatility of the underlying
    double Tset = 1.0;   // One year until expiry; settlement date
    double Tmat = 1.0;   // One year until expiry; maturity date
};

#endif // BSM_H
