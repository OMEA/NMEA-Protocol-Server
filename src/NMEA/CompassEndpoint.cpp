//
//  CompassEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 09.05.13.
//
//

#include "CompassEndpoint.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "Messages/HDMmsg.h"



typedef boost::asio::posix::stream_descriptor pipe_end;

void CompassEndpoint::run_child()
{
    std::vector<std::string> args;
    args.push_back("/usr/bin/minimu9-ahrs");
    args.push_back("-b");
    args.push_back("/dev/i2c-1");
    args.push_back("--output");
    args.push_back("euler");
    try {
        boost::process::pipe pipe = boost::process::create_pipe();
        boost::iostreams::file_descriptor_sink sink(pipe.sink, boost::iostreams::close_handle);
        boost::iostreams::file_descriptor_source source(pipe.source, boost::iostreams::close_handle);
        pend = new pipe_end(io_service, pipe.source);
        
        boost::process::child child = boost::process::execute(boost::process::initializers::set_args(args),
                                                          boost::process::initializers::start_in_dir("."),
                                                          boost::process::initializers::throw_on_error(),
                                                          boost::process::initializers::bind_stdout(sink),
                                                          boost::process::initializers::close_stdin()//,
                                                          //boost::process::initializers::close_stderr()
                                                          );
        
        pend->async_read_some(boost::asio::buffer(data_, max_length),boost::bind(&CompassEndpoint::handle_read, this,
                                                                            boost::asio::placeholders::error,
                                                                            boost::asio::placeholders::bytes_transferred));
        io_service.run();
        wait_for_exit(child);
    }catch (std::exception& e)
    {
        std::ostringstream oss;
        oss << "CompassEndpoint Exception: " << e.what();
        log(oss.str());
    }
}


boost::shared_ptr<CompassEndpoint> CompassEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    CompassEndpoint_ptr compassEndpoint(new CompassEndpoint(connectedTo));
    compassEndpoint->initialize();
    return compassEndpoint;
}

CompassEndpoint::CompassEndpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo), timer_(io_service,boost::posix_time::seconds(1)){
    yaw = 0;
    pitch = 0;
    roll = 0;
    timer_.async_wait(boost::bind(&CompassEndpoint::send_messages, this));
}

void CompassEndpoint::initialize(){
    NMEAEndpoint::initialize();
    workerThread=boost::thread(&CompassEndpoint::run_child, this);
    //run_child();
    boost::function<void (Command_ptr)> func = boost::bind(&CompassEndpoint::list_cmd, this, _1);
    registerVoidCmd("list","List of Properties", "List of all properties of the digital compass.",  func);
    registerEndpoint();
}


void CompassEndpoint::list_cmd(Command_ptr command){
    std::ostringstream oss;
    oss << "Heading:" << "\t\t" << yaw << std::endl;
    oss << "Pitch:" << "\t\t" << pitch << std::endl;
    oss << "Roll:" << "\t\t" << roll << std::endl;
    command->answer(oss.str(), v_shared_from_this());
}

void CompassEndpoint::deliver_impl(NMEAmsg_ptr msg){
    
}

void CompassEndpoint::handle_read(const boost::system::error_code& error,
                                  size_t bytes_transferred){
    if (!error)
    {
        data.append(data_,bytes_transferred);
        size_t pos = std::string::npos;
        if((pos = data.find('\n')) != std::string::npos){
            if(pos>1){
                std::string tmpString = data.substr(0,pos);
                tmpString.erase(std::remove(tmpString.begin(), tmpString.end(), '\n'), tmpString.end());
                tmpString.erase(std::remove(tmpString.begin(), tmpString.end(), '\r'), tmpString.end());
                
                std::vector< std::string > floats;
                split( floats, tmpString, is_any_of(" "), token_compress_on );
                if(floats.size()==9){
                    float yaw = boost::lexical_cast<float>(floats[0]);
                    float pitch = boost::lexical_cast<float>(floats[1]);
                    float roll = boost::lexical_cast<float>(floats[2]);
                    update(yaw, pitch, roll);
                }
            }
            data = data.substr(pos+1);
        }
        pend->async_read_some(boost::asio::buffer(data_, max_length),boost::bind(&CompassEndpoint::handle_read, this,
                                                                      boost::asio::placeholders::error,
                                                                      boost::asio::placeholders::bytes_transferred));
    }
}

void CompassEndpoint::update(float yaw, float pitch, float roll){
    this->yaw = yaw;
    this->pitch = pitch;
    this->roll = roll;
}

void CompassEndpoint::send_messages(){
    HDMmsg_ptr msg(new HDMmsg(this->v_shared_from_this()));
    msg->setHeadingMagnetic(yaw);
    receive(msg);
    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(&CompassEndpoint::send_messages, this));
}