#include <QDebug>
#include <QStringList>
#include <iostream>
#include "Serial.h"
#include "Ergomo.h"
#include "Ride.h"

int main(int argc, char *argv[])
{
    bool list      = false;
    bool del       = false;
    bool settime   = false;
    bool show      = false;
    bool id        = false;
    int  nr        = 0;
    QString device = "/dev/ttyUSB0";

    if ((  argc == 2 || argc == 4 ) && strcmp(argv[1], "list" ) == 0 )
        list=true;

    if (( argc == 2 || argc == 4 ) &&  strcmp(argv[1], "delete" ) == 0 )
        del=true;

    if (( argc == 2 || argc == 4 ) && strcmp(argv[1], "settime" ) == 0 )
        settime=true;

    if ((  argc == 2 || argc == 4)  && strcmp(argv[1], "id" ) == 0 )
        id=true;


    if (( argc == 3 || argc == 5 ) && strcmp(argv[1], "show" ) == 0 )
    {
        show=true;
        nr=atoi(argv[2]);
    }

    if ( argc == 5 && strcmp(argv[3], "-d" ) == 0 )
    {
        device=QString(argv[4]);
    }

    if ( argc == 4 && strcmp(argv[2], "-d" ) == 0 )
    {
        device=QString(argv[3]);
    }


    if ( !list && !del && !settime && !show && !id )
    {
       std::cout << "usage: ergomo {id|list|delete|settime} [-d /dev/ttydevice]" << std::endl;
       std::cout << "       ergomo show nr [-d /dev/ttydevice]" << std::endl;
       exit (1); 
    }

    Ergomo ergomo(device);

    if ( ! ergomo.connectionTest() ) 
    {
        std::cerr << "no ergomo connected or ergomo is sleeping" << std::endl;
        exit(1);
    }

    // show recorded rides
    if ( list )
    {
        QStringList rides = ergomo.rideList();
        std::cout << "Ergomo Rides: " << rides.count() << std::endl;
        for (int i=0; i < rides.count(); ++i)
            std::cout << qPrintable(rides[i]) << std::endl;
    }

    // get one ride as csv
    if ( show )
    {
        std::cout << qPrintable(ergomo.csv(nr)) << std::endl;
    }

    //delete rides
    if ( del )
    {
        std::cout << "Ergomo deleting rides..." << std::endl;
        ergomo.deleteRides();
    }

    //set current date and time
    if ( settime )
    {
        std::cout << "Ergomo set current time and date..." << std::endl;
        ergomo.setTime();
    }

    if ( id )
    {
        std::cout << qPrintable(ergomo.id()) << std::endl;
    }


    return 0;
}

