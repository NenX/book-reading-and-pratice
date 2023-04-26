
#ifndef TINYJSON_H__
#define TINYJSON_H__

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
typedef enum
{
    TJSON_NULL,
    TJSON_FALSE,
    TJSON_TRUE,
    TJSON_STRING,
    TJSON_NUMBER,
    TJSON_OBJECT,
    TJSON_ARRAY,
} tjson_type;

typedef struct
{
    tjson_type type;
    union tinyjson
    {
        double n;
        struct
        {
            size_t len;
            char *s;
        } s;
    } u;

} tjson_value;

typedef enum
{
    TJSON_PARSE_OK = 0,
    TJSON_PARSE_EXPECT_VALUE,
    TJSON_PARSE_INVALID_VALUE,
    TJSON_PARSE_ROOT_NOT_SINGULAR,
    TJSON_PARSE_NUMBER_TOO_BIG,

    TJSON_PARSE_MISS_QUOTATION_MARK,
    TJSON_PARSE_INVALID_STRING_ESCAPE,
    TJSON_PARSE_INVALID_STRING_CHAR,
    TJSON_PARSE_INVALID_STRING_CHAR,
    TJSON_PARSE_INVALID_UNICODE_HEX,
    TJSON_PARSE_INVALID_UNICODE_SURROGATE
} tjson_parse_type;

typedef struct
{
    const char *json;
    size_t top, size;
    char *stack;
} tjson_context;

tjson_parse_type tjson_parse(tjson_value *v, const char *json);

tjson_type tjson_get_type(const tjson_value *v);

double tjson_get_number(const tjson_value *v);
void tjson_set_number(tjson_value *v, double n);

int tjson_get_boolean(const tjson_value *v);
void tjson_set_boolean(tjson_value *v, int b);

const char *tjson_get_string(const tjson_value *v);
size_t tjson_get_string_length(const tjson_value *v);
void tjson_set_string(tjson_value *v, const char *s, size_t len);

void tjson_free(tjson_value *v);
void tjson_init(tjson_value *v);
#endif /* TINYJSON_H__ */