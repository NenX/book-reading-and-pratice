

#include "1_processpool.h"
class cgi_conn
{
private:
    static const int BUFFER_SIZE = 1024;
    int m_epollfd;
    int m_sockfd;
    int m_read_idx;
    char m_buf[BUFFER_SIZE];
    sockaddr_in m_address;

public:
    cgi_conn(/* args */);
    ~cgi_conn();
    void init(int epollfd, int sockfd, sockaddr_in &addr)
    {
        m_epollfd = epollfd;
        m_sockfd = sockfd;
        m_address = addr;

        bzero(m_buf, sizeof(m_buf));
    }
    void process()
    {
        m_read_idx = 0;
        while (true)
        {
            int n = recv(m_sockfd, m_buf + m_read_idx, BUFFER_SIZE - m_read_idx - 1, 0);
            if (n < 0)
            {
                if (errno != EAGAIN)
                {
                    removefd(m_epollfd, m_sockfd);
                }
                break;
            }
            if (n == 0)
            {
                removefd(m_epollfd, m_sockfd);
                break;
            }

            int i = 0;
            m_read_idx = m_read_idx + n;
            for (; i < m_read_idx; i++)
            {
                if (i > 0 && m_buf[i] == '\n' && m_buf[i - 1] == '\r')
                {
                    break;
                }
            }

            if (i == m_read_idx)
            {
                continue;
            }
            m_buf[i - 1] = '\0';
            printf("recv:%s,%d,%d\n", m_buf, n, n);

            if (access(m_buf, F_OK) == -1)
            {
                removefd(m_epollfd, m_sockfd);
                errMsg("gg");
                break;
            }

            int ret = fork();
            if (ret < 0)
            {
                removefd(m_epollfd, m_sockfd);
                break;
            }
            if (ret == 0)
            {
                close(STDOUT_FILENO);
                dup(m_sockfd);

                execl(m_buf, m_buf, 0);
                removefd(m_epollfd, m_sockfd);
                break;
            }
            else
            {
                removefd(m_epollfd, m_sockfd);
                break;
            }
        }
    }
};

cgi_conn::cgi_conn(/* args */)
{
}

cgi_conn::~cgi_conn()
{
}

int main(int argc, char const *argv[])
{
    const char *ip;
    int port;
    int ret;
    if (argc <= 2)
    {
        ip = "127.0.0.1";
        port = 3333;
    }
    else
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_port = htons(port);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);

    ret = bind(listenfd, (sockaddr *)&address, sizeof(address));
    if (ret < 0)
    {
        err_exit("bind");
    }

    ret = listen(listenfd, 5);
    if (ret < 0)
    {
        err_exit("listen");
    }

    processpool<cgi_conn> *p = processpool<cgi_conn>::create(listenfd, 8);
    if (p)
    {
        p->run();
        delete p;
    }
    close(listenfd);
    return 0;
}
