#ifndef TEMPLATE_PHRASES_NAMESPACE_H
#define _H

#include <string_view>
#include <string>
#include <vector>

namespace template_phrases {

    const std::string_view good_status = " 200 OK";//6 symbols
    const std::string_view protocol_ = "HTTP/";//5 symbols
    const std::string_view bad_status = " 501 error";
    const std::string_view bad_request = " 404 Not Found";

    const std::string_view connect_status = "Connection: ";// 12 symbols
    const std::string_view connect_keep = "Keep alive";//10
    const std::string_view connect_not_keep = "close";//5


    const std::string_view host = "Host: ";//6 symbols
    const std::string_view server_name = "";//¯\_(ツ)_/¯

    const std::string_view alive_status = "Keep-Alive: ";//¯\_(ツ)_/¯

    const std::string_view accept_ranges = "Accept-ranges: ";
    const std::string_view accept_ranges_bytes = "bytes";

    const std::string_view lenght = "Content-Lenght: ";//16 symbols

    const std::string_view date = "Date: ";//6 symbols

    const std::string_view end_of_line = "\r\n";//4 - 5 symbols
    const std::string_view end_of_message = "\r\n\r\n";//8 symbols

    const std::string_view first_protocol = "1.0";
    const std::string_view second_protocol = "1.1";
    const std::string_view third_protocol = "2.0";

   const std::string/*_view*/ catalog = "/home/epii/libev_web_server/sources/";

    void enter_end_of_line_only(std::vector <char> *recipient)
    {
        for(std::string_view::iterator i = end_of_line.begin();*i != '\0'/*end_of_message.end() */;i++){ recipient->push_back(*i);}//Конец строки
    }

    void add_end_of_answer(std::vector <char> *recipient)
    {
        for(std::string_view::iterator i = end_of_message.begin() ; *i != '\0'/*end_of_message.end() */; i++)
        { recipient->push_back(*i); }//end of message
    }

    void enter_line_withOut_newline(std::vector <char> *recipient/*std::vector<char>::iterator recipient*/,std::string_view added,bool do_new_line)
    {
        for(std::string_view::iterator i = added.begin(); *i != '\0' /*added.end()*/;i++)
        { recipient->push_back(*i);/**recipient = *i ; recipient++;*/}

        if(do_new_line){ for(std::string_view::iterator i = end_of_line.begin();i != end_of_line.end();i++){ recipient->push_back(*i);}}//Конец строки
    }

}          
#endif // TEMPLATE_PHRASES_NAMESPACE_H
