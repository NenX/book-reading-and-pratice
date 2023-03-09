#ifndef HTTP_CONN_H
#define HTTP_CONN_H
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
class http_conn
{
private:
    /* data */
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;

    enum METHOD
    {
        GET = 0,
        POST,
        PUT,
        DELETE,
        PATCH,
        TRACE,
        OPTION,
        HEAD
    };
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    static int m_epollfd;
    // 统计用户数量
    static int m_user_count;

public:
    http_conn(/* args */);
    ~http_conn();
    // 初始化新接受的链接
    void init(int sockfd, sockaddr_in &add);
    // 关闭连接
    void close_conn(bool real_close = true);
    // 处理客户请求
    void process();
    // 非阻塞读操作
    bool read();
    // 非阻塞写操作
    bool write();

private:
    // 读 HTTP 链接的 socket 和对方的 socket 地址
    int m_sockfd;
    sockaddr_in m_address;

    // 读缓冲区
    char m_read_buf[READ_BUFFER_SIZE];

    // 读缓冲区中已经读入的用户数据的最后一个字节的下一个位置
    int m_read_idx;

    // 当前正在分析的字符在读缓冲区的位置
    int m_checket_idx;

    // 当前正在解析的行的起始位置
    int m_start_line;

    // 写缓冲区
    char m_write_buf[WRITE_BUFFER_SIZE];

    // 写缓冲区待发送的字节数
    int m_write_idx;

    // 主缓冲区当前所处的状态
    CHECK_STATE m_check_state;

    // 请求方法
    METHOD m_method;

    // 客户请求的目标文件的完整路径, 等于doc_root + m_url, doc_root 是网站根目录
    char m_real_file[FILENAME_LEN];

    // 客户请求的目标文件的文件名
    char *m_url;

    // 主机名
    char *m_host;

    // HTTP 协议版本号, 我们仅支持 HTTP/1.1
    char *m_version;

    // HTTP 请求消息体的长度
    int m_content_length;

    // HTTP 请求是否要求保持连接
    bool m_linger;

    // 客户请求的文件被 mmap 到内存中的起始位置
    int m_file_address;

    // 目标文件的状态
    struct stat m_file_stat;

    // 我们将采用 readv 来执行写操作
    struct iovec m_iv;
    int m_iv_count;

    // 初始化连接
    void init();

    // 解析 HTTP 请求
    HTTP_CODE process_read();

    // 填充 HTTP 应答
    bool process_write(HTTP_CODE ret);

    // 下面一组函数被 process_read 调用以分析 HTTP 请求

    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request(char *text);
    char *get_line() { return m_read_buf + m_start_line; };
    LINE_STATUS parse_line();

    // 下面一组函数被 procee_write 调用以填充 HTTP 应答
    void unmap();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();
};

http_conn::http_conn(/* args */)
{
}

http_conn::~http_conn()
{
}

const char *ok_200_title = "OK";
const char *error_400_title = "400 Bad Request";
const char *error_400_form = "400 Bad Request";
const char *error_403_title = "403 Bad Request";
const char *error_403_form = "403 Bad Request";
const char *error_404_title = "404 Bad Request";
const char *error_404_form = "404 Bad Request";
const char *error_500_title = "500 Bad Request";
const char *error_500_form = "500 Bad Request";
const char *doc_root = "/var/www/html";

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd, bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (one_shot)
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
}
void modfd(int epollfd, int fd, int env)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = env | EPOLLIN | EPOLLET | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int http_conn::m_epollfd = -1;
int http_conn::m_user_count = 0;

void http_conn::close_conn(bool real_close)
{
    if (real_close && (m_sockfd != -1))
    {
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;
    }
}

void http_conn::init(int sockfd, sockaddr_in &addr)
{
    m_sockfd = sockfd;
    m_address = addr;

    if (true)
    {
        int reuse = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    }
    addfd(m_epollfd, m_sockfd, true);
    m_user_count++;
}

void http_conn::init()
{
    m_check_state = CHECK_STATE_REQUESTLINE;

    m_linger = false;

    m_method = GET;
    m_url = NULL;
    m_version = NULL;
    m_host = NULL;
    m_content_length = 0;
    m_checket_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    m_start_line = 0;

    memset(m_read_buf, '\0', READ_BUFFER_SIZE);
    memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
    memset(m_real_file, '\0', FILENAME_LEN);
}

bool http_conn::read()
{

    int bytes_read = 0;

    if (m_read_idx >= READ_BUFFER_SIZE)
    {
        return false;
    }
    while (true)
    {
        bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0);
        if (bytes_read < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            return false;
        }
        if (bytes_read == 0)
        {
            return false;
        }
        m_read_idx += bytes_read;
    }
    return true;
}

http_conn::LINE_STATUS http_conn::parse_line()
{
    char tmp;

    for (; m_checket_idx < m_read_idx; m_checket_idx++)
    {
        tmp = m_read_buf[m_checket_idx];
        if (tmp == '\r')
        {
            if (m_checket_idx + 1 == m_read_idx)
            {
                return LINE_OPEN;
            }
            if (m_read_buf[m_checket_idx + 1] == '\n')
            {
                m_read_buf[m_checket_idx++] = '\0';
                m_read_buf[m_checket_idx++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        if (tmp == '\n')
        {
            if (m_checket_idx > 0 && m_read_buf[m_check_state - 1] == '\r')
            {
                m_read_buf[m_checket_idx - 1] = '\0';
                m_read_buf[m_checket_idx++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

http_conn::HTTP_CODE http_conn::parse_request_line(char *text)
{
    m_url = strpbrk(text, " \t");

    if (!m_url)
    {
        return BAD_REQUEST;
    }
    *m_url++ = '\0';

    if (strcasecmp(text, "GET") != 0)
    {
        return BAD_REQUEST;
    }
    m_method = GET;

    m_url += strspn(m_url, " \t");

    m_version = strpbrk(m_url, " \t");

    if (!m_version)
    {
        return BAD_REQUEST;
    }
    *m_version++ = '\0';
    m_version += strspn(m_version, " \t");
    if (strcasecmp(m_version, "HTTP/1.1") != 0)
    {
        return BAD_REQUEST;
    }
    if (strncasecmp(m_url, "http://", 7) == 0)
    {
        m_url += 7;
        m_url = strchr(m_url, '/');
    }
    if (!m_url || m_url[0] != '/')
    {
        return BAD_REQUEST;
    }

    m_check_state = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::parse_headers(char *text)
{
    if (text && text[0] == '\0')
    {
        if (m_content_length != 0)
        {
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        return GET_REQUEST;
    }
    else if (strncasecmp(text, "Connection:", 11) == 0)
    {
        text += 11;
        text += strspn(text, " \t");
        if (strcasecmp(text, "keep-alive"))
        {
            m_linger = true;
        }
    }
    else if (strncasecmp(text, "Content-Length:", 15) == 0)
    {
        text += 15;
        text += strspn(text, " \t");
        m_content_length = atol(text);
    }
    else if (strncasecmp(text, "Host:", 5) == 0)
    {
        text += 5;
        text += strspn(text, " \t");

        m_host = text;
    }
    else
    {
        printf("oop! unknown header: %s\n", text);
    }
    return NO_REQUEST;
}
http_conn::HTTP_CODE http_conn::parse_content(char *text)
{
    if (m_read_idx >= (m_checket_idx + m_content_length))
    {
        text[m_content_length] = '\0';
        return GET_REQUEST;
    }
    return NO_REQUEST;
}

#endif