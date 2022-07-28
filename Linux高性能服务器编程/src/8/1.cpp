/**
 * 状态独立的有限状态机
 * HTTP 请求的读取和分析
 */
#include <stdio.h>

enum CHECK_STATE
{
    CHECK_STATE_REQUEST_LINE = 0,
    CHECK_STATE_HEADER = 1
};
enum LINE_STATUS
{
    LINE_OK = 0,
    LINE_BAD,
    LINE_OPEN
};

enum HTTP_CODE
{
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

static const char *szret[] = {"I get a correct result\n", "Something wrong\n"};

LINE_STATUS parse_line(char *buffer, int &checked_index, int &read_index)
{
    char temp;
    for (checked_index; checked_index < read_index; checked_index++)
    {
        /* code */
    }
    
    return LINE_OPEN;
}

int main()
{
    printf("hello world 222\n");
    
}