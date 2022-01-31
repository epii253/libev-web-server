#include <fn.h>

int main()
{
    struct ev_loop *loop = ev_default_loop(0);

    int MasterSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    sockaddr_in addr = enter_addr();
    bind(MasterSocket,(struct sockaddr*)(&addr),sizeof(addr));
    listen(MasterSocket,SOMAXCONN);

    //ev_timer *timer = set_timer(loop);

    ev_io accept_watcher;//input_output
    ev_io_init(&accept_watcher,accept_watcher_cb,MasterSocket,EV_READ);
    ev_io_start(loop,&accept_watcher);

    std::cout<<"START WORK"<<std::endl;
    ev_loop(loop,0);

    //destroy_all(timer);
    ev_default_destroy();

    return 0;
}
