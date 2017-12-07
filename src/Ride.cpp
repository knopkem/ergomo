#include <QDebug>
#include <QStringList>
#include "Ride.h"

Ride::Ride( QString line)
{
    _nr        = line.mid(0,4).toInt();
    _start     = QDateTime::fromString(line.mid(5,19),"dd.MM.yyyy-hh:mm:ss");
    _N         = line.mid(29,6).toInt();
    _T         = QTime::fromString(line.mid(41).trimmed(),"h:mm:ss" );

    _alt=0;
    _kfactor=0; 
    _wheelsize=0;
    if ( _T > QTime::fromString("00:01:00") )
        _valid=true;
    else
        _valid=false;
}

Ride::Ride()
{
    _nr = 0;
    _start = QDateTime();
    _N = 0;
    _T = QTime();
    _kfactor= 0;   
    _wheelsize = 0;
    _alt = 0;
    _data = QStringList();
    _valid=false;
}

Ride::Ride (const Ride& r)
{
    _nr = r._nr;
    _start = r._start;
    _N = r._N;
    _T = r._T;
    _kfactor= r._kfactor;   
    _wheelsize = r._wheelsize;   
    _alt = r._alt;          
    _data = r._data;
    _valid=r._valid;
}


Ride::~Ride()
{ 
}


void Ride::addDetails(QString line)
{
    //=line.mid(27).section(";",0,0 )
    //=line.mid(27).section(";",1,1 )
    _kfactor   = line.mid(27).section(";", 2, 2).toInt();
    _wheelsize = line.mid(27).section(";", 3, 3).toInt();
    _alt       = line.mid(27).section(";", 4, 4).toInt();
}


void Ride::setData(QStringList d)
{
    _data = d;
    for (int i=0; i< d.size(); ++i)
    if (d[i].startsWith(":"))
        decodeLine(d[i]);

}


void Ride::decodeLine(QString line)
{
    QByteArray header = QByteArray::fromHex( line.mid(1, 32).toLocal8Bit() );

    unsigned int data_sets       =  (unsigned char) header[ 0] & 0x1f;     // nr of data sets
    unsigned int dat_step        =  (unsigned char) header[ 1];            // data steps in seconds
    qint64 seconds               =  (unsigned char) header[ 2] 
                                 | ((unsigned char) header[ 3] << 8)
                                 | ((unsigned char) header[ 4] << 16)
                                 | ((unsigned char) header[ 5] << 24);
    unsigned int kfactor         =  (unsigned char) header[ 6] 
                                 | ((unsigned char) header[ 7] << 8);              
    unsigned int wheelsize       =  (unsigned char) header[ 8]             // wheelsize in mm            
                                 | ((unsigned char) header[ 9] << 8);  
    unsigned int origalt         =  (unsigned char) header[10]             // altitude in m 
                                 | ((unsigned char) header[11] << 8);  
                                 // (unsigned char) header[12] dummy
                                 // (unsigned char) header[13] dummy
                                 // (unsigned char) header[14] dummy
    unsigned int weight          =  (unsigned char) header[15];            // weight in kg

    
    QDateTime date_time = QDateTime::fromMSecsSinceEpoch(seconds * 1000).addYears(31).toUTC();


    for (unsigned int i=0; i<data_sets; ++i)
    {
        QByteArray dat = QByteArray::fromHex( line.mid(33+i*24, 24).toLocal8Bit() );

        unsigned int power =  (unsigned char) dat[ 0]               // power in watt
                           | ((unsigned char) dat[ 1] << 8);              
        unsigned int rpm   =  (unsigned char) dat[ 2]               // rpm in 1/min
                           | ((unsigned char) dat[ 3] << 8);              
        unsigned int turns =  (unsigned char) dat[ 4] 		    // wheel turns
                           | ((unsigned char) dat[ 5] << 8);              
        unsigned int alt   =  (unsigned char) dat[ 6]               // alt in m
                           | ((unsigned char) dat[ 7] << 8);              
        float speed        = ((unsigned char) dat[ 8]               // speed in km/h
                           | ((unsigned char) dat[ 9] << 8)) / 10.0;              
        unsigned int hr    =  (unsigned char) dat[10];              // heart rate in bpm
        float        temp  = ((float) (unsigned char) dat[11] ) / 3.0;        // temperature in °C


        // calculate distance
        int turnsum=turns;
        for (int i=0; i < timeFrames.size(); ++i )
           turnsum+=timeFrames[i].turns();
        int distance = (int) ( (double) turnsum * (double) wheelsize / 1000.0 );

	timeFrames << TimeFrame(date_time, power, rpm, turns, alt, speed, hr, temp, kfactor, wheelsize, weight, origalt, distance);

        date_time = date_time.addSecs(dat_step);

    } 

    // checksum

    unsigned char mychecksum=0;
    QByteArray chunk = QByteArray::fromHex( line.mid(1,512).toLocal8Bit());
    for (int i=0; i < chunk.size(); ++i )
    {
        mychecksum += (unsigned char) chunk[i];
    }
 
    unsigned char checksum = (unsigned char) QByteArray::fromHex( line.mid(513,4).toLocal8Bit() )[0];
 
    if ( (unsigned char) ( checksum += mychecksum ) != 0 )
        qDebug() << "checksum ERROR";


}


QString Ride::asString()
{
    QString line;
    line += QString::number(_nr) + ": ";
    line += _start.toString("yyyy-MM-dd hh:mm:ss") + " ==> ";
    line += _T.toString("hh:mm:ss");
    return line;
}


QString Ride::csv()
{
    QString ret;
    if (timeFrames.size()>0 )
    {
        ret = timeFrames[0].csvHeader();
        for (int i = 0; i < timeFrames.size(); ++i)
        {
            ret += "\n" + timeFrames[i].csv();
        }
    }
    return ret;
}


TimeFrame::TimeFrame(QDateTime time, int power, int rpm, int turns, int alt, float speed, int hr, float temp,
              int kfactor, int wheelsize, int weight, int origalt, int distance)
    : _time(time), _power(power), _rpm(rpm), _turns(turns), _alt(alt), _speed(speed), _hr(hr), _temp(temp),
              _kfactor(kfactor), _wheelsize(wheelsize), _weight(weight), _origalt(origalt), _distance(distance)
{

}


QString TimeFrame::csvHeader()
{
    return QString("time,power,rpm,turns,alt,speed,hr,temperature,kfactor,wheelsize,weight,origalt,distance");
}


QString TimeFrame::csv()
{
    QString d=",";

    QString line;
    line += _time.toString("yyyy-MM-dd hh:mm:ss") + d;
    line += QString::number(_power) + d;
    line += QString::number(_rpm) + d;
    line += QString::number(_turns) + d;
    line += QString::number(_alt) + d;
    line += QString::number(_speed) + d;
    line += QString::number(_hr) + d;
    line += QString::number(_temp) + d;
    line += QString::number(_kfactor) + d;
    line += QString::number(_wheelsize) + d;
    line += QString::number(_weight) + d;
    line += QString::number(_origalt) + d;
    line += QString::number(_distance);
    return line;
}

