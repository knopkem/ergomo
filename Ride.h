#ifndef _RIDE_H
#define _RIDE_H 1

#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <iostream>

class TimeFrame;

class Ride
{
  public:
    Ride( QString line);
    Ride();
    Ride (const Ride& r);
    ~Ride();

    void addDetails(QString line);
    void setData(QStringList d);
    QString asString();
    QString csv();
    int nr() { return _nr; };
    bool isValid() { return _valid; } ;
   
  private:
    void decodeLine(QString line);

    int _nr;
    QDateTime _start;
    int _N;
    QTime _T;
    int _kfactor;      
    int _wheelsize;    
    int _alt;          
    QStringList _data;
    bool _valid;
    QList<TimeFrame> timeFrames;
};

class TimeFrame
{
  public:
    TimeFrame(QDateTime time, int power, int rpm, int turns, int alt, float speed, int hr, float temp,
              int kfactor, int wheelsize, int weight, int origalt, int distance);
    ~TimeFrame() {};

    QString csvHeader();
    QString csv();
    int turns() { return _turns ;};

  private:
    QDateTime _time;
    int       _power;
    int       _rpm;
    int       _turns;
    int       _alt;
    float       _speed;
    int       _hr;
    float     _temp;
    int       _kfactor;
    int       _wheelsize;
    int       _weight;
    int       _origalt;
    int	      _distance;

};

#endif // _RIDE_H

