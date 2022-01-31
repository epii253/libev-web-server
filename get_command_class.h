#ifndef GET_COMMAND_CLASS_H
#define GET_COMMAND_CLASS_H

#include <message_class.h>

class get_command : public message
{
    virtual void do_all_information(Data_states st);        //
    virtual void do_only_main_information(Data_states st);  // Нужно ли писать ' virtual ' ?
    virtual void do_errore_message(Data_states st);        //

public:

    void create_not_found_response(std::vector<char>* recipient);

    get_command( struct ev_loop *loop,struct ev_io *watcher,double prot,int content_fd , int content_size,Language lan );
};


#endif // GET_COMMAND_CLASS_H
