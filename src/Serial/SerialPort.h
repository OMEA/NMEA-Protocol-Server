//
//  SerialPort.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__SerialPort__
#define __NMEA_Protocol_Server__SerialPort__

#include <iostream>
#include <boost/asio.hpp>
#include "../Support/AsyncEndpoint.h"

using namespace boost;

class SerialPort : public AsyncEndpoint<asio::serial_port>{
public:
    static boost::shared_ptr<SerialPort> factory(boost::shared_ptr<Endpoint> connectedTo, std::string devName);
public:
    SerialPort();
    /**
     * Constructor. Creates and opens a serial device.
     * \param devname serial device name, example "/dev/ttyS0" or "COM1"
     * \param baud_rate serial baud rate
     * \param opt_parity serial parity, default none
     * \param opt_csize serial character size, default 8bit
     * \param opt_flow serial flow control, default none
     * \param opt_stop serial stop bits, default 1
     * \throws boost::system::system_error if cannot open the
     * serial device
     */
    SerialPort(boost::shared_ptr<Endpoint> connectedTo, const std::string& devname, unsigned int baud_rate,
               asio::serial_port_base::parity opt_parity=
                    asio::serial_port_base::parity(asio::serial_port_base::parity::none),
               asio::serial_port_base::character_size opt_csize=
               asio::serial_port_base::character_size(8),
               asio::serial_port_base::flow_control opt_flow=
               asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none),
               asio::serial_port_base::stop_bits opt_stop=
               asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    
    ~SerialPort();
    /**
     * Opens a serial device.
     * \param devname serial device name, example "/dev/ttyS0" or "COM1"
     * \param baud_rate serial baud rate
     * \param opt_parity serial parity, default none
     * \param opt_csize serial character size, default 8bit
     * \param opt_flow serial flow control, default none
     * \param opt_stop serial stop bits, default 1
     * \throws boost::system::system_error if cannot open the
     * serial device
     */
    void open(const std::string& devname, unsigned int baud_rate,
              asio::serial_port_base::parity opt_parity=
              asio::serial_port_base::parity(asio::serial_port_base::parity::none),
              asio::serial_port_base::character_size opt_csize=
              asio::serial_port_base::character_size(8),
              asio::serial_port_base::flow_control opt_flow=
              asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none),
              boost::asio::serial_port_base::stop_bits opt_stop=
              asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
    /**
     * \return true if serial device is open
     */
    bool isOpen() const;
    
    /**
     * Close the serial device
     * \throws boost::system::system_error if any error
     */
    void close();
    /**
     * \return true if error were found
     */
    bool errorStatus() const;
    
    std::string getId();
protected:
    virtual void initialize();
private:
    /**
     * Read buffer maximum size
     */
    static const int readBufferSize=82;
    
    asio::io_service io; ///< Io service object
    asio::serial_port port; ///< Serial port object
    bool openFlag; ///< True if port open
    bool error; ///< Error flag
    mutable boost::mutex errorMutex; ///< Mutex for access to error
    
    std::string devname;
    
    /**
     * Callback to close serial port
     */
    void doClose();
    
    /**
     * Report errors
     * \param e error status
     */
    void setErrorStatus(bool e);
    
};

typedef boost::shared_ptr<SerialPort> SerialPort_ptr;

#endif /* defined(__NMEA_Protocol_Server__SerialPort__) */
