#ifndef POST_COMMAND_CLASS_H
#define POST_COMMAND_CLASS_H

#include <iostream>
#include <string>
#include <ev++.h>
#include <array>
#include <memory>
#include <sys/socket.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include <message_class.h>

#define SIZE_OF_BUFFER 25600

//std::array< uint32_t, SIZE_OF_BUFFER >* recv_file( int fd)
//{
//    std::array< uint32_t, SIZE_OF_BUFFER >* FILE = new std::array <uint32_t , SIZE_OF_BUFFER>;
//    //std::unique_ptr< std::array< uint8_t, 102400 > > ptr = std::make_unique< std::array< uint8_t, 102400 > >;
//    recv(fd , FILE->data() , /*12800*/SIZE_OF_BUFFER*4 , MSG_NOSIGNAL); //endless read

//    std::cout << "Image has been get" << std::endl;

//    return FILE;
//}

//std::string::iterator get_iter_of_start(std::string::iterator begin , size_t start)
//{
//    for( ; start != 0 ; start --)
//    {
//        begin++;
//    }

//    return begin;
//}

//size_t get_end(size_t i , std::string::iterator begin)
//{
//    size_t end = i;

//    for(int k = i ; k != 0 ; i--)
//    {
//        begin++;
//    }

//    for( ; *begin != 'r'; begin++)
//    {
//        end++;
//    }
//    end -= 2;

//   return end;
//}

//std::size_t find_size_of_content( std::array <char , 512>* BUFFER )
//{
//    std::string* mess = new std::string(BUFFER->data());

//    size_t start = mess->find("Content-Lenght: ");start += 16; //get_position of start
//    size_t end = get_end(start , mess->begin());               //get position of end

//    std::string str( *mess/*refference to string*/ , start , end - start);

//    size_t size = boost::lexical_cast<int>(str);

//    delete mess; //return memory

//    return size;
//}

class post_command : public message
{
    virtual void do_all_information(Data_states st);        //
    virtual void do_only_main_information(Data_states st);  // Нужно ли писать ' virtual ' ?
    virtual void do_errore_message(Data_states st);        //

    void recv_file( int fd );

    std::string::iterator get_iter_of_start(std::string::iterator begin , size_t start);
    size_t get_end(size_t i , std::string::iterator begin);

    std::size_t find_size_of_content( std::array <char , 512>* BUFFER );

    std::array<uint32_t , SIZE_OF_BUFFER> content;
    std::size_t file_size;


public:

    post_command( struct ev_loop* loop , struct ev_io* watcher , std::array <char , 512>* BUFFER);

    std::size_t get_file_size(){return file_size;}

};

void do_command_post(std::array <char , 512>* BUFFER, ev_io* watcher)
{
    post_command* post = new post_command(ev_default_loop() , watcher , BUFFER);
    //std::array< uint32_t, SIZE_OF_BUFFER >* FILE = recv_file(watcher->fd);

    //size_t size = find_size_of_content(BUFFER);

    if(post->get_file_size())
    {

    std::string catalog = "/home/epii/libev_web_server/sources/";

    std::ofstream stream( catalog + "A.png" , std::ofstream::trunc);

    //stream.write(FILE->data() , std::ceil(size/4)); // | doesn't work
    //stream.write(FILE->data() , size); //              |
    }

    delete post;
}

#endif // POST_COMMAND_CLASS_H
