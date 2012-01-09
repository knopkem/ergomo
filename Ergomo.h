#ifndef _ERGOMO_H
#define _ERGOMO_H 1

#include <QString>
#include "Serial.h"
#include "Ride.h"

class Ergomo
{
  public:

    Ergomo( QString device );
    ~Ergomo ();

    bool connectionTest();
    QStringList rideList();
    QString csv(int i);
    void setTime();
    void deleteRides();
    QString id();

  private:
    QString readLine();
    void write( QString text );
    Serial serial;
    void log (QString, bool);
    QString getRideDetails(int rideNumber);
    QStringList getRideData();
    QMap<int,Ride> _rideList;
};

#endif // _EGOMO_H

