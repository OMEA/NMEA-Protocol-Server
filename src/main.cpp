//
//  main.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include <signal.h>

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
#include "Config/ConfigEndpoint.h"


NMEAServer_ptr srv;

void sighandler(int sig)
{
    std::cout << "Shutdown server ";
    srv->stop();
}

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Hello, World!\n";
    
    srv = NMEAServer::getInstance();
    boost::thread workerThread(&NMEAServer::run, srv);
    std::cout << "Serverthread started" << std::endl;
    ConfigEndpoint::factory(srv, "default");
    std::cout << "default config loaded" << std::endl;
    
    
    //try
    //{
    //    SerialPort p("/dev/master",9600);
    //}
    //catch (std::exception& e)
    //{
    //    std::cerr << "SerialPort Exception: " << e.what() << "\n";
    //}
    struct sigaction act;
    act.sa_handler = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);
    sigaction(SIGTERM, &act, 0);
    sigaction(SIGABRT, &act, 0);
    
    std::cout << "Server ready" << std::endl;
    
        
    
    workerThread.join();
    std::cout << "[done]" << std::endl;
    
    return EXIT_SUCCESS;
}

