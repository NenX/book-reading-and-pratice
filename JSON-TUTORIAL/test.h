
#ifndef TEST_H__
#define TEST_H__
#include "stdio.h"
#include "tinyjson.h"
#include "stdlib.h"
#include "string.h"
static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format)                                                          \
    do                                                                                                            \
    {                                                                                                             \
        test_count++;                                                                                             \
        if (equality)                                                                                             \
            test_pass++;                                                                                          \
        else                                                                                                      \
        {                                                                                                         \
            fprintf(stderr, "%s:%d expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual); \
            main_ret = 1;                                                                                         \
        }                                                                                                         \
                                                                                                                  \
    } while (0)
#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define EXPECT_EQ_STRING(expect, actual, len) EXPECT_EQ_BASE(memcmp(expect, actual, len) == 0 && (sizeof(expect) - 1) == len, expect, actual, "%s")

#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")

#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true", "%s")

#define PRINT_TEST_RESULT() printf("%d/%d (%3.2f %%) passed.\n", test_pass, test_count, test_pass * 100.0 / test_count)



#define TEST_ERROR(error, json)                        \
    do                                                 \
    {                                                  \
        tjson_value v;                                 \
        v.type = TJSON_FALSE;                          \
        EXPECT_EQ_INT(error, tjson_parse(&v, json));   \
        EXPECT_EQ_INT(TJSON_NULL, tjson_get_type(&v)); \
    } while (0)

#define TEST_NUMBER(expect, json)                             \
    do                                                        \
    {                                                         \
        tjson_value v;                                        \
        EXPECT_EQ_INT(TJSON_PARSE_OK, tjson_parse(&v, json)); \
        EXPECT_EQ_INT(TJSON_NUMBER, tjson_get_type(&v));      \
        EXPECT_EQ_DOUBLE(expect, tjson_get_number(&v));       \
    } while (0)

#endif /* TEST_H__ */

#define TEST_STRING(expect, json)                                                    \
    do                                                                               \
    {                                                                                \
        tjson_value v;                                                               \
        tjson_init(&v);                                                              \
        EXPECT_EQ_INT(TJSON_PARSE_OK, tjson_parse(&v, json));                        \
        EXPECT_EQ_INT(TJSON_STRING, tjson_get_type(&v));                             \
        EXPECT_EQ_STRING(expect, tjson_get_string(&v), tjson_get_string_length(&v)); \
        tjson_free(&v);                                                              \
    } while (0)
