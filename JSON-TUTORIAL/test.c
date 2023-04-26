#include "test.h"

void test_parse_null()
{
    tjson_value v;
    v.type = TJSON_FALSE;
    EXPECT_EQ_INT(TJSON_PARSE_OK, tjson_parse(&v, "null"));
    EXPECT_EQ_INT(TJSON_NULL, tjson_get_type(&v));
}
void test_parse_true()
{
    tjson_value v;
    v.type = TJSON_NULL;
    EXPECT_EQ_INT(TJSON_PARSE_OK, tjson_parse(&v, "true"));
    EXPECT_EQ_INT(TJSON_TRUE, tjson_get_type(&v));
}
void test_parse_false()
{
    tjson_value v;
    v.type = TJSON_NULL;
    EXPECT_EQ_INT(TJSON_PARSE_OK, tjson_parse(&v, "false"));
    EXPECT_EQ_INT(TJSON_FALSE, tjson_get_type(&v));
}
void test_parse_expect_value()
{
    TEST_ERROR(TJSON_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(TJSON_PARSE_EXPECT_VALUE, " ");
}
void test_parse_invalid_valie()
{
    TEST_ERROR(TJSON_PARSE_INVALID_VALUE, "flase");
}
void test_parse_root_not_singular()
{
    TEST_ERROR(TJSON_PARSE_ROOT_NOT_SINGULAR, "true x");
}
static void test_parse_number()
{
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}

static void test_parse_string()
{
    TEST_STRING("1\n1", "\"1\n1\"");
    TEST_STRING("Hello", "\"Hello\"");
#if 0
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
#endif
}
static void test_access_boolean()
{
    tjson_value v;
    tjson_init(&v);
    tjson_set_string(&v, "a", 1);
    tjson_set_boolean(&v, 1);
    EXPECT_TRUE(tjson_get_boolean(&v));
    tjson_set_boolean(&v, 0);
    EXPECT_FALSE(tjson_get_boolean(&v));
    tjson_free(&v);
}

static void test_access_number()
{
    tjson_value v;
    tjson_init(&v);
    tjson_set_string(&v, "a", 1);
    tjson_set_number(&v, 1234.5);
    EXPECT_EQ_DOUBLE(1234.5, tjson_get_number(&v));
    tjson_free(&v);
}
void test_parse()
{
    test_parse_string();
    test_access_boolean();
    test_access_number();
    test_parse_number();
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_valie();
    test_parse_root_not_singular();
}

int main(int argc, char const *argv[])
{
#if 1
    test_parse();

    PRINT_TEST_RESULT();
#endif
    return 0;
}
