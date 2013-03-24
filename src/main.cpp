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
#include <boost/program_options.hpp>

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
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("config-files", boost::program_options::value< std::vector<std::string> >(), "set initial config files")
    ;
    
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    // insert code here...
    std::cout << "Hello, NMEA!\n";
    
    srv = NMEAServer::getInstance();
    boost::thread workerThread(&NMEAServer::run, srv);
    std::cout << "Serverthread started" << std::endl;
    
    if (vm.count("config-files")){
        std::vector<std::string> configs = vm["config-files"].as< std::vector<std::string> >();
        for(std::vector<std::string>::iterator config=configs.begin(); config!=configs.end(); ++config){
            ConfigEndpoint::factory(srv, *config);
            std::cout << "config "+*config+" loaded" << std::endl;
        }
    }
    else{
        ConfigEndpoint::factory(srv, "default");
        std::cout << "default config loaded" << std::endl;
    }
    
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

