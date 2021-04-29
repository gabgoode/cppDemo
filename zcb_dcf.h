#ifndef ZCB_DCF_H
#define ZCB_DCF_H


#include "dcf.h"
#include "utilities.h"
#include<iostream>
#include<QObject>
#include<QVector>
#include<QDate>
#include <QDebug>
#include "global.h"

class zcb_dcf : public dcf
{
public:
    zcb_dcf();

    QVector<QDate> genCashFlowDate(QDate SETDATE, QString pay_freq, QString pay_adjust_rule);  //zcb只有一个到期日付息
    void test_genCashFlowDate(Calendar cal_input);
    QVector<float> genDiscountRate(QVector<Rate> Spot_Curve, QVector<QDate> vectorOfDays, QDate current_date);
    void test_genDiscontRate();
    QVector<float> genDiscountFactor(QDate current_date, QVector<float> discount_rate, QVector<QDate> vectorOfDays, QString INT_ACOUNT_BASIS); //only principal, not coupon
    void test_genDiscountFactor();
    QVector<float> genCashFlowAmt_Principal(float face_value_unit);
    void test_genCashFlowAmt_Principal();
    QVector<float> genCashFlowAmt_Interest(float interest_unit);
    float genPresentValue(QVector<float> cash_amt, QVector<float> interest_amt, QVector<float> df);
    float genPrice(float Notioanl, float pv);//final price =  sum of present value for each period

    output pricing(input IN);

private:
    output result;
};

#endif // ZCB_DCF_H
