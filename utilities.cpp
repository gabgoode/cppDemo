#include "utilities.h"

utilities::utilities()
{

}

QVector<Rate> utilities::Ytm_to_Spot(QVector<Rate> term_structure, QString mode)
{
    qInfo()<<"Calling Ytm_to_Spot Function";

    //    qInfo()<<"============bef conversion========================";
    //    for(int i=0;i<term_structure.size();++i){
    //        qInfo()<<"Tenor: "<<term_structure.at(i).tenor
    //              <<" days len: "<<term_structure.at(i).day_length
    //             <<"rate type"<<term_structure.at(i).rate_type
    //            <<" rate val"<<term_structure.at(i).value;
    //    }



    //    QDate cur_date = QDateTime::currentDateTime().date();
    QDate cur_date(2020,10,30);
    int longest_year;
    //stub code, need further implementation to conver ytm to spot curve
    //find the longest tenor in years
    //layout the full tenor list according to the mode
    //interpolate the rate

    //declare a term_structure pointer for late value update
    Rate * p_ts = term_structure.data();

    //Composite is an internal data structure used to handle the conversion from par rate to spot rate.
    struct Composite{
        QString tenor; // the tenor key value;
        float value;//par value rate
        float pv; //present value
        float sumpv; //sum of present value
        float zrate; //spot rate
        float coupon_rate; //coupon rate
        float cf;
        float day_length;

        bool operator<(const Composite& other) const{
            return this->day_length<other.day_length;
        }

        //constructor method
        Composite(QString tenor, float curve_rate, float day_length){
            this->tenor=tenor;
            this->value=curve_rate;
            this->day_length= day_length;
            this->pv=0; // the date specifici discount present value
            this->sumpv=0; // the sum of discounted cash flow up until the date point there is zrate;
            this->cf=0; //cash flow on this date
            this->coupon_rate=0; //coupon rate
            this->zrate=0; // spot rate
        }

    };


    QVector<Composite> df;

    //copy the required data from term structure to df.
    for (int i=0;i<term_structure.size();++i){
        df.append(Composite(term_structure.at(i).tenor,term_structure.at(i).value,term_structure.at(i).day_length));
    }



    //get a pointer for the df
    Composite* p_comp = df.data();


    //sort the df by day_length
    std::sort(df.begin(),df.end());
    //this does assume the longest tenor are in integer years
    longest_year = static_cast<int>(std::round(df.at(df.size()-1).day_length/365));
    //    qInfo()<<"inside ytm_to_spot longest year"<<longest_year;

    //layout the full tenorlist according to the mode the code below did in quarterly basis
    if(mode=="Q"){
        for (int i=0;i<longest_year;++i){
            for(int j=0;j<4;++j){
                QString tenor_key;
                tenor_key = "Y"+QString::number(i)+"Q"+QString::number(j+1);
                int todays;
                todays = cur_date.daysTo(cur_date.addYears(i).addMonths((j+1)*3));
                float value=0;
                df.append(Composite(tenor_key,value,todays));
            }
        }
    }

    std::sort(df.begin(),df.end());
    p_comp=df.data();


    //assign par value with the same day_length
    for(int i=0;i<df.size();++i){
        for(int j=0;j<df.size();++j){
            if(i!=j && df.at(i).day_length == df.at(j).day_length){
                p_comp[i].value = std::max(df.at(i).value,df.at(j).value);
                p_comp[j].value = std::max(df.at(i).value,df.at(j).value);
            }
        }
    }




    //sort before rate interpolation

    int p_bef;
    int p_aft;


    std::sort(df.begin(),df.end());
    p_comp = df.data();
    //interpolate the curve rate
    for(int i=0;i<df.size();++i){
        if(df.at(i).value==0){

            if(i==0){
                for(int j=i+1;j<df.size();++j){
                    if(df.at(j).value!=0){
                        p_comp[i].value = df.at(j).value;
                        //                        qInfo()<<"finding j"<<j;
                        break;
                    }

                }
                //                qInfo()<<"Head";
            } //beginning of the list

            if(i==(df.size()-1)){
                for(int j = i-1;j>=0;j--){
                    if(df.at(j).value!=0){
                        p_comp[i].value = df.at(j).value;
                        break;
                    }

                }
                //                qInfo()<<"Tail";
            }//end of the list

            if(i>0 && i<(df.size()-1))
            {
                //                qInfo()<<"current i"<<i;
                for(int j=i-1;j>=0;j--)
                {
                    if(df.at(j).value!=0){
                        p_bef = j;
                        //                        qInfo()<<"finding node before"<<p_bef;
                        break;
                    }

                }

                for(int k=i+1;k<df.size();++k){
                    //                    qInfo()<<"iterating k"<<k;
                    if(df.at(k).value!=0){
                        p_aft = k;
                        //                        qInfo()<<"finding node after"<<p_aft;
                        break;
                    }

                }

                p_comp[i].value = ((df.at(i).day_length - df.at(p_bef).day_length)*df.at(p_aft).value + (df.at(p_aft).day_length - df.at(i).day_length)*df.at(p_bef).value)/(df.at(p_aft).day_length- df.at(p_bef).day_length);

            }//in between the tenor

        } // i where rate needs interpolating
    } // end of interpolating rates


    p_comp = df.data();
    //conver the continuously compounding curve rate into quarterly compounding equivalent rate
    for(int i=0;i<df.size();++i){
        if(mode=="Q" and df.at(i).tenor.contains("Q",Qt::CaseInsensitive)){
            p_comp[i].coupon_rate = utilities::rateCompConversion(df.at(i).value,"C","Q");
        }
    }

    //    qInfo()<<"after converting the rate from C to Q";


    int counter =0;
    p_comp = df.data();
    //core logic only applicable for mode="Q"
    for(int i=0;i<df.size();++i){
        if(df.at(i).coupon_rate!=0){
            p_comp[i].cf = df.at(i).coupon_rate*100/4; //coupon cash flow
            counter+=1;
            if(counter==1){
                //first quarter
                p_comp[i].zrate = -std::log(100/(df.at(i).cf+100))/(df.at(i).day_length/365);
                //                p_comp[i].sumpv = df.at(i).cf*std::exp(-df.at(i).zrate*(df.at(i).day_length/365));
            }else{
                //following quarters
                float sumpv=0;
                for(int k=i-1;k>=0;--k){
                    if(df.at(k).coupon_rate!=0){
                        sumpv += df.at(i).coupon_rate*25*std::exp(-df.at(k).zrate*df.at(k).day_length/365);
                    }
                }
                float cf=0;
                cf = df.at(i).coupon_rate*25+100;

                //                qInfo()<<"calling newtonMethod"
                //                      <<"coupon rate"<<df.at(i).coupon_rate
                //                     <<" tenor"<<df.at(i).tenor
                //                     <<" sumpv"<<sumpv
                //                    <<" cf"<<cf
                //                   <<" day length"<<df.at(i).day_length;
                p_comp[i].zrate = utilities::newtonMethod(df.at(i).coupon_rate,sumpv,cf,df.at(i).day_length/365,100,0.00001);
                //                qInfo()<<"finish newton method";
            }//end else
        }
    }


    //copy paste the zrate to other tenor of the same day length
    p_comp = df.data();
    for(int i=0;i<df.size();++i){
        for(int j=0;j<df.size();++j){
            if(i!=j && df.at(i).day_length == df.at(j).day_length){
                p_comp[i].zrate = std::max(df.at(i).zrate,df.at(j).zrate);
                p_comp[j].zrate = std::max(df.at(i).zrate,df.at(j).zrate);
            }
            //copy paste the curve value that is less than 3M as zrate
            if(df.at(i).day_length<=90 && df.at(i).zrate==0){
                p_comp[i].zrate = df.at(i).value;
            }
        }
    }

    //interpolating the spot rate
    p_comp = df.data();
    for(int i=0;i<df.size();++i){
        if(df.at(i).zrate==0){

            if(i==0){
                for(int j=i+1;j<df.size();++j){
                    if(df.at(j).zrate!=0){
                        p_comp[i].zrate = df.at(j).zrate;
                        //                        qInfo()<<"finding j"<<j;
                        break;
                    }
                }
                //                qInfo()<<"Head";
            } //beginning of the list

            if(i==(df.size()-1)){
                for(int j = i-1;j>=0;j--){
                    if(df.at(j).zrate!=0){
                        p_comp[i].zrate = df.at(j).zrate;
                        break;
                    }
                }
                //                qInfo()<<"Tail";
            }//end of the list

            if(i>0 && i<(df.size()-1))
            {
                //                qInfo()<<"current i"<<i;
                for(int j=i-1;j>=0;j--)
                {
                    if(df.at(j).zrate!=0){
                        p_bef = j;
                        //                        qInfo()<<"finding node before"<<p_bef;
                        break;
                    }
                }

                for(int k=i+1;k<df.size();++k){
                    //                    qInfo()<<"iterating k"<<k;
                    if(df.at(k).zrate!=0){
                        p_aft = k;
                        //                        qInfo()<<"finding node after"<<p_aft;
                        break;
                    }
                }
                p_comp[i].zrate = ((df.at(i).day_length - df.at(p_bef).day_length)*df.at(p_aft).zrate + (df.at(p_aft).day_length - df.at(i).day_length)*df.at(p_bef).zrate)/(df.at(p_aft).day_length- df.at(p_bef).day_length);
            }//in between the tenor
        } // i where rate needs interpolating
    } // end of interpolating rates


    //assign the spot to the term structure
    for(int i=0;i<term_structure.size();++i){
        for(int j=0;j<df.size();++j){
            if(term_structure.at(i).day_length==df.at(j).day_length)
            {
                p_ts[i].value = df.at(j).zrate;
                p_ts[i].rate_type = "SPOT";
                break;
            }
        }

    }

    //    qInfo()<<"after conversion";
    //    for(int i=0;i<term_structure.size();++i){
    //        qInfo()<<"Tenor: "<<term_structure.at(i).tenor
    //              <<" days len: "<<term_structure.at(i).day_length
    //             <<"rate type"<<term_structure.at(i).rate_type
    //            <<" rate val"<<term_structure.at(i).value;
    //    }


    //clear the pointer

    return term_structure;
}

QVector<Rate> utilities::Spot_to_Fwd(QVector<Rate> fxcurve, float fxrate, float spread)
{
    QVector<Rate> term_structure;
    for (int i=0;i<fxcurve.size();++i)
    {
        float forward_rate = fxrate + fxcurve.at(i).value / spread;
        term_structure.append(Rate(fxcurve.at(i).tenor,forward_rate,"","",fxcurve.at(i).day_length));
    }
    return term_structure;
}

QVector<Rate> utilities::Fwd_to_Implied(QVector<Rate> fwd_curve, QVector<Rate> term_structure_spot, float fxrate)
{
    // sort fx curve by day length
    std::sort(fwd_curve.begin(),fwd_curve.end());


    // generate dummy points on the spot rate curve
    term_structure_spot.prepend(Rate("0D",term_structure_spot[0].value,"DF","C",0));
    term_structure_spot.append(Rate("9999Y",term_structure_spot.at(term_structure_spot.size()-1).value,"DF","C",9999*365));

    QVector<Rate> term_structure_byFXtenor;
    for (int i=0;i<fwd_curve.size();++i)
    {
        Rate fx = fwd_curve[i];
        int j;
        for (j=0;j<term_structure_spot.size()-1;++j)
        {
            if (fx.day_length<term_structure_spot[j+1].day_length) {break;}
        }
        float interpolate_value = (fx.day_length-term_structure_spot[j].day_length) * (term_structure_spot[j+1].value-term_structure_spot[j].value) /(term_structure_spot[j+1].day_length-term_structure_spot[j].day_length);
        term_structure_byFXtenor.append(Rate(fx.tenor,interpolate_value,"DF","C",fx.day_length));
    }

    // generate discount factor for base currency
    QVector<Rate> discount_factor;
    for (int i=0;i<term_structure_byFXtenor.size();++i)
    {
        Rate now = term_structure_byFXtenor[i];
        float df = std::exp(-now.value*now.day_length/365);
        discount_factor.append(Rate(now.tenor,df,"DF","C",now.day_length));
    }

    QVector<Rate> result;
    for (int i=0;i<term_structure_byFXtenor.size();++i)
    {
        Rate now = term_structure_byFXtenor[i];
        float df = discount_factor.at(i).value * fxrate / fwd_curve.at(i).value;
        result.append(Rate(now.tenor,df,"DF","C",now.day_length));
    }

    return result;
}


float utilities::rateCompConversion(float input, QString compounding_from, QString compounding_to)
{
    float num_to=-1;
    float fv = 0.0;
    //    qInfo()<<"input value is:"<<input;
    // Bi-Monthly B; Monthly M; Quarterly Q; Semi-Annual S, Annual A; Continuously C;
    if(compounding_from == compounding_to){
        return input;
    }
    else{

        if(compounding_from=='A'){

            fv = 1+input;
        }
        if(compounding_from=='S'){

            fv =std::pow((1+input*0.5),2);
        }
        if(compounding_from=='Q'){

            fv =std::pow((1+input*0.25),4);
        }
        if(compounding_from=='M'){

            fv =std::pow((1+input/12),12);
        }
        if(compounding_from=='B'){

            fv =std::pow((1+input/24),24);
        }

        if(compounding_from=='C'){

            fv =std::exp(input);
        }

        //        qInfo()<<"FV value is "<<fv;

        if(compounding_to=='A'){
            num_to=1;
            return fv-1;
        }

        if(compounding_to=='S')
        {
            num_to=2;
            float temp;
            temp = (std::sqrt(fv)-1)*2;
            return temp;
        }

        if(compounding_to=='Q'){
            num_to=4;
            float temp;
            //            qInfo()<<"dddd"<<std::pow(fv,0.25);
            temp = (std::pow(fv,0.25)-1)*num_to;
            //            qInfo()<< "to Q"<<temp;
            return temp;
        }

        if(compounding_to=='M'){
            num_to=12;
            float temp;
            float expo = 1/num_to;
            //            qInfo()<<"expo"<<expo;
            temp = (std::pow(fv,expo)-1)*num_to;
            //            qInfo()<< "to M"<<temp;
            return temp;
        }
        if(compounding_to=='B'){
            num_to=24;
            float temp;
            temp = (std::pow(fv,1/num_to)-1)*num_to;
            //            qInfo()<< "to B"<<temp;
            return temp;
        }
        if(compounding_to=='C'){
            num_to=0;
            return std::log(fv);
        }

    }
    return input;
}

float utilities::newtonMethod(float rate_guess, float sumpv, float cf, float t, float base, float convergence_th)
{


    float y;
    float y_prime;
    float x_next;

    y = sumpv + cf*std::exp(-rate_guess*t)-base;
    y_prime = -t*cf*std::exp(-rate_guess*t);
    x_next = rate_guess - y/y_prime;

    if(std::abs(x_next - rate_guess) <= convergence_th){
        return x_next;
    }else
    {
        return newtonMethod(x_next,sumpv,cf,t,base,convergence_th);
    }

}



QVector<FxVolDelta> utilities::quote_to_delta(QVector<FxVolQuote> quote)
{
    QVector<FxVolDelta> delta;
    if (quote.size() != 5) {return delta;}
    std::sort(quote.begin(), quote.end());
    float atm, bf10, rr10, bf25, rr25;
    atm = quote[0].value;
    bf10 = quote[1].value;
    rr10 = quote[2].value;
    bf25 = quote[3].value;
    rr25 = quote[4].value;
    FxVolDelta delta_sub;
    delta_sub.term = quote[0].term;
    for (int i=0; i<=3; i++) delta.append(delta_sub);
    delta[0].delta_lvl = bf10 + 0.5 * (atm + rr10);  // 10 call
    delta[1].delta_lvl = bf10 + 0.5 * (atm - rr10);  // 10 put
    delta[2].delta_lvl = bf25 + 0.5 * (atm + rr25);  // 25 call
    delta[3].delta_lvl = bf25 + 0.5 * (atm - rr25);  // 25 put
    return delta;
}

RF_ISR utilities::genRiskFactor(BondCurve curve)
{
    RF_ISR rf;
    rf.ccy = curve.ccy;
    rf.data = Ytm_to_Spot(curve.data, "");
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    rf.index_id = curve.index_id;
    return rf;
}

RF_RFISR utilities::genRiskFactor(BondRFCurve curve)
{
    RF_RFISR rf;
    rf.ccy = curve.ccy;
    rf.data = Ytm_to_Spot(curve.data, "");
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    rf.index_id = curve.index_id;
    return rf;
}

RF_CSISR utilities::genRiskFactor(BondCSCurve curve)
{
    RF_CSISR rf;
    rf.ccy = curve.ccy;
    rf.data = Ytm_to_Spot(curve.data, "");
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    rf.index_id = curve.index_id;
    return rf;
}

RF_IIR utilities::genRiskFactor(ComSpotCurve curve, QVector<Rate> YTM, float fxrate)
{
    RF_IIR rf;
    rf.ccy1 = curve.ccy1;
    rf.ccy2 = curve.ccy2;
    rf.data = Fwd_to_Implied(Spot_to_Fwd(curve.data, fxrate, 0), YTM, fxrate);
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    rf.index_id = curve.index_id;
    return rf;
}

RF_IIR utilities::genRiskFactor(ComFwdCurve curve, QVector<Rate> YTM, float fxrate)
{
    RF_IIR rf;
    rf.ccy1 = curve.ccy1;
    rf.ccy2 = curve.ccy2;
    rf.data = Fwd_to_Implied(Spot_to_Fwd(curve.data, fxrate, 0), YTM, fxrate);
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    rf.index_id = curve.index_id;
    return rf;
}

RF_IIR utilities::genRiskFactor(FxFwdCurve curve, QVector<Rate> YTM, float fxrate)
{
    RF_IIR rf;
    rf.ccy1 = curve.ccy1;
    rf.ccy2 = curve.ccy2;
    rf.data = Fwd_to_Implied(Spot_to_Fwd(curve.data, fxrate, 0), YTM, fxrate);
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    rf.index_id = curve.index_id;
    return rf;
}

RF_FXS utilities::genRiskFactor(FxRate curve)
{
    RF_FXS rf;
    rf.ccy1 = curve.ccy1;
    rf.ccy2 = curve.ccy2;
    rf.data = curve.data;
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    return rf;
}

RF_FXIR utilities::genRiskFactor(FXIRRate curve)
{
    RF_FXIR rf;
    rf.data = curve.data;
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    rf.index_id = curve.index_id;
    return rf;
}

RF_FXVOL utilities::genRiskFactor(FxVolData curve)
{
    RF_FXVOL rf;
    rf.ccy1 = curve.ccy1;
    rf.ccy2 = curve.ccy2;
    rf.data = quote_to_delta(curve.data);
    rf.date = curve.date;
    rf.hash = curve.hash;
    rf.name = curve.name;
    return rf;
}

float utilities::linearinterpolate(float day1, float rate1, float day2, float rate2, float day_interpolation){
    float rate = ((day_interpolation-day2)/(day1-day2))*rate1+((day_interpolation-day1)/(day2-day1))*rate2;
    return rate;
}














