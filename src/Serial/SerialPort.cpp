//
//  SerialPort.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "SerialPort.h"
#include "../NMEA/NMEAServer.h"

boost::shared_ptr<SerialPort> SerialPort::factory(boost::shared_ptr<Endpoint> connectedTo, std::string devName){
    //IOService in endpoint und beim beenden stop() aufrufen
    
    SerialPort_ptr sp = boost::shared_ptr<SerialPort>(new SerialPort(connectedTo, devName,9600));
    sp->initialize();
    return sp;
}

SerialPort::SerialPort(boost::shared_ptr<Endpoint> connectedTo, const std::string& devname,unsigned int baud_rate,
                       asio::serial_port_base::parity opt_parity,
                       asio::serial_port_base::character_size opt_csize,
                       asio::serial_port_base::flow_control opt_flow,
                       asio::serial_port_base::stop_bits opt_stop):AsyncEndpoint<asio::serial_port>(connectedTo), io(), port(io), openFlag(false),
error(false){
    
    setSessionId(devname);
    setAOStream(&port);
    open(devname,baud_rate,opt_parity,opt_csize,opt_flow,opt_stop);
}

void SerialPort::initialize(){
    AsyncEndpoint<asio::serial_port>::initialize();
    start();
    io.run();
}

SerialPort::~SerialPort()
{
    if(isOpen())
    {
        try {
            close();
        } catch(...)
        {
            //Don't throw from a destructor
        }
    }
}

std::string SerialPort::getId(){
    return "Serial://"+devname;
}

void SerialPort::open(const std::string& devname, unsigned int baud_rate,
                       asio::serial_port_base::parity opt_parity,
                       asio::serial_port_base::character_size opt_csize,
                       asio::serial_port_base::flow_control opt_flow,
                       asio::serial_port_base::stop_bits opt_stop)
{
    this->devname = devname;
    if(isOpen()) close();
    
    setErrorStatus(true);//If an exception is thrown, error_ remains true
    port.open(devname);
    port.set_option(asio::serial_port_base::baud_rate(baud_rate));
    port.set_option(opt_parity);
    port.set_option(opt_csize);
    port.set_option(opt_flow);
    port.set_option(opt_stop);
    
    setErrorStatus(false);//If we get here, no error
    openFlag=true; //Port is now open
}

void SerialPort::close()
{
    if(!isOpen()) return;
    
    openFlag=false;
    io.post(boost::bind(&SerialPort::doClose, this));
    io.reset();
    if(errorStatus())
    {
        throw(boost::system::system_error(boost::system::error_code(),
                                          "Error while closing the device"));
    }
}

void SerialPort::doClose()
{
    boost::system::error_code ec;
    port.cancel(ec);
    if(ec) setErrorStatus(true);
    port.close(ec);
    if(ec) setErrorStatus(true);
}


bool SerialPort::isOpen() const
{
    return openFlag;
}

bool SerialPort::errorStatus() const
{
    lock_guard<mutex> l(errorMutex);
    return error;
}

void SerialPort::setErrorStatus(bool e)
{
    lock_guard<mutex> l(errorMutex);
    error=e;
}
