#include <get_command_class.h>

get_command::get_command(struct ev_loop *loop,struct ev_io *watcher,double prot,int content_fd , int content_size,Language lan)
{   set_current_loop(loop);

    set_current_watcher(watcher);

    set_fd_file(content_fd);

    set_protocol(prot);
    this->message::recogonize_protocol(prot);

    set_file_size(content_size);

    set_lang(lan);

//        if(prot){ protocol = prot; recogonize_protocol(protocol); }else{new_data_state(Data_states::Not_enough_main_information);}

//        if(content_fd){fd_file = content_fd;} else {fd_file = content_fd; new_data_state(Data_states::Not_enough_main_information);}
//        if(content_size){size_file = content_size;} else {new_data_state(Data_states::Not_enough_main_information);}

//        if(lang){ lang = lan;}else{new_data_state(Data_states::Only_main_information);}

//        if(lan && protocol){ new_data_state(Data_states::All_Information); } //????

    if(prot && content_fd && content_size && lan)
    { this->message::recogonize_protocol(get_protoco_double()) ; new_data_state(Data_states::All_Information);}
    else
    {
        new_data_state(Data_states::Not_enough_main_information);
    }
}


void get_command::do_errore_message(Data_states st)
{
    if(st == Data_states::Not_enough_main_information)
    {

        if(get_fd_file() == -1)
        {
        create_not_found_response(get_message_data());
        send(get_current_watcher()->fd , get_message_data()->data() , get_message_data()->size() , MSG_NOSIGNAL);

        std::cout << "Errore message has been send" << std::endl;
        std::cout << "------------------------------" << std::endl;
        }

    close_connection(ev_default_loop(),get_current_watcher()); //?????????????????

    ev_timer* ptr = static_cast<ev_timer*>(get_current_watcher()->data);//(ev_timer*) current_watcher->data;   //
    ev_timer_stop(ev_default_loop() , ptr);             // memory leak possible

    delete ptr->data;//Pointer to 'Connection' class    //SIGSEGV possible

    delete ptr;                                         //
    //this->get_message_data()->~vector(); //memory leak possible !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}

void get_command::do_all_information(Data_states st)
{   if(st == Data_states::All_Information)
    {
    enter_line_withOut_newline(get_message_data() , protocol_ , false);//enter proto                                          //
    enter_line_withOut_newline(get_message_data() , /*"1.0"*/ get_protocol() , false);//version of protocol                     //status line
    enter_line_withOut_newline(get_message_data() , good_status , true);//enter status line with transition                  //

    enter_line_withOut_newline(get_message_data() , connect_status , false);                   //
    enter_line_withOut_newline(get_message_data() , connect_not_keep , true);//!!                 //headers
                                                                                      // memory leak
    enter_line_withOut_newline(get_message_data() , accept_ranges , false);
    enter_line_withOut_newline(get_message_data() , accept_ranges_bytes , true);//!!

    enter_line_withOut_newline(get_message_data() , lenght , false);//!!//64
    enter_line_withOut_newline(get_message_data() , std::to_string(get_file_size()) , false/*true*/);//

    //enter_end_of_line_only(&message_data);
    add_end_of_answer(get_message_data());
    }else{std::cout << "Smt else" <<std::endl;}
}

void get_command::do_only_main_information(Data_states st)
{
    if(st == Data_states::Only_main_information)
    {
    enter_line_withOut_newline(get_message_data() , protocol_ , false);//enter proto                                          //
    enter_line_withOut_newline(get_message_data() , get_protocol() , false);//version of protocol                   //status line
    enter_line_withOut_newline(get_message_data() , good_status , true);//enter status line with transition                   //

    enter_line_withOut_newline(get_message_data() , connect_status , false);             //
    enter_line_withOut_newline(get_message_data() , connect_not_keep , true);            //headers

    enter_line_withOut_newline(get_message_data() , lenght , false);                     //
    enter_line_withOut_newline(get_message_data() , std::to_string(get_file_size()) , true);   //

    enter_end_of_line_only(get_message_data());
    }
}

void get_command::create_not_found_response(std::vector<char> *recipient)
{
    enter_line_withOut_newline(recipient , protocol_ , false);
    enter_line_withOut_newline(recipient , get_protocol()/*first_protocol*/ , false);
    enter_line_withOut_newline(recipient , bad_request , false);
    add_end_of_answer(recipient);
}
