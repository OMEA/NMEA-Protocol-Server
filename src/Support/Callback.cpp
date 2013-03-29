//
//  Callback.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 17.03.13.
//
//

#include "Callback.h"
#include <sstream>
#include <boost/bind.hpp>

const std::string Callback::to_str() const{
    std::ostringstream ss;
    ss << name << '\t' << speakingName << ": " << description;
    return ss.str();
}

std::ostream& operator<< (std::ostream &out, Callback &callback)
{
    out << callback.to_str();
    return out;
}

void BoolCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    std::ostringstream ss;
    if(command->getCommand()==name){
        if(command->getArguments().length()==0){
            ss<<speakingName<<" is ";
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
                ss<<speakingName<< " is now turned on\n";
                command->answer(ss.str(), instance);
            }
            else{
                ss<<speakingName<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
        else if(command->getArguments()=="off"){
            if(writeable){
                *boolean=false;
                ss<<speakingName<< " is now turned off\n";
                command->answer(ss.str(), instance);
            }
            else{
                ss<<speakingName<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
        else{
            command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n", instance);
        }
    }
}

const std::string BoolCallback::to_str() const{
    std::ostringstream ss;
    ss << MemberCallback::to_str();
    if(writeable){
        ss << " The command takes \"on\" or \"off\" as an argument to change the property and can be read with an empy argument.";
    }
    else{
        ss << "The property is read-only and cannot be changed.";
    }
    return ss.str();
}

void UIntCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    std::ostringstream ss;
    if(command->getCommand()==name){
        if(command->getArguments().length()==0){
            ss<<speakingName<<" is " << *uint << '\n';
            command->answer(ss.str(), instance);
        }
        else{
            if(writeable){
                try
                {
                    unsigned int tmp_uint = boost::lexical_cast<unsigned int>(command->getArguments());
                    if(tmp_uint >= min && tmp_uint <= max){
                        *uint = tmp_uint;
                        ss<< speakingName << " changed to " <<  *uint << "\n";
                        command->answer(ss.str(), instance);
                    }
                    else{
                        ss << "Cannot set " << speakingName << " to " <<  tmp_uint << ". Must be between "<< min << " and " << max <<"\n";
                        command->answer(Answer::WRONG_ARGS, ss.str(), instance);
                    }
                    
                }
                catch(const boost::bad_lexical_cast&)
                {
                    command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+". Not a number!\n", instance);
                }
            }
            else{
                ss<<speakingName<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
    }
}

const std::string UIntCallback::to_str() const{
    std::ostringstream ss;
    ss << Callback::to_str();
    if(writeable){
        ss << " The command takes a number between " << min << " and " << max << " as an argument to change the property and can be read with an empy argument.";
    }
    else{
        ss << "The property is read-only and cannot be changed.";
    }
    return ss.str();
}

void IntCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    std::ostringstream ss;
    if(command->getCommand()==name){
        if(command->getArguments().length()==0){
            ss<<speakingName<<" is " << *int_ << '\n';
            command->answer(ss.str(), instance);
        }
        else{
            if(writeable){
                try
                {
                    int tmp_int = boost::lexical_cast<unsigned int>(command->getArguments());
                    if(tmp_int >= min && tmp_int <= max){
                        *int_ = tmp_int;
                        ss<< speakingName << " changed to " <<  *int_ << "\n";
                        command->answer(ss.str(), instance);
                    }
                    else{
                        ss << "Cannot set " << speakingName << " to " <<  tmp_int << ". Must be between "<< min << " and " << max <<"\n";
                        command->answer(Answer::WRONG_ARGS, ss.str(), instance);
                    }
                    
                }
                catch(const boost::bad_lexical_cast&)
                {
                    command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+". Not a number!\n", instance);
                }
            }
            else{
                ss<<speakingName<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
    }
}

const std::string IntCallback::to_str() const{
    std::ostringstream ss;
    ss << Callback::to_str();
    if(writeable){
        ss << " The command takes a number between " << min << " and " << max << " as an argument to change the property and can be read with an empy argument.";
    }
    else{
        ss << "The property is read-only and cannot be changed.";
    }
    return ss.str();
}

void StringCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    std::ostringstream ss;
    if(command->getCommand()==name){
        if(command->getArguments().length()==0){
            ss<<speakingName<<" is " << *string << '\n';
            command->answer(ss.str(), instance);
        }
        else{
            if(writeable){
                *string = command->getArguments();
                ss<< speakingName << " changed to " <<  *string << "\n";
            }
            else{
                ss<<speakingName<< " is not writeable\n";
                command->answer(ss.str(), instance);
            }
        }
    }
}

const std::string StringCallback::to_str() const{
    std::ostringstream ss;
    ss << Callback::to_str();
    if(writeable){
        ss << " The command takes a string as an argument to change the property and can be read with an empy argument.";
    }
    else{
        ss << "The property is read-only and cannot be changed.";
    }
    return ss.str();
}

void StringVectorCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    std::ostringstream oss;
    if(command->getArguments().length()==0){
        oss << speakingName << " is ";
        if(vector->size()==0){
            oss << "empty!";
        }
        else{
            for(std::vector<std::string>::iterator stringId = vector->begin(); stringId != vector->end();){
                oss << *stringId;
                ++stringId;
                if(stringId != vector->end()){
                    oss << ", ";
                }
            }
        }
        oss << std::endl;
        command->answer(oss.str(), instance);
    }
    else if(unsigned int i = command->getArguments().find("add")!=std::string::npos){
        if(writeable){
            i+=std::string("add").length();
            std::string ids=command->getArguments().substr(i, std::string::npos);
            std::stringstream iss(ids);
            std::string id;
            while(std::getline(iss, id, ',')) {
                trim(id);
                if(id.length()>0 && std::find(vector->begin(), vector->end(), id) == vector->end()){
                    vector->push_back(id);
                }
            }
            oss << speakingName << " now is ";
            if(vector->size()==0){
                oss << "empty!";
            }
            else{
                for(std::vector<std::string>::iterator stringId = vector->begin(); stringId != vector->end();){
                    oss << *stringId;
                    ++stringId;
                    if(stringId != vector->end()){
                        oss << ", ";
                    }
                }
            }
            oss << std::endl;
            command->answer(oss.str(), instance);
        }
        else{
            oss << speakingName << " is read-only and cannot be changed!" << std::endl;
            command->answer(oss.str(), instance);
        }
    }
    else if(unsigned int i = command->getArguments().find("set")!=std::string::npos){
        if(writeable){
            vector->erase(vector->begin(),vector->end());
            i+=std::string("set").length();
            std::string ids=command->getArguments().substr(i, std::string::npos);
            std::stringstream iss(ids);
            std::string id;
            while(std::getline(iss, id, ',')) {
                trim(id);
                if(id.length()>0 && std::find(vector->begin(), vector->end(), id) == vector->end()){
                    vector->push_back(id);
                }
            }
            oss << speakingName << " now is ";
            if(vector->size()==0){
                oss << "empty!";
            }
            else{
                for(std::vector<std::string>::iterator stringId = vector->begin(); stringId != vector->end();){
                    oss << *stringId;
                    ++stringId;
                    if(stringId != vector->end()){
                        oss << ", ";
                    }
                }
            }
            oss << std::endl;
            command->answer(oss.str(), instance);
        }
        else{
            oss << speakingName << " is read-only and cannot be changed!" << std::endl;
            command->answer(oss.str(), instance);
        }
    }
    else if(unsigned int i = command->getArguments().find("remove")!=std::string::npos){
        if(writeable){
            i+=std::string("remove").length();
            std::string ids=command->getArguments().substr(i, std::string::npos);
            std::stringstream iss(ids);
            std::string id;
            while(std::getline(iss, id, ',')) {
                trim(id);
                if(id.length()>0){
                    std::vector<std::string>::iterator it = std::find(vector->begin(), vector->end(), id);
                    if(it != vector->end()){
                        vector->erase(it);
                    }
                }
            }
            oss << speakingName << " now is ";
            if(vector->size()==0){
                oss << "empty!";
            }
            else{
                for(std::vector<std::string>::iterator stringId = vector->begin(); stringId != vector->end();){
                    oss << *stringId;
                    ++stringId;
                    if(stringId != vector->end()){
                        oss << ", ";
                    }
                }
            }
            oss << std::endl;
            command->answer(oss.str(), instance);
        }
        else{
            oss << speakingName << " is read-only and cannot be changed!" << std::endl;
            command->answer(oss.str(), instance);
        }
    }
    else{
        command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+". Not one of add, set or remove \n", instance);
    }
}

const std::string StringVectorCallback::to_str() const{
    std::ostringstream ss;
    ss << Callback::to_str();
    if(writeable){
        ss << " Can be changed using \"set <a>,<b>,...,<n>\", \"add <a>,<b>,...,<n>\", or \"remove <a>,<b>,...,<n>\"";
    }
    else{
        ss << "The property is read-only and cannot be changed.";
    }
    return ss.str();
}

void VoidCallback::execute(Command_ptr command, CommandEndpoint_ptr instance){
    func(command);
}