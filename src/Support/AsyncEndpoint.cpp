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

template<class T> AsyncEndpoint<T>::AsyncEndpoint(){
    persist=false;
    isActive=false;
    message_queue_size=10;
}

template<class T> AsyncEndpoint<T>::AsyncEndpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo)
{
    persist=false;
    isActive=false;
    message_queue_size=10;
}

template<class T> AsyncEndpoint<T>::~AsyncEndpoint(){
}

template<class T> void AsyncEndpoint<T>::receive(Command_ptr command){
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
    else if(command->getCommand()=="persist"){
        if(command->getArguments()=="on"){
            persist=true;
            command->answer("persist is now turned on\n", this->shared_from_this());
        }
        else if(command->getArguments()=="off"){
            persist=false;
            command->answer("persist is now turned off\n", this->shared_from_this());
        }
        else{
            command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n", this->shared_from_this());
        }
    }
    else if(command->getCommand()=="queue_size"){
        if(command->getArguments().length()==0){
            std::stringstream ss;
            ss<<"Maximum Queue size is " << message_queue_size << "\n";
            command->answer(ss.str(), this->shared_from_this());
        }
        else{
            try
            {
                message_queue_size = boost::lexical_cast<unsigned int>(command->getArguments());
                std::stringstream ss;
                ss<<"Maximum Queue size changed to " << message_queue_size << "\n";
                command->answer(ss.str(), this->shared_from_this());
            }
            catch(const boost::bad_lexical_cast&)
            {
                command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+". Not a number!\n", this->shared_from_this());
            }
        }
    }
    else{
        NMEAEndpoint::receive(command);
    }
}

template<class T> void AsyncEndpoint<T>::deliver_impl(NMEAmsg_ptr msg){
    bool wasEmpty = true;
    bool wasFull = false;
    {
        boost::mutex::scoped_lock lock(message_queueMutex);
        wasEmpty = message_queue.empty();
        while(message_queue.size()>=message_queue_size){
            message_queue.pop_front();
            wasFull=true;
        }
        message_queue.push_back(msg);
    }
    if(wasEmpty){
        boost::system::error_code ec(0,boost::system::system_category());
        handle_write(ec);
    }
    else if(wasFull){
        std::cerr << "The message queue for "<<getSessionId()<<" was too small, old messages were lost!"<<std::endl;
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
        if(persist){
            AsyncEndpoint<T>::deactivateSession(this->shared_from_this());
            std::cout << "Session deactivated"<<std::endl;
        }
        else{
            unregisterEndpoint();
        }
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
            if(persist){
                AsyncEndpoint<T>::deactivateSession(this->shared_from_this());
                std::cout << "Session deactivated"<<std::endl;
            }
            else{
                unregisterEndpoint();
            }
        }
    }
}

template<class T> void AsyncEndpoint<T>::stop()
{
    if(persist){
        AsyncEndpoint<T>::deactivateSession(this->shared_from_this());
        std::cout << "Session deactivated"<<std::endl;
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
        std::cout << "Session reactivated"<<std::endl;
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