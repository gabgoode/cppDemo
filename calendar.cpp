#include "calendar.h"


Calendar::Calendar()
{
    qInfo()<<"Default Constructor called";
}



Calendar::Calendar(QString cal_id, int day_length, QDate sys_date)
{

    qInfo()<<"Calendar Constructor called";
    this->qcal = QCalendar(QCalendar::System::Gregorian);
    this->cal_id=cal_id;
    for (int i=0;i<day_length;++i){
        if (i==0){
            //CalendarDay cday;
            this->cday.date =sys_date;

            if (this->qcal.dayOfWeek(sys_date)>=6){
                this->cday.Workday_Flag=false;
            } else{
                this->cday.Workday_Flag=true;
            }
            this->cal_days.append(cday);

        }//system date initialisation
        else{
            QDate qd = sys_date.addDays(i);
            cday.date=qd;
            if (this->qcal.dayOfWeek(qd)>=6){
                this->cday.Workday_Flag=false;
            } else{
                this->cday.Workday_Flag=true;
            }
            this->cal_days.append(cday);
        }
    }}

Calendar::~Calendar()
{
    qInfo()<<"Calendar with id"<<this->cal_id<<" is destoryed";
}

bool Calendar::loadCalendarFile(QString filepath)
{
    QStringList linelist={};
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);

    while (!in.atEnd()) {

        QString line = in.readLine();          // code here to process the data
        line.remove("\"",Qt::CaseSensitive);
        linelist = line.split(",",Qt::KeepEmptyParts);     // Reading line

        if (linelist[1] != "Date")
        {
            QString cal_id = linelist[0];
            QDate date = QDate::fromString(linelist[1],"yyyyMMdd");
            bool workday_flag;
            if ((linelist[2]=="true")) workday_flag = true;
            else workday_flag = false;
            updateCalendarDay(cal_id, date, workday_flag);
        }


    } // inside the while loop for file processing

    return true;
}

bool Calendar::isWorkDay(QDate qd)
{
    if (qd<this->cal_days.at(0).date){
        throw std::runtime_error("Date entered not maintained in the Calendar");
    }

    for (int i = 0; i < this->cal_days.size(); ++i) {
        if (this->cal_days.at(i).date == qd)
        {

            if(this->cal_days.at(i).Workday_Flag==true)
            {return true;}
            else
            {return false;}
        }


    }

    return false;

}

void Calendar::addCalendarDay(QDate qd, bool workday_flag)
{
    CalendarDay cday;
    cday.date=qd;
    cday.Workday_Flag=workday_flag;
    this->cal_days.append(cday);
}

void Calendar::addCalendarDay(QDate qd)
{
    CalendarDay cday;
    cday.date=qd;
    this->cal_days.append(cday);

}

qint64 Calendar::getCalendarDays()
{
    return this->cal_days.size();
}

QString Calendar::getCALId()
{
    return this->cal_id;
}

void Calendar::updateCalendarDay(QString cal_id, QDate date, bool flag)
{
    bool found = false;
    for (int i=0; i<cal_days.size(); ++i)
    {
        if ((cal_days[i].cal_id == cal_id) || (cal_days[i].date == date))
        {
            found = true;
            cal_days[i].Workday_Flag = flag;
        }
    }
    if (!found) { }
}

QDate Calendar::offsetCalendarWorkDay(QDate qd, int offset_day)
{
    if(offset_day==0){

        return qd;
    }
    else{
        int counter=0;
        QDate tempdate=qd;
        const int increment = offset_day/std::abs(offset_day);
        while(counter<std::abs(offset_day)){
            tempdate = tempdate.addDays(increment);
            if(this->isWorkDay(tempdate)){
                counter++;}
        }
        return tempdate;
    }
}

void Calendar::toggleWorkDay(QDate qd)
{
    CalendarDay *p_calday = this->cal_days.data();
    for(int i=0;i<this->cal_days.size();i++){
        if(qd==cal_days.at(i).date){
            if(this->cal_days.at(i).Workday_Flag==true){
                p_calday[i].Workday_Flag=false;
            }else
            {
                p_calday[i].Workday_Flag=true;
            }
        }
    }
}

QDate Calendar::getNextWorkDay(QDate qd){
    while(!this->isWorkDay(qd)){
        qInfo() << qd.toString();
        qd = qd.addDays(1);
    }
    return qd;
}
//get the next workday of the current date
QDate Calendar::getPrevWorkDay(QDate qd){
    while(!this->isWorkDay(qd)){
        qd = qd.addDays(-1);
    }
    return qd;
}

