#include <iostream>
#include "mainwindow.h"
#include "bsm.h"
#include "vanilla_put.h"
#include "vanilla_call.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    /*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();*/

    double S = 100.0;  // Underlying price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate
    double v = 0.2;    // Volatility of the underlying
    double Tset = 1.0;   // One year until expiry; settlement date
    double Tmat = 1.0;   // One year until expiry; maturity date

    vanilla_put put;
    vanilla_call call;

    // Then we calculate the call/put values
    //double call_price = call.vanilla_call::genPrice(S, K, r, v, Tmat);
    //double put_price = put.vanilla_put::genPrice(S, K, r, v, Tmat);

    double call_price = call.genPrice(S, K, r, v, Tmat);
    double put_price = put.genPrice(S, K, r, v, Tmat);

    // Finally we output the parameters and prices
    std::cout << "Underlying:      " << S << std::endl;
    std::cout << "Strike:          " << K << std::endl;
    std::cout << "Risk-Free Rate:  " << r << std::endl;
    std::cout << "Volatility:      " << v << std::endl;
    std::cout << "Maturity:        " << Tmat << std::endl;

    std::cout << "Call Price:      " << call_price << std::endl;
    std::cout << "Put Price:       " << put_price << std::endl;
}
