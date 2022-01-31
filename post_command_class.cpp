#include <post_command_class.h>

post_command::post_command( struct ev_loop* loop , struct ev_io* watcher , std::array <char , 512>* BUFFER)
{
    //set_request_message(BUFFER);

    this->message::set_current_loop(loop);
    this->message::set_current_watcher(watcher);

    recv_file(watcher->fd);
    file_size = find_size_of_content( BUFFER );

    if(!file_size){new_data_state(Data_states::Not_enough_main_information);}
}

void post_command::recv_file( int fd)
{
    recv(fd , content.data() , /*12800*/SIZE_OF_BUFFER*4 , MSG_NOSIGNAL); //endless read

    std::cout << "Image has been get" << std::endl;

    //return FILE;
}

std::string::iterator post_command::get_iter_of_start(std::string::iterator begin , size_t start)
{
    for( ; start != 0 ; start --)
    {
        begin++;
    }

    return begin;
}

size_t post_command::get_end(size_t i , std::string::iterator begin)
{
    size_t end = i;

    for(int k = i ; k != 0 ; i--)
    {
        begin++;
    }

    for( ; *begin != 'r'; begin++)
    {
        end++;
    }
    end -= 2;

   return end;
}

std::size_t post_command::find_size_of_content( std::array <char , 512>* BUFFER )
{
    std::string* mess = new std::string(BUFFER->data());

    size_t start = mess->find("Content-Lenght: ");start += 16; //get_position of start
    size_t end = get_end(start , mess->begin());               //get position of end

    std::string str( *mess/*refference to string*/ , start , end - start);

    size_t size = boost::lexical_cast<int>(str);

    delete mess; //return memory

    return size;
}

void post_command::do_all_information(Data_states st)
{
    std::cout << "Do_all_information" << std::endl;
}

void post_command::do_errore_message(Data_states st)
{
    std::cout << "Do_errore_message" << std::endl;
}

void post_command::do_only_main_information(Data_states st)
{
    std::cout << "Do_only_main_information" << std::endl;
}
