#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include "global.h"
#include <QVector>
#include <QString>
#include <QDebug>
#include <QFile>
#include <map>
#include <QStringList>
#include <algorithm>
#include <QtMath>
#include <QCryptographicHash>

class utilities
{
public:
    utilities();

private:


public:
    // Curve change;
    static QVector<Rate> Ytm_to_Spot(QVector<Rate> term_structure, QString mode);
    static QVector<Rate> Fwd_to_Implied(QVector<Rate> fwd_curve, QVector<Rate> term_structure, float fxrate);
    static QVector<Rate> rrbf_to_volsur();
    static QVector<Rate> Spot_to_Fwd(QVector<Rate> fxcurve, float fxrate, float spread);

    // quotation change
    static QVector<FxVolDelta> quote_to_delta(QVector<FxVolQuote>);


    // math method
    static float rateCompConversion(float input,QString compounding_from, QString compounding_to);
    static float newtonMethod(float rate_guess, float sumpv, float cf,float t, float base, float convergence_th);
    static float linearinterpolate(float day1, float rate1, float day2, float rate2, float day_interpolatoin);



    // generate risk factor for market data
    static RF_ISR genRiskFactor(BondCurve curve);
    static RF_RFISR genRiskFactor(BondRFCurve curve);
    static RF_CSISR genRiskFactor(BondCSCurve curve);
    static RF_IIR genRiskFactor(FxFwdCurve curve, QVector<Rate> YTM, float fxrate);
    static RF_IIR genRiskFactor(ComFwdCurve curve, QVector<Rate> YTM, float fxrate);
    static RF_IIR genRiskFactor(ComSpotCurve curve, QVector<Rate> YTM, float fxrate);
    static RF_FXS genRiskFactor(FxRate curve);
    static RF_FXIR genRiskFactor(FXIRRate curve);
    static RF_FXVOL genRiskFactor(FxVolData curve);


};

#endif // UTILITIES_H
