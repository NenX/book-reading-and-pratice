#include "tinyjson.h"
#include <errno.h>  /* errno, ERANGE */
#include <math.h>   /* HUGE_VAL */
#include <stdlib.h> /* NULL, malloc(), realloc(), free(), strtod() */
#include <string.h> /* memcpy() */

#ifndef TJSON_PARSE_STACK_INIT_SIZE
#define TJSON_PARSE_STACK_INIT_SIZE 256
#endif

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)                                         \
    do                                                      \
    {                                                       \
        *(char *)lept_context_push(c, sizeof(char)) = (ch); \
    } while (0)
#define STRING_ERROR(ret) \
    do                    \
    {                     \
        c->top = head;    \
        return ret;       \
    } while (0)

void expect_and_move_to_next(tjson_context *ctx, char c)
{
    assert(*ctx->json == c);
    ctx->json++;
}

void tjson_parse_utf8(tjson_context *c, unsigned u)
{
    if (u <= 0x7F)
    {
        PUTC(c, u & 0xFF);
    }
    else if (u <= 0x7FF)
    {
        PUTC(c, ((u >> 6) & 0x1F) | 0xC0);
        PUTC(c, (u & 0x3F) | 0x80);
    }
    else if (u <= 0xFFFF)
    {
        PUTC(c, ((u >> 12) & 0xF) | 0xE0);
        PUTC(c, ((u >> 6) & 0x3F) | 0x80);
        PUTC(c, (u & 0x3F) | 0x80);
    }
    else
    {
        assert(u <= 0x10FFFF);

        PUTC(c, ((u >> 18) & 0x7) | 0xF0);
        PUTC(c, ((u >> 12) & 0x3F) | 0x80);
        PUTC(c, ((u >> 6) & 0x3F) | 0x80);
        PUTC(c, (u & 0x3F) | 0x80);
    }
}

void tjson_free(tjson_value *v)
{
    assert(v != NULL);
    if (v->type == TJSON_STRING)
        free(v->u.s.s);
    v->type = TJSON_NULL;
}
void tjson_init(tjson_value *v)
{
    assert(v != NULL);
    v->type = TJSON_NULL;
}

void *tjson_context_push(tjson_context *c, size_t len)
{
    assert(c != NULL && len != 0);

    if (c->top + len >= c->size)
    {
        if (c->size == 0)
            c->size = TJSON_PARSE_STACK_INIT_SIZE;

        while (c->top + len >= c->size)
            c->size += c->size >> 1;
        c->stack = realloc(c->stack, c->size);
    }
    void *ret = c->stack + c->top;
    c->top += len;
    return ret;
}
void *tjson_context_pop(tjson_context *c, size_t len)
{
    assert(c != NULL && c->top >= len);
    return c->stack + (c->top -= len);
}

tjson_type tjson_get_type(const tjson_value *v)
{
    assert(v != NULL);

    return v->type;
}

double tjson_get_number(const tjson_value *v)
{
    assert(v != NULL && v->type == TJSON_NUMBER);
    return v->u.n;
}

void tjson_set_number(tjson_value *v, double n)
{
    assert(v != NULL);
    tjson_free(v);

    v->u.n = n;
    v->type = TJSON_NUMBER;
}
int tjson_get_boolean(const tjson_value *v)
{
    assert(v != NULL);

    assert(v->type == TJSON_FALSE || v->type == TJSON_TRUE);
    return v->type == TJSON_TRUE;
}
void tjson_set_boolean(tjson_value *v, int b)
{
    assert(v != NULL);
    tjson_free(v);

    v->type = b ? TJSON_TRUE : TJSON_FALSE;
}

void tjson_set_string(tjson_value *v, const char *s, size_t len)
{
    assert(v != NULL && (s != NULL || len == 0));

    tjson_free(v);

    v->u.s.s = (char *)malloc(len + 1);
    assert(v->u.s.s != NULL);
    memcpy(v->u.s.s, s, len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;

    v->type = TJSON_STRING;
}

const char *tjson_get_string(const tjson_value *v)
{
    assert(v != NULL && v->type == TJSON_STRING);
    return v->u.s.s;
}

size_t tjson_get_string_length(const tjson_value *v)
{
    assert(v != NULL);
    assert(v->type == TJSON_STRING);
    return v->u.s.len;
}

void tjson_parse_whitespace(tjson_context *c)
{
    const char *p = c->json;

    while (*p == '\n' || *p == '\t' || *p == '\r' || *p == ' ')
    {
        p++;
    }
    c->json = p;
}

tjson_parse_type tjson_parse_string(tjson_context *c, tjson_value *v)
{
    assert(c != NULL && v != NULL);
    expect_and_move_to_next(c, '\"');

    size_t strlen;
    size_t head = c->top;
    char *p_tmp;
    char ch;
    const char *p_next = c->json;

    while (1)
    {

        switch (ch = *p_next++)
        {
        case '\"':
            strlen = c->top - head;
            tjson_set_string(v, tjson_context_pop(c, strlen), strlen);
            c->json = p_next;

            return TJSON_PARSE_OK;
        case '\0':
            STRING_ERROR(TJSON_PARSE_MISS_QUOTATION_MARK);
        case '\\':
            switch (ch = *p_next++)
            {
            case 't':
                PUTC(c, '\t');
                break;
            case 'r':
                PUTC(c, '\r');
                break;
            case 'n':
                PUTC(c, '\n');
                break;
            case 'f':
                PUTC(c, '\f');
                break;
            case 'b':
                PUTC(c, '\b');
                break;
            case '\\':
                PUTC(c, '\\');
                break;
            case '/':
                PUTC(c, '/');
                break;
            case '"':
                PUTC(c, '"');
                break;
            default:
                STRING_ERROR(TJSON_PARSE_INVALID_STRING_ESCAPE);
            }

        default:
            p_tmp = tjson_context_push(c, sizeof(char));
            *p_tmp = ch;
        }
    }

    return TJSON_PARSE_OK;
}
tjson_parse_type tjson_parse_literal(tjson_context *c, tjson_value *v, const char *literal, tjson_type type)
{
    char *str = c->json;
    expect_and_move_to_next(c, literal[0]);
    size_t i;

    for (i = 1; literal[i]; i++)
    {
        if (literal[i] != str[i])
            return TJSON_PARSE_INVALID_VALUE;
    }

    c->json += i - 1;
    v->type = type;
    return TJSON_PARSE_OK;
}

tjson_parse_type tjson_parse_number(tjson_context *c, tjson_value *v)
{
    char *p_next = c->json;
    if (*p_next == '-')
        p_next++;
    if (*p_next == '0')
        p_next++;
    else
    {
        if (!ISDIGIT1TO9(*p_next))
            return TJSON_PARSE_INVALID_VALUE;
        while (ISDIGIT(*p_next))
            p_next++;
    }
    if (*p_next == '.')
    {
        p_next++;
        if (!ISDIGIT(*p_next))
            return TJSON_PARSE_INVALID_VALUE;
        while (ISDIGIT(*p_next))
            p_next++;
    }
    if (*p_next == 'e' || *p_next == 'E')
    {
        p_next++;
        if (*p_next == '+' || *p_next == '-')
            p_next++;
        if (!ISDIGIT(*p_next))
            return TJSON_PARSE_INVALID_VALUE;
        while (ISDIGIT(*p_next))
            p_next++;
    }
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return TJSON_PARSE_NUMBER_TOO_BIG;
    v->type = TJSON_NUMBER;
    c->json = p_next;
    return TJSON_PARSE_OK;
}

tjson_parse_type tjson_parse_value(tjson_context *c, tjson_value *v)
{
    switch (*c->json)
    {
    case 'n':
        return tjson_parse_literal(c, v, "null", TJSON_NULL);
    case 't':
        return tjson_parse_literal(c, v, "true", TJSON_TRUE);
    case 'f':
        return tjson_parse_literal(c, v, "false", TJSON_FALSE);
    case '\0':
        return TJSON_PARSE_EXPECT_VALUE;

    case '\"':
        return tjson_parse_string(c, v);
    default:
        return tjson_parse_number(c, v);
    }
}
tjson_parse_type tjson_parse(tjson_value *v, const char *json)
{
    tjson_context c;
    assert(v != NULL);
    tjson_parse_type result;
    c.json = json;
    c.size = c.top = 0;
    c.stack = NULL;
    tjson_init(v);
    tjson_parse_whitespace(&c);
    if ((result = tjson_parse_value(&c, v)) == TJSON_PARSE_OK)
    {
        tjson_parse_whitespace(&c);
        if (*c.json != '\0')
        {
            v->type = TJSON_NULL;
            return TJSON_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);
    free(c.stack);
    return result;
}
