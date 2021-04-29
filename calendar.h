#ifndef CALENDAR_H
#define CALENDAR_H

#include <QObject>
#include "global.h"
#include <cmath>
#include <QFile>
#include <QCalendar>
#include <QDebug>
class Calendar
{

public:
    explicit Calendar();
    Calendar(QString cal_id,int day_length,QDate sys_date);
    ~Calendar();
    void updateCalendarDay(QString cal_id, QDate date, bool flag);
    bool isWorkDay(QDate qd); //check whether the input date is a workday
    void addCalendarDay(QDate qd, bool workday_flag); //add a date and specify the workday or non-workday
    void addCalendarDay(QDate qd); //add a date without specifying the workday or on-workday
    qint64 getCalendarDays();
    QString getCALId();
    QDate offsetCalendarWorkDay(QDate qd, int offset_day);
    void toggleWorkDay(QDate qd);
    QDate getNextWorkDay(QDate qd); //get the next workday of the current date
    QDate getPrevWorkDay(QDate qd);
    bool loadCalendarFile(QString filepath);


signals:

public:
    QVector<CalendarDay> cal_days;
    QString cal_id;
    QCalendar qcal;
    CalendarDay cday;

};

#endif // CALENDAR_H
