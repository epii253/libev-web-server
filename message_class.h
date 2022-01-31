#ifndef MESSAGE_CLASS_H
#define MESSAGE_CLASS_H

#include <iostream>
#include <cmath>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ev++.h>
#include <array>
#include <thread>
#include <string.h>
#include <string>
#include <algorithm>
#include <fcntl.h>
#include <boost/algorithm/string.hpp>
#include <string.h>
#include <vector>
#include <sys/stat.h>
#include <iterator>
#include <template_phrases_namespace.h>
//#include <post_command.h>

enum class States_of_request { Waiting,Parsing};

enum class Data_states {All_Information , Only_main_information , Not_enough_main_information , Wait_information};//Или лучше для наличия/отсутсвия каждого пункта сделать состояние?

enum Language{ Fr,Ru,En,Ns /*(Not Stated)*/};

using namespace template_phrases;

void taimer_callback(struct ev_loop *loop,ev_timer *timer_watcher,int revents);

void close_connection(struct ev_loop *loop,ev_io *watcher)
{
    if(watcher->/*priority*/active /*>*/!= 0 /*!= NULL*/)
    {
    shutdown(watcher->fd,SHUT_RDWR);//
    close(watcher->fd);             //System part

    ev_io_stop(loop,watcher);//loop part  //SIGSEGV

    //delete watcher;          //return memory at heap  //doesn't work                              //Problem: How delete ev_io watcher?
    //watcher->~ev_io(); //????????????                                                              //
    delete watcher;    //Do I need to do this? !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  //
    }
    std::cout <<"Close connection"<< std::endl;
}


class message
{
private:
    struct ev_loop* current_loop;
    struct ev_io *current_watcher;

    Language lang = Language::Ns;
    /*const*/ int fd_file = 0;  //!!!
    /*const*/ int size_file = 0;//!!!!
    std::vector<char> message_data;
    /*const*/ double protocol = 0;

    std::string_view string_ptocol;

    Data_states data_state = Data_states::Wait_information;
    std::function<void(Data_states st)> data_state_function = [this](Data_states st){ this->new_data_state(st);};//start state

    States_of_request request_state = States_of_request::Waiting;
    std::function<void(States_of_request req_st)> request_state_function = [this](States_of_request req_st){ this->new_request_state(req_st);};
    void new_request_state(States_of_request req_st);

    virtual void do_all_information(Data_states st) = 0;           //
    virtual void do_only_main_information(Data_states st) = 0;     //  Формировани ответов
    virtual void do_errore_message(Data_states st) = 0;           //
       //




public:
//    message(struct ev_loop *loop,struct ev_io *watcher,double prot,int content_fd , int content_size,Language lan)
//    { current_loop = loop; current_watcher = watcher; fd_file = content_fd;protocol = prot; size_file = content_size; lang = lan;

////        if(prot){ protocol = prot; recogonize_protocol(protocol); }else{new_data_state(Data_states::Not_enough_main_information);}

////        if(content_fd){fd_file = content_fd;} else {fd_file = content_fd; new_data_state(Data_states::Not_enough_main_information);}
////        if(content_size){size_file = content_size;} else {new_data_state(Data_states::Not_enough_main_information);}

////        if(lang){ lang = lan;}else{new_data_state(Data_states::Only_main_information);}

////        if(lan && protocol){ new_data_state(Data_states::All_Information); } //????

//        if(prot && content_fd &&content_size && lang)
//        {recogonize_protocol(protocol);new_data_state(Data_states::All_Information);}
//        else
//        {
//            new_data_state(Data_states::Not_enough_main_information);
//        }
//    }
    void recogonize_protocol(double prot);

    void new_data_state(Data_states st);

    ev_io* get_current_watcher(){ return current_watcher; }
    void set_current_watcher(struct ev_io* new_watcher){ current_watcher = new_watcher; }

    struct ev_loop* get_current_loop(){ return current_loop; }
    void set_current_loop(struct ev_loop *new_loop){ current_loop = new_loop ; }

    std::vector<char>* get_message_data(){ return &message_data; }

    Language get_lang() {return lang;}
    void set_lang(Language new_lang) { lang = new_lang; }

    int get_fd_file() {return fd_file;}
    void set_fd_file(int new_fd_file){fd_file = new_fd_file;}

    int get_file_size() {return size_file;}
    void set_file_size(int new_file_size) {size_file = new_file_size ;}

    std::string_view get_protocol() {return string_ptocol;}
    double get_protoco_double(){return protocol;};
    void set_protocol(double new_proto) { protocol = new_proto;}



    void change_request_state (States_of_request st_req){ request_state = st_req; }
    void change_data_state(Data_states st){data_state_function(st);}
    void creat_answer(Data_states st){ data_state_function(st); }

    virtual void create_message();
};

void message::recogonize_protocol(double prot)
{
   if(prot == 1.0){this->string_ptocol = first_protocol;}
   if(prot == 1.1){this->string_ptocol = second_protocol;}
   if(prot == 2.0){this->string_ptocol = third_protocol;}
}

void message::new_request_state(States_of_request req_st)
{//добавить Sending
    if(req_st == States_of_request::Waiting){ request_state = States_of_request::Parsing; std::cout << "Requests state has been changed" << std::endl;}
}

void message::new_data_state(Data_states st)
{
    data_state = st;
    switch (data_state){
        case Data_states::All_Information:
         data_state_function = [this](Data_states st){this->do_all_information(st);};
             std::cout<< "State has been changed (All information)" <<std::endl;
             break;
        case Data_states::Only_main_information:
            data_state_function = [this](Data_states st){this->do_only_main_information(st);};
         std::cout<< "State has been changed (Only main information)" <<std::endl;
         break;
        case Data_states::Not_enough_main_information:
            data_state_function = [this](Data_states st){this->do_errore_message(st);}; //Как-то сюда передать loop и watcher
            data_state_function(st);
            std::cout<< "State has been changed(to disconnct)" <<std::endl;
         break;
        default:
            std::cout << "HOW?" << std::endl;
         break;
    }
}

//void message::do_all_information(Data_states st)
//{   if(st == Data_states::All_Information)
//    {
//    enter_line_withOut_newline(&message_data,protocol_,false);//enter proto                                          //
//    enter_line_withOut_newline(&message_data,/*"1.0"*/ string_ptocol /*boost::lexical_cast<std::string>(protocol)*/ /*std::to_string(std::round( (protocol*10.0) )/10.0  )*/,false);//version of protocol                     //status line
//    enter_line_withOut_newline(&message_data,good_status,true);//enter status line with transition                  //

//    enter_line_withOut_newline(&message_data,connect_status,false);                   //
//    enter_line_withOut_newline(&message_data,connect_not_keep,true);//!!                 //headers
//                                                                                      // memory leak
//    enter_line_withOut_newline(&message_data,accept_ranges,false);
//    enter_line_withOut_newline(&message_data,accept_ranges_bytes,true);//!!

//    enter_line_withOut_newline(&message_data,lenght,false);//!!//64
//    enter_line_withOut_newline(&message_data,std::to_string(size_file),false/*true*/);//

//    //enter_end_of_line_only(&message_data);
//    add_end_of_answer(&message_data);
//    }else{std::cout << "Smt else" <<std::endl;}
//}

//void message::do_only_main_information(Data_states st)
//{
//    if(st == Data_states::Only_main_information)
//    {
//    enter_line_withOut_newline(&message_data,protocol_,false);//enter proto                                          //
//    enter_line_withOut_newline(&message_data,std::to_string(protocol),false);//version of protocol                   //status line
//    enter_line_withOut_newline(&message_data,good_status,true);//enter status line with transition                   //

//    enter_line_withOut_newline(&message_data,connect_status,false);             //
//    enter_line_withOut_newline(&message_data,connect_not_keep,true);            //headers

//    enter_line_withOut_newline(&message_data,lenght,false);                     //
//    enter_line_withOut_newline(&message_data,std::to_string(size_file),true);   //

//    enter_end_of_line_only(&message_data);
//    }
//}

//void message::do_errore_messages(Data_states st)
//{
//    if(st == Data_states::Not_enough_main_information)
//    {

//        if(fd_file == -1)
//        {
//        create_not_found_response(&message_data);
//        send(current_watcher->fd ,message_data.data() , message_data.size() , MSG_NOSIGNAL);

//        std::cout << "Errore message has been send" << std::endl;
//        std::cout << "------------------------------" << std::endl;
//        }

//    close_connection(current_loop,current_watcher); //?????????????????

//    ev_timer* ptr = static_cast<ev_timer*>(current_watcher->data);//(ev_timer*) current_watcher->data;   //
//    ev_timer_stop(ev_default_loop() , ptr);             // memory leak possible

//    delete ptr->data;//Pointer to 'Connection' class    //SIGSEGV possible

//    delete ptr;                                         //
//    //this->get_message_data()->~vector(); //memory leak possible !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    }
//}


void message::create_message()
{
    switch (data_state) {
    case Data_states::All_Information:
        do_all_information(data_state);
        break;
    case Data_states::Only_main_information :
        do_only_main_information(data_state);
        break;
    default:
        std::cout<< "HOW?" <<std::endl;
        break;
    }
}

class Connection
{
private:
    ev_io *watcher;
   // ev_timer *timer; <-- а зачем он вообще нужен в классе?

public:
        ~Connection(){
        //if(watcher != NULL){delete  watcher;}
        std::cout << " Connection destroy! " << std::endl;
                 }//Удасться избежать утечки?????

    Connection(ev_io *new_watcher /*, struct ev_loop *new_loop*/){watcher = new_watcher; /*loop_ = new_loop;*/
                                                              /*timer = set_timer(new_loop)*/;}
    ev_io* get_watcher(){return watcher;}
    //struct ev_loop* get_loop(){return loop_;}

};

ev_timer* set_timer(struct ev_loop *loop,Connection *ptr)
{
    ev_timer *t = new(ev_timer);//timer
    t->data = ptr;//Save pointer to connection
    ev_timer_init(t,taimer_callback, 1.7/*5*/ , 0);//without *, because it's pointer  //cannot convert ‘Connection::taimer_callback’ from type ‘void (Connection::)(ev_loop*, ev_timer*, int)’ to type ‘void (*)(ev_loop*, ev_timer*, int)’
    ev_timer_start(loop,t);//without *, because it's pointer
    return t;
}
void taimer_callback(struct ev_loop *loop,ev_timer *timer_watcher,int revents)//они обязательны
{
    std::cout << " TIMER PING " << std::endl;

    Connection *ptr = static_cast<Connection*>(timer_watcher->data);//(Connection*)timer_watcher->data;//Чтобы обращатся к объектам класса
    if( ptr->get_watcher()->active != 0 /*!= NULL*/ /*->active != 0*/)// //Нужно нормальное условие  //!(sizeof(ptr->get_watcher()) == 0)   //Some error
    {
        close_connection(loop/* ptr->get_loop()*/ , ptr->get_watcher());
    }
    delete ptr;//ptr-> ~Connection(); | delete ptr;<-- Неявный вызов деструктор
    //timer_watcher->~ev_timer();          // | memory leak possible
    delete timer_watcher;                //Do I need to do this?
    //exit(0);//special for valgrind                                      //valgrind --leak-check=full /home/epii/build-libev_web_server-Desktop-Debug/libev_web_server
}
#endif // MESSAGE_CLASS_H
