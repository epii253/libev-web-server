#ifndef FN_H
#define FN_H

#include <get_command_class.cpp>
#include <post_command_class.cpp>

using namespace template_phrases;

get_command* get_responce(std::array<char,512>* BUFFER , ev_io* watcher);
std::string_view::iterator find_current_symbol_in_view(std::string_view::iterator start_position , char symbol );
void execute_request_command(int command, std::array<char, 512> *BUFFER, ev_io *watcher);
std::string *recogonize_file(const char* http_message);
Language recogonize_language(const char* http_message);
void/*bool*/ do_command_get(std::array<char, 512> *BUFFER, ev_io *watcher);
double recogonize_num_of_proto(const char* http_message);
void do_echo(struct ev_loop *loop,ev_io *watcher,int revents);
bool check_for_error(const int ReadSize);
int recogonize_comand(const char* http_message);
void do_exchange(struct ev_loop *loop,ev_io *watcher);
void close_connection(struct ev_loop *loop,ev_io *watcher);
int get_fd(const char *source);
void creat_answer(const double proto, const int size, const Language lang, std::vector<char> *p);
void enter_start_line(std::vector<char>::iterator bias);//!!!
int get_file_size(int source_size);
void enter_line_withOut_newline(std::vector <char> *recipient,std::string added,bool do_new_line);
template <typename X>
void enter_wiht_cast_into_string(std::vector <char> *recipient,X a,bool do_new_line);
message *create_mess_form(struct ev_loop *loop , ev_io *watcher, const double proto , Language lang);

sockaddr_in enter_addr()
{
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(5050);//1234
    SockAddr .sin_addr.s_addr = htonl(INADDR_LOOPBACK);//0x7f000001
    return SockAddr;
}

void taimer_cb(struct ev_loop *loop,ev_timer *w,int revents)
{
    std::cout<<"TIMER PING"<<std::endl;
}

void accept_watcher_cb(struct ev_loop *loop,ev_io *watcher,int revents)
{
    int ClientSocket = accept(watcher->fd,0,0);
    ev_io *client_read = new (ev_io);//memory leak(maybe) !!!!!
    ev_io_init(client_read,do_echo,ClientSocket,EV_READ);//waiting read event...
    ev_io_start(loop,client_read);//without *, because it's pointer

    Connection* client_connection = new Connection(client_read/*,loop*/);//memory leak possible
    ev_timer *timer = set_timer(loop,client_connection);//memory leak possible

    client_read->data = timer;//Чтобы достать и остановить
    std::cout<<"NEW CONNECTION!!!"<<std::endl;
}

void destroy_all(ev_timer *t)
{
    ev_default_destroy();//потомучто default может быть только один
    delete t;
}

void do_echo(struct ev_loop *loop,ev_io *watcher,int revents)
{
    std::cout << "Client ping" << std::endl;

    std::thread exchange_information(do_exchange,loop,watcher); //third thread

    exchange_information.join();
}

bool check_for_error(const int ReadSize)
{
    if(ReadSize > 0)
    {
        return false;//true
    }
    else
    {
        std::cout<<"Error: read size is null!!!"<<std::endl;
        return true;//false
    }
}

void do_exchange(struct ev_loop *loop,ev_io *watcher)
{
    std::array <char,512> BUFFER;//У каждого потока свой буффер
    const int ReadSize = recv(watcher->fd,BUFFER.data(),512,MSG_NOSIGNAL);

    if(check_for_error(ReadSize))
    {

        close_connection(loop,watcher);///???????????????????

        ev_timer* ptr =(ev_timer*) watcher->data;           //
        ev_timer_stop(ev_default_loop() , ptr);             // memory leak possible

        delete ptr->data;//Pointer to 'Connection' class    //SIGSEGV possible

        delete ptr;                                         //

        std::cout << "Connection has been closed (Read size is null)" << std::endl;
    }
    else
    {
        //Реализация HTTP протокола

        const int command = recogonize_comand(BUFFER.data());

//        const double proto = recogonize_num_of_proto(BUFFER.data());
//        Language lang = recogonize_language(BUFFER.data());

//        std::string* source(recogonize_file(BUFFER.data())); //memory leak possible!!!
//        const int recourse_fd = get_fd(source->data());                   //memory leak possible!!!

//        if(recourse_fd == -1)
//        {
//           std::string/*_view*/ *ptr_of_response(create_not_found_response());

//           send(watcher->fd , ptr_of_response->data() , ptr_of_response->size() , MSG_NOSIGNAL);
//           std::cout << "Errore message has been send" << std::endl;
//           delete ptr_of_response;//Returns it a memory?
//           delete source;
//           close_connection(loop,watcher);

//           return;
//        }

//        delete source; //Return memory(maybe)
//        const int file_size =  get_file_size(recourse_fd);  //Подготовка почвы для определения запрашиваемого ресурса


        //std::unique_ptr<message> ptr = std::make_unique<message>(loop , watcher , proto , recourse_fd , file_size ,lang);
        execute_request_command(command , &BUFFER , watcher);
        //читаем hhtp message

    }
}

void out_string(std::array<char,512>* ptr)
{
    for(std::array<char,512>::iterator i = ptr->begin() ; *i != '\0' ; i++)
    {
        std::cout << *i ;//<< std::endl;
    }
}

void execute_request_command(int command , std::array<char,512>* BUFFER, ev_io* watcher)
{
    switch(command)
    {
    case 1://GET

        do_command_get(BUFFER , watcher);
        //close_connection(loop,watcher);
        break;//200

    case 2://POST

//        std::cout << "========= POST request(begin) ========="<< std::endl;
//out_string(BUFFER);
//        std::cout << "========= POST request(end) =========="<< std::endl;
        do_command_post( BUFFER , watcher);     //experement

        break;//200

    default:
//        memset(BUFFER.data(), 0, sizeof(BUFFER.data()));
//        send(watcher->fd,template_phrases::bad_status.data(),3,MSG_NOSIGNAL);
//        close_connection(ev_default_loop(),watcher);

        return;
        break;
    }
}

std::string* recogonize_file(const char* http_message)
{
    std::string_view *mess = new std::string_view(http_message);

    std::vector<char> source_name;
    for(std::string_view::iterator i = (find_current_symbol_in_view(mess->begin() , '?') + 1) ; *i != ' ' ;i++)
    {
        source_name.push_back(*i);
    }
    delete mess;
//    std::string str(source_name.data());
//    return str.data();

    std::string *str = new std::string (source_name.data());
    return str;//memory leak possible !!!!!!!!
}
std::string_view::iterator find_current_symbol_in_view(std::string_view::iterator start_position , char symbol )
{
    for( ; *start_position != '\0' ; start_position++)
    {
        if(*start_position == symbol){return start_position;}
    }
    return (start_position - 2);//Some error
}
int recogonize_comand(const char* http_message)
{
   std::string mess(http_message);

   if(boost::algorithm::contains(mess,"GET") /*mess.find("GET",0)*/){return 1;}//GET
   if(boost::algorithm::contains(mess,"POST")/*mess.find("POST",0)*/){return 2;}//POST

   return 0;//501 error
}

double recogonize_num_of_proto(const char* http_message)
{
    std::string mess(http_message);
    if(mess.find("1.0",0)){return 1.0;}
    if(mess.find("1.1",0)){return 1.1;}
    if(mess.find("2.0",0)){return 2.0;}
    return 0.0;//error

}

Language recogonize_language(const char* http_message)
{
    std::string mess(http_message);
    if(mess.find("ru",0)){return Ru;}
    if(mess.find("fr",0)){return Fr;}
    if(mess.find("en",0)){return En;}
    return Ns;
}

void do_command_get(std::array<char,512>* BUFFER , ev_io* watcher)
{
    get_command* mess = get_responce(BUFFER , watcher); //function allocate memmory

    if(mess -> get_fd_file() == -1)
    {
        delete mess;
        return;
    }

    mess->create_message();
    send(watcher->fd , mess->get_message_data()->data() , mess->get_message_data()->size(),MSG_NOSIGNAL);
    sendfile(watcher->fd , mess->get_fd_file(), 0 ,mess->get_file_size()/*mes->size_file*/);

    //mess->get_message_data()->~vector();//eliminates leak of memory(WHY???)

    std::cout << "Send" << std::endl;
    std::cout << "------------------------------" << std::endl;
    delete mess;
}

get_command* get_responce(std::array<char, 512> *BUFFER , ev_io* watcher)
{
    const double proto = recogonize_num_of_proto(BUFFER->data());
    Language lang = recogonize_language(BUFFER->data());

    std::string* source(recogonize_file(BUFFER->data())); //memory leak possible!!!
    const int recourse_fd = get_fd(source->data());                   //memory leak possible!!!

    delete source; //Return memory(maybe)
    const int file_size =  get_file_size(recourse_fd);  //Подготовка почвы для определения запрашиваемого ресурса

    //std::unique_ptr<message> ptr = std::make_unique<message>(ev_default_loop() , watcher , proto , recourse_fd , file_size , lang);
    get_command *ptr = new get_command(ev_default_loop() , watcher , proto , recourse_fd , file_size , lang);
    return ptr/*ptr.get()*/;
}

int get_fd(const char* source)
{
    std::string/*_view*/ str(source);

    return open( (catalog + str).data() /*как бы в view вывести*/,O_RDONLY/*Используется если создаётся новый файл(третий параметр)*/);
}

int get_file_size(int source_size)
{
    struct/*Иначе ошибки*/ stat *file_stat = new struct stat;///???
    fstat(source_size,file_stat);
    int size = file_stat->st_size;//Total size, in bytes

    delete file_stat;
    return size;
}
#endif // FN_H
