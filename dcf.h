#ifndef DCF_H
#define DCF_H

#include <QObject>
#include"global.h"
#include"tenorconfiger.h"
#include"datamanager.h"
#include"utilities.h"
#include"calendar.h"


class dcf
{
public:
    dcf();

    struct input{
        QString pay_freq, pay_adjust_rule;
        QString interest_basis;
        float coupon;
        QVector<Rate> curve;
        QDate start_date, end_date, current_date;
        Calendar cal;
    };

    struct output{
        QVector<QDate> cashflow_date;
        QVector<float> cashflow_amt;
        QVector<Rate> discount_curve;
        QVector<float> discount_factor;
        QVector<float> present_value;
        float price;
    };

    void Process_Input(input In);

    output pricing(input IN);

    QVector<QDate> genCashFlowDate(QDate SETDATE, QString pay_freq, QString pay_adjust_rule); //付息的日期， 比如 2020/4/1 2020/7/1 2020/10/1 2021/1/1
    QVector<float> genCashFlowAmt_Principal(float face_value_unit);//每一期付息的amount, as a vector <0.01, 0.01, 0.01, 1.001>
    QVector<float> genCashFlowAmt_Interest(float interest_unit);
    QVector<float> genDiscountRate(QVector<Rate> Spot_Curve, QVector<QDate> vectorOfDays, QDate current_date);
    QVector<float> genDiscountFactor(QDate current_date, QVector<float> discount_factor, QVector<QDate> vectorOfDays, QString INT_ACOUNT_BASIS); //present value of the bond
    float genPresentValue(QVector<float> cash_amt, QVector<float> interest_amt, QVector<float> df);
    float genPrice(float Notional, float pv);


protected:
    Calendar cal;

private:
    output result;
};

#endif // DCF_H
