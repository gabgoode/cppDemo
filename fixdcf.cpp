#include "fixdcf.h"

fixdcf::fixdcf()
{

}

//PriceCal： 全价 = 最后一期现金流 * 折现因子
double fixdcf::PriceCal(BondRef bondref){
    double price; // the returned value
    QString type;

    //Process data passed in from bondref
    QDate maturity_date = bondref.maturity_date;
    QString day_count_basis = bondref.day_count_basis;
    double cashflow = bondref.notional; //I_SUM_TD_BOND_NOTIONAL
    QString rf_name = bondref.rf_name;  // I_SUM_RD_SASSET_SUBINDEX_折现曲线ID； 用于找到用哪条折现曲线


    //Determine which subtype the bond is and compute respectively

    //1):Zero Coupon Bond
    if(bondref.payment_freq=="Z" && bondref.coupon_rate == 0){
        type = "Zero Coupon Bond";
        cout << "We are calcuating the price for a  "<< type.toStdString() << endl;

        //calculate spot_rate： rf_name-->market data (yield to maturity)-->spot_rate

        //NOTE!!! what is term_structure?? market data??
        QVector spot_rate_vector = utilities::Ytm_to_Spot(term_structure, rf_name);
        //插值算点spot_rate
        //float spot_rate = linear(spot_rate_vector);

        double discount_factor = DFConstruction(spot_rate, maturity_date, day_count_basis);
        price = cashflow * discount_factor;
    }


    cout << "The Price of this" << type.toStdString() << "is：" << price <<endl;
    return price;
}

/************Helper functions to get the parameters that we want to plug in*********/

//DFConstruction: 计算折现因子
//Params:
//折现率 spot_rate： subindex-->market data (yield to maturity)-->spot
//结算日 maturity_date
//计息基数分母 day_count_basis

double DFConstruction(float spot_rate, QDate maturity_date , QString day_count_basis){
    //Step1： Convert day_count_basis to numerical value
    int day_count_basis_num;
    if (day_count_basis == "COUP"){
        day_count_basis_num = 365;
    }else if (day_count_basis == "A360"){
        day_count_basis_num = 360;
    }else{
        //Can probably use QDate's isLeapYear() fxn
        day_count_basis_num = 366; //this is an error, not sure how ACT is indicated in data. In reality, ACT = 365 or 366.
    }

    //Step2: Calculate the date til maturity
    QDate today = QDate::currentDate();
    qint64 daysTilMaturity = today.daysTo(maturity_date);

    //Step3: Compute the discount factor
    double df = exp(-spot_rate*daysTilMaturity/day_count_basis_num);

    return df;
}
