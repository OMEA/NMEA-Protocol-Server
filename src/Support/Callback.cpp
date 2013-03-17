//
//  Callback.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 17.03.13.
//
//

#include "Callback.h"
#include <sstream>

void BoolCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    std::stringstream ss;
    if(command->getCommand()==name){
        if(command->getArguments().length()==0){
            ss<<name<<" is ";
            if(*boolean){
                ss<<"on";
            }
            else{
                ss<<"off";
            }
            ss<<'\n';
            command->answer(ss.str(), instance);
        }
        else if(command->getArguments()=="on"){
            if(writeable){
                *boolean=true;
                ss<<name<< " is now turned on\n";
                command->answer(ss.str(), instance);
            }
            else{
                ss<<name<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
        else if(command->getArguments()=="off"){
            if(writeable){
                *boolean=false;
                ss<<name<< " is now turned off\n";
                command->answer(ss.str(), instance);
            }
            else{
                ss<<name<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
        else{
            command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n", instance);
        }
    }
}

void UIntCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    std::stringstream ss;
    if(command->getCommand()==name){
        if(command->getArguments().length()==0){
            ss<<name<<" is " << *uint << '\n';
            command->answer(ss.str(), instance);
        }
        else{
            if(writeable){
                try
                {
                    unsigned int tmp_uint = boost::lexical_cast<unsigned int>(command->getArguments());
                    if(tmp_uint > min && tmp_uint < max){
                        *uint = tmp_uint;
                        std::stringstream ss;
                        ss<< name << " changed to " <<  *uint << "\n";
                        command->answer(ss.str(), instance);
                    }
                    else{
                        std::stringstream ss;
                        ss << "Cannot set " << name << " to " <<  tmp_uint << ". Must be between "<< min << " and " << max <<"\n";
                        command->answer(Answer::WRONG_ARGS, ss.str(), instance);
                    }
                    
                }
                catch(const boost::bad_lexical_cast&)
                {
                    command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+". Not a number!\n", instance);
                }
            }
            else{
                ss<<name<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
    }
}