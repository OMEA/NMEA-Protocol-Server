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
#include <boost/lexical_cast.hpp>

template<class T> AsyncEndpoint<T>::AsyncEndpoint(){}

template<class T> AsyncEndpoint<T>::AsyncEndpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo){}

template<class T> void AsyncEndpoint<T>::initialize(){
    NMEAEndpoint::initialize();
    
    registerBoolCmd("persist" ,"Session persistance", "When on, messages are stored after a disconnect and after the reconnect replayed. See also [queue_size]", &persist, false, true);
    registerUIntCmd("queue_size","Message Queue Size", "Defines the maximum size of the message queue for this endpoint. See also [persist]", &message_queue_size, 10, 0, 65535, true);
    boost::function<void (Command_ptr)> func = boost::bind(&AsyncEndpoint<T>::exit_cmd, this, _1);
    registerVoidCmd("exit","End Session", "Ends the session and disconnects the remote host. Command does not take any arguments.",  func);
    unregisterCmd("print_stats");
    boost::function<void (Command_ptr)> func2 = boost::bind(&AsyncEndpoint<T>::print_stats_cmd, this, _1);
    registerVoidCmd("print_stats","Print Session statistics", "Prints statistics about the session.",  func2);
    isActive=false;
    wasFull = false;
}

template<class T> AsyncEndpoint<T>::~AsyncEndpoint(){
}

template<class T> void AsyncEndpoint<T>::exit_cmd(Command_ptr command){
    try
    {
        aostream->close();
    }
    catch (std::exception& e)
    {
        std::cerr << "TCPSession Exception: " << e.what() << "\n";
    }
}

template<class T> void AsyncEndpoint<T>::print_stats_cmd(Command_ptr command){
    NMEAEndpoint::print_stats_cmd(command);
}

template<class T> void AsyncEndpoint<T>::deliver_impl(NMEAmsg_ptr msg){
    bool wasEmpty = true;
    bool nowFull = false;
    {
        boost::mutex::scoped_lock lock(message_queueMutex);
        wasEmpty = message_queue.empty();
        while(message_queue.size()>=message_queue_size){
            message_queue.pop_front();
            nowFull=true;
        }
        message_queue.push_back(msg);
    }
    if(wasEmpty){
        boost::system::error_code ec(0,boost::system::system_category());
        handle_write(ec);
    }
    if(!nowFull){
        wasFull=false;
    }
    else if(nowFull && !wasFull){
        wasFull=true;
        std::ostringstream oss;
        oss << "The message queue for "<<getSessionId()<<" was too small, old messages were lost!";
        log(oss.str());
    }
}

template<class T> void AsyncEndpoint<T>::deliverAnswer_impl(Answer_ptr answer){
    bool wasEmpty = true;
    {
        boost::mutex::scoped_lock lock(message_queueMutex);
        wasEmpty = message_queue.empty();
        message_queue.push_back(answer);
    }
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
                        CommandEndpoint::deliver(command);
                    }
                    catch (const std::invalid_argument& ia) {
                        std::cerr << "Received a command that might not be conform with command format: " << tmpString << std::endl;
                    }
                }
                else if(data.find('$')!=std::string::npos || data.find('!')!=std::string::npos){
                    if(input){
                        try {
                            NMEAmsg_ptr msg = NMEAmsg::factory(tmpString, this->shared_from_this(), check_checksum);
                            receive(msg);
                        }
                        catch (const std::invalid_argument& ia) {
                            std::cerr << "Received a message that might not be conform with NMEA-0183 message format: " << tmpString << " (" << ia.what() << ")" << std::endl;
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
        stop();
//        if(persist){
//            AsyncEndpoint<T>::deactivateSession(this->shared_from_this());
//            std::cout << "Session deactivated"<<std::endl;
//        }
//        else{
//            unregisterEndpoint();
//        }
    }
}

template<class T> void AsyncEndpoint<T>::handle_write(const boost::system::error_code& error)
{
    if(isActive){
        if (!error)
        {
            message_queueMutex.lock();
            if(!message_queue.empty()){
                Message_ptr send_msg = message_queue.front();
                int length = send_msg->toBuffer(data_send_, max_length);
                message_queue.pop_front();
                message_queueMutex.unlock();
                boost::asio::async_write(*aostream, boost::asio::buffer(data_send_,length),
                                         boost::bind(&AsyncEndpoint::handle_write, this->shared_from_this(),
                                                     boost::asio::placeholders::error));
            }
            else{
                message_queueMutex.unlock();
            }
        }
        else
        {
            stop();
//            if(persist){
//                AsyncEndpoint<T>::deactivateSession(this->shared_from_this());
//                std::cout << "Session deactivated"<<std::endl;
//            }
//            else{
//                unregisterEndpoint();
//            }
        }
    }
}

template<class T> void AsyncEndpoint<T>::stop()
{
    if(persist){
        AsyncEndpoint<T>::deactivateSession(this->shared_from_this());
        log("Session deactivated");
    }
    else{
        unregisterEndpoint();
    }
}

template<class T> void AsyncEndpoint<T>::start()
{
    registerEndpoint();
    if(boost::shared_ptr<AsyncEndpoint<T> > activated = activateSession(this->shared_from_this())){
        //Transfer session object over to old session
        activated->unregisterEndpoint();
        {
            boost::mutex::scoped_lock lock(message_queueMutex);
            message_queue = std::deque<Message_ptr>(activated->message_queue);
        }
        log("Session reactivated");
        boost::system::error_code ec(0,boost::system::system_category());
        handle_write(ec);
    }
    isActive=true;
    aostream->async_read_some(boost::asio::buffer(data_, max_length),
                              boost::bind(&AsyncEndpoint::handle_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
}//  NMEA-Protocol-Server

template<class T> void AsyncEndpoint<T>::deactivateSession(boost::shared_ptr<AsyncEndpoint<T> > session){
    session->isActive=false;
    sessions.insert(std::pair<std::string,boost::shared_ptr<AsyncEndpoint<T> > >(session->sessionId,session));
}

template<class T> boost::shared_ptr<AsyncEndpoint<T> > AsyncEndpoint<T>::activateSession(boost::shared_ptr<AsyncEndpoint<T> > session){    
    if(sessions.find(session->sessionId)!=sessions.end()){
        std::pair<std::string,boost::shared_ptr<AsyncEndpoint<T> > > pair = *(sessions.find(session->sessionId));
        sessions.erase(session->sessionId);
        session->isActive=true;
        return pair.second;
    }
    else{
        return boost::shared_ptr<AsyncEndpoint<T> >();
    }
}

template <class T> std::map<std::string, boost::shared_ptr<AsyncEndpoint<T> > > AsyncEndpoint<T>::sessions;

template class AsyncEndpoint<boost::asio::ip::tcp::socket>;
template class AsyncEndpoint<boost::asio::serial_port>;