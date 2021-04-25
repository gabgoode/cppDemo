#ifndef FIXDCF_H
#define FIXDCF_H

#include "dcf.h"
#include <chrono>
#include <ctime>
#include <QDate>
#include <math.h>
#include <utilities.h>

class fixdcf : public dcf
{
public:
    fixdcf();

    double PriceCal(); //计算债券价格（全价）
    double DFConstruction(float coupon_rate, QDate maturity_date, QString day_count_basis); //构建折现因子
    double CashflowCal();//计算各期现金流（折现前）


};

#endif // FIXDCF_H
