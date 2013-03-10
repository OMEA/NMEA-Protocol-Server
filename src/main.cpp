//
//  main.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include <iostream>


#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <boost/thread.hpp>

#include "TCP/TCPServer.h"
#include "Serial/SerialPort.h"
#include "NMEA/NMEAServer.h"
#include "File/FileEndpoint.h"


int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Hello, World!\n";
    
    NMEAServer *srv = NMEAServer::getInstance();
    boost::thread workerThread(&NMEAServer::run, srv);
    
    try
    {
        FileEndpoint_ptr testfile(new FileEndpoint);
        testfile->open("/tmp/test");
    }
    catch (std::exception& e)
    {
        std::cerr << "FileEndpoint Exception: " << e.what() << "\n";
    }
    
    try
    {
        SerialPort p("/dev/master",9600);
    }
    catch (std::exception& e)
    {
        std::cerr << "SerialPort Exception: " << e.what() << "\n";
    }
    
    try
    {
        
        boost::asio::io_service io_service;
        
        using namespace std; // For atoi.
        TCPServer s(io_service, 10110);
        TCPServer s2(io_service, 10111);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}

