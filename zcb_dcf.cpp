#include "zcb_dcf.h"

//This is the class that covers the subclass of zero-coupon bond dcf model
zcb_dcf::zcb_dcf()
{

}

QVector<QDate> zcb_dcf::genCashFlowDate(QDate SETDATE, QString pay_freq, QString pay_adjust_rule){
    //check whether SCHED_PAY_FREQ ="Z", which represents getting a one-time payment in the end
    QVector<QDate> vectorOfDays;
    if (pay_freq == "Z"){
        //Case 1: No need to adjust
        if (pay_adjust_rule == "U" || cal.isWorkDay(SETDATE)){
            vectorOfDays.append(SETDATE);
        }
        //Case 2: Not a business day, adjust to FOLLOWING business day
        else if(pay_adjust_rule == "F"){
            QDate payment_day = cal.getNextWorkDay(SETDATE);
            vectorOfDays.append(payment_day);
        }
        //Case 3: Not a business day, adjust to MODIFIED FOLLOWING business day
        else if(pay_adjust_rule == "MF"){
            QDate payment_day = cal.getNextWorkDay(SETDATE);
            if (payment_day.month() != SETDATE.month()){
                payment_day = cal.getPrevWorkDay(SETDATE);
            }
            vectorOfDays.append(payment_day);
        }
    }else{
        std::cout<<"This is not a zero-coupon bond";
    }
    std::cout << vectorOfDays.size();

    //for testing purposes
    for (int i=0; i<vectorOfDays.size(); i++){
        std::cout<<"*******************************";
        qInfo()<<i;
        qInfo()<<vectorOfDays.at(i).toString("yyyyMMdd");
    }

    return vectorOfDays;
}

//function that tests the output of test_genCashFlowDate
void zcb_dcf::test_genCashFlowDate(Calendar cal_input){
    std::cout << "ENTERING test";
    QDate SETDATE = QDate(2021,5,1);
    QString pay_freq = "Z";
    QString pay_adjust_rule = "F";
    cal = cal_input;
    genCashFlowDate(SETDATE,pay_freq,pay_adjust_rule);
}


QVector<float> zcb_dcf::genDiscountRate(QVector<Rate> Spot_Curve, QVector<QDate> vectorOfDays, QDate current_date){
    QVector<float> discount_rate;

    //sort the Spot_Curve in ASEC day_length
    std::sort(Spot_Curve.begin(),Spot_Curve.end());

    //add dummy values in the Spot_Curve vector in the beginning and end
    Spot_Curve.prepend(Rate{"0D",Spot_Curve.at(0).value,"MM","Continuously",0});
    Spot_Curve.append(Rate{"999Y",Spot_Curve.at(Spot_Curve.size()-1).value,"MM","Continuously",999*365});

    //for each payment day in vectorOfDays, find the discount rate
    int k=1;
    for (int i=0; i<vectorOfDays.size(); i++)
    {
        QDate date = vectorOfDays[i]; //  WE WANT： day1 付款日 day2
        QDate later_date = current_date.addDays(Spot_Curve[k].day_length); // initial value of day2 --> 2nd value of Spot_Curve
        while ((k<Spot_Curve.size()) && (later_date.daysTo(date)>=0))
        {
            k = k+1;
            later_date = current_date.addDays(Spot_Curve[k].day_length);
         }
        //QDate previous_date = current_date.addDays(Spot_Curve[k-1].day_length); --> day1

        float day1 = (float) Spot_Curve[k-1].day_length;
        float rate1 = Spot_Curve[k-1].value;
        float day2 = (float) Spot_Curve[k].day_length;
        float rate2 = Spot_Curve[k].value;
        discount_rate.append(utilities::linearinterpolate(day1, rate1, day2, rate2, (float) current_date.daysTo(date)));

        //for testing purposes
        qInfo() << discount_rate.last();
       } //linear time
    return discount_rate;
}

void zcb_dcf::test_genDiscontRate(){

    //set up Spot_Curve
    QVector<Rate> rf;
    rf.append(Rate{"1D",0.02,"MM","Continuously",1});
    rf.append(Rate{"1M",0.03,"MM","Continuously",30});
    rf.append(Rate{"3M",0.04,"MM","Continuously",90});
    rf.append(Rate{"6M",0.05,"MM","Continuously",180});
    rf.append(Rate{"1Y",0.06,"MM","Continuously",360});
    rf.append(Rate{"2Y",0.07,"MM","Continuously",720});
    rf.append(Rate{"5Y",0.08,"MM","Continuously",1800});

    //set up vectorOfDays and current_date

    QDate SETDATE = QDate(2021,5,30);
    QVector<QDate> vectorOfDays;
    vectorOfDays.append(SETDATE);

    QDate test_date_1 = QDate(2021,5,10);
    QDate test_date_2 = QDate(2021,5,15);
    QDate test_date_3 = QDate(2021,5,20);

    qInfo() << genDiscountRate(rf,vectorOfDays,test_date_1);
    qInfo() << genDiscountRate(rf,vectorOfDays,test_date_2);
    qInfo() << genDiscountRate(rf,vectorOfDays,test_date_3);

}

QVector<float> zcb_dcf::genDiscountFactor(QDate current_date, QVector<float> discount_rate, QVector<QDate> vectorOfDays, QString INT_ACOUNT_BASIS){
    //This function calculates 折现因子 countinous compounding =exp ( - 折现率% * 距离结算日的天数 / 计息基数分母)


    //距离结算日的天数
    qint64 daysTilPayDay = current_date.daysTo(vectorOfDays[0]);
    std::cout<<"daysTilPayDay"<<endl;
    qInfo()<<daysTilPayDay<<endl;


    //计息基数分母
    //日期这里可能有bug
    int denominator;
    if (INT_ACOUNT_BASIS == "A360"){
        denominator = 360;
    }else if(INT_ACOUNT_BASIS == "COUP"){
        denominator = 365;
    }else{ //ACT --> 365 or 366. Not sure how this value is determined.
        //The implementation here is based on determining whether the last and only payment date is in a leap year or not.
        QDate final = vectorOfDays.last();
        if(QDate::isLeapYear(final.year())){
            denominator = 366;
        }else{
            denominator = 365;
        }
    }
    std::cout<<"Denominator"<<endl;
    qInfo() << denominator<<endl;

    QVector<float> discount_factor;

    for(int i=0; i<vectorOfDays.size();++i){
      float df = exp(-discount_rate[i] * daysTilPayDay/denominator);
      std::cout<<"df"<<endl;
      qInfo() << df;
      discount_factor.append(df);
    }
    qInfo() << discount_factor;
    return discount_factor;
}

void zcb_dcf::test_genDiscountFactor(){
    QDate test_date_1 = QDate(2020,5,1);
    float discount_rate = 0.25;
    QVector<float> dr;
    dr.append(discount_rate);
    QVector<QDate> vectorOfDays;
    vectorOfDays.append(QDate(2020,5,31));
    QString INT_ACOUNT_BASIS = "ACT";
    genDiscountFactor(test_date_1,dr,vectorOfDays,INT_ACOUNT_BASIS);
}

QVector<float> zcb_dcf::genCashFlowAmt_Principal(float face_value_unit){
    //for Zero-coupon bond, notional in TB_BOND is the total value controlled by an obligation
    QVector<float> vectorOfPrincipal;
    vectorOfPrincipal.append(face_value_unit);
    return vectorOfPrincipal;
}

void zcb_dcf::test_genCashFlowAmt_Principal(){
    float face_value_unit  = 1;
    qInfo()<<zcb_dcf::genCashFlowAmt_Principal(face_value_unit);
}

QVector<float> zcb_dcf::genCashFlowAmt_Interest(float interest_unit=0){
     QVector<float> vectorOfInterest;
     vectorOfInterest.append(interest_unit);
     return vectorOfInterest;
}

//unit present value
float zcb_dcf::genPresentValue(QVector<float> cash_amt, QVector<float> interest_amt, QVector<float> discount_factor){
    if (cash_amt.size()!= discount_factor.size()){
        std::cout<<"There is a mismatch in the sizes of cashamt and df";
        return 0;
    }
    float pv = 0;
    for (int i=0;i<cash_amt.size();++i){
        float pv_pri = cash_amt[i]*discount_factor[i];
        pv = pv + pv_pri;
    }
    for (int i=0;i<interest_amt.size();++i){
        float pv_int = interest_amt[i]*discount_factor[i];
        pv = pv + pv_int;
    }
    return pv;
}

//total price = notional * unit present value
float zcb_dcf::genPrice(float Notioanl, float pv){
    return Notioanl * pv;
}


dcf::output zcb_dcf::pricing(input In){
    std::cout << "Entering Pricing in zcb_dcf";
    QVector<QDate> cashFlowDays;
    cashFlowDays = genCashFlowDate(In.end_date, In.pay_freq, In.pay_adjust_rule);

    dcf::output result;
    result.cashflow_date = cashFlowDays;
    return result;
}

