//
//  AsyncEndpoint.cpp
//
//  AsyncEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 27.02.13.
//
//

#include "AsyncEndpoint.h"

#include "../NMEA/NMEAServer.h"

template<class T> AsyncEndpoint<T>::~AsyncEndpoint(){
}

template<class T> void AsyncEndpoint<T>::receiveCommand(Command_ptr command){
    if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
        try
        {
            aostream->close();
        }
        catch (std::exception& e)
        {
            std::cerr << "TCPSession Exception: " << e.what() << "\n";
        }
    }
    else{
        NMEAEndpoint::receiveCommand(command);
    }
}

template<class T> void AsyncEndpoint<T>::deliver_impl(NMEAmsg_ptr msg){
    message_queueMutex.lock();
    bool wasEmpty = message_queue.empty();
    message_queue.push_back(msg);
    message_queueMutex.unlock();
    if(wasEmpty){
        boost::system::error_code ec(0,boost::system::system_category());
        handle_write(ec);
    }
}

template<class T> void AsyncEndpoint<T>::deliverAnswer_impl(std::string answer){
    answer_queueMutex.lock();
    bool wasEmpty = answer_queue.empty();
    answer_queue.push_back(answer);
    answer_queueMutex.unlock();
    if(wasEmpty){
        boost::system::error_code ec(0,boost::system::system_category());
        handle_write(ec);
    }
}

template<class T> void AsyncEndpoint<T>::handle_read(const boost::system::error_code& error,
                             size_t bytes_transferred)
{
    if (!error)
    {
        data.append(data_,bytes_transferred);
        size_t pos = std::string::npos;
        if((pos = data.find('\n')) != std::string::npos){
            if(pos>1){
                std::string tmpString = data.substr(0,pos);
                tmpString.erase(std::remove(tmpString.begin(), tmpString.end(), '\n'), tmpString.end());
                tmpString.erase(std::remove(tmpString.begin(), tmpString.end(), '\r'), tmpString.end());
                if(tmpString=="exit"){
                    aostream->close();
                }
                if(data.find('#')!=std::string::npos){
                    try {
                        Command_ptr command(new Command(tmpString, this->shared_from_this()));
                        deliverCommand(command);
                    }
                    catch (const std::invalid_argument& ia) {
                        std::cerr << "Received a command that might not be conform with command format: " << tmpString << std::endl;
                    }
                }
                else if(data.find('$')!=std::string::npos || data.find('!')!=std::string::npos){
                    if(input){
                        try {
                            NMEAmsg_ptr msg(new NMEAmsg(tmpString, this->shared_from_this()));
                            receive(msg);
                        }
                        catch (const std::invalid_argument& ia) {
                            std::cerr << "Received a message that might not be conform with NMEA-0183 message format: " << tmpString << std::endl;
                        }
                    }
                }
            }
            data = data.substr(pos+1);
        }
        aostream->async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&AsyncEndpoint::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        NMEAServer::getInstance()->removeEndpoint(this->shared_from_this());
    }
}

template<class T> void AsyncEndpoint<T>::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        message_queueMutex.lock();
        if(!message_queue.empty()){
            NMEAmsg_ptr send_msg = message_queue.front();
            int length = send_msg->toBuffer(data_send_,max_length,checksum);
            message_queue.pop_front();
            message_queueMutex.unlock();
            boost::asio::async_write(*aostream, boost::asio::buffer(data_send_,length),
                                     boost::bind(&AsyncEndpoint::handle_write, this->shared_from_this(),
                                                 boost::asio::placeholders::error));
            return;
        }
        else{
            message_queueMutex.unlock();
        }
        
        if(!answer_queue.empty()){
            std::string answer = answer_queue.front();
            int length = answer.length();
            memcpy(data_send_,answer.c_str(),length);
            answer_queue.pop_front();
            answer_queueMutex.unlock();
            boost::asio::async_write(*aostream, boost::asio::buffer(data_send_,length),
                                     boost::bind(&AsyncEndpoint::handle_write, this->shared_from_this(),
                                                 boost::asio::placeholders::error));
            return;
        }
        else{
            answer_queueMutex.unlock();
        }

        
    }
    else
    {
        NMEAServer::getInstance()->removeEndpoint(this->shared_from_this());
    }
}

template<class T> void AsyncEndpoint<T>::stop()
{
    NMEAServer::getInstance()->removeEndpoint(this->shared_from_this());
}

template<class T> void AsyncEndpoint<T>::start()
{
    NMEAServer::getInstance()->addEndpoint(this->shared_from_this());
    aostream->async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&AsyncEndpoint::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}//  NMEA-Protocol-Server


template class AsyncEndpoint<boost::asio::ip::tcp::socket>;
template class AsyncEndpoint<boost::asio::serial_port>;