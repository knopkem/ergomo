#include "Ergomo.h"
#include "Ride.h"
#include <QDebug>

#ifndef Q_OS_WIN32
#include <unistd.h>
#endif

namespace {
    class SleepThread : protected QThread
    {
    public:
        static void msleep(unsigned long msecs) { QThread::msleep(msecs); }
    };
}


Ergomo::Ergomo( QString device )
: serial(device)
{
    QString message;

    if (! serial.open(message) )
    {
        qDebug() << "Error: Cannot open serial port:" << message;
    }
}

Ergomo::~Ergomo ()
{
    serial.close();
}


QString Ergomo::csv(int nr)
{
    getRideDetails(nr);
    return _rideList[nr].csv();
}

void Ergomo::log(QString line, bool fromErgomo)
{
//        qDebug() << (fromErgomo == true?"ergomo->pc:":"pc->ergomo:") << line;
}

void Ergomo::write( QString text )
{
    QString message;
    const int l = text.length();
    char* buf = new char[l+1];
    for (int i = 0; i < text.length(); ++i)
        buf[i] = text[i].toLatin1();
    buf[text.length()]= 0x0d;

    log(text,false);
    serial.write( buf , sizeof(buf), message);
}


QString Ergomo::readLine()
{
    QString line;
    int count;
    QString message;
    char buf[1];
    QTime timeout = QTime::currentTime().addSecs(1);
 
    while (QTime::currentTime() < timeout )
    {
       count = serial.read(buf, sizeof(buf), message);

       if ( count > 0 )
       {
          if ( buf[0] == 0x0a )
          {
             log(line,true);
             return line;
          }
    
          if ( buf[0] != 0x0d )
             line = line.append(buf[0]); 
          timeout = QTime::currentTime().addSecs(1);
       }
    }

    log(line,true);
    return line;
}

bool Ergomo::connectionTest()
{
    for (int i=0; i<3; ++i)
    {
       write("ID");
       if ( readLine().contains("ERGOMO") )
       {
           log ("ergomo connection successful",false);
           return true;
       }
    }

    qDebug() << "ergomo does not respond!";
    return false;
}

QString Ergomo::id()
{
   write("ID");
   return readLine();
}

QString Ergomo::getRideDetails(int rideNumber)
{
    QString line;
 
    write("DH");
    if (!readLine().contains("?") )
        return QString();

    write(QString::number(rideNumber));
    line = readLine();
    _rideList[rideNumber].addDetails(line);
    _rideList[rideNumber].setData(getRideData());

    return line;
}

QStringList Ergomo::getRideData()
{
    QStringList lines;
    QString line;
    QTime timeout = QTime::currentTime().addSecs(1);

    write("DS");

    std::cerr << "Reading ergomo data";
    while (QTime::currentTime() < timeout )
    {
       line = readLine();
       if ( line.contains ("END") )
       {
           std::cout << std::endl;
           return lines;
       }
       SleepThread::msleep(25000);
       write("O");
       SleepThread::msleep(25000);
       timeout = QTime::currentTime().addSecs(1);
       lines << line;
       std::cerr << "." << std::flush;
    }
    return QStringList();
}

QStringList Ergomo::rideList()
{
    QStringList rides;
    QString line;
    QStringList validRides;
    QTime timeout = QTime::currentTime().addSecs(1);

    write("DT");

    while (QTime::currentTime() < timeout )
    {
        line = readLine();
        if ( line.contains("RIDES AT FLASH") )
            return validRides;
        else
        {
            Ride r(line);
            if (r.isValid())
               validRides << r.asString();
            
            _rideList.insert(r.nr(), r );
            rides << line;
    	    timeout = QTime::currentTime().addSecs(1);
        }
    }
    return validRides;
}


void Ergomo::deleteRides()
{
    if ( ! connectionTest() )
    {
    	qDebug() << "no connection";
	return ;
    }

    write ("DCLEAR");
    while ( ! readLine().contains("SECTOR 7 EMPTY") );
    readLine();	// get additional empty lines
    readLine();

}

void Ergomo::setTime()
{
    if ( ! connectionTest() )
    {
    	qDebug() << "no connection";
	return ;
    }

    QString time = QDateTime::currentDateTime().toString("yyyy;MM;dd;hh;mm;ss");
    write ("ST="+time);
}

