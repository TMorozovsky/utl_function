#include "override_global_new_and_delete.hpp"

#include "utl/function.hpp"

#include "tested_functions_and_function_objects.hpp"
#include "basic_utl_function_test_fixture.hpp"

class test_utl_function_case_01_default_construction : public basic_utl_function_test
{
public:
  test_utl_function_case_01_default_construction() = default;

  ~test_utl_function_case_01_default_construction()
  {
    EXPECT_EQ(counter_of_allocations_done_by_operator_new, 0);
  }
};

TEST_F(test_utl_function_case_01_default_construction, construct_without_anything)
{
  utl::function<void()> function;
  EXPECT_FALSE(function);
  EXPECT_TRUE(!function);
}

TEST_F(test_utl_function_case_01_default_construction, construct_with_empty_braces)
{
  utl::function<void()> function{};
  EXPECT_FALSE(function);
  EXPECT_TRUE(!function);
}

TEST_F(test_utl_function_case_01_default_construction, construct_with_eq_empty_braces)
{
  utl::function<void()> function = {};
  EXPECT_FALSE(function);
  EXPECT_TRUE(!function);
}

TEST_F(test_utl_function_case_01_default_construction, construct_with_nullptr_in_braces)
{
  utl::function<void()> function{nullptr};
  EXPECT_FALSE(function);
  EXPECT_TRUE(!function);
}

TEST_F(test_utl_function_case_01_default_construction, construct_with_nullptr_in_parentheses)
{
  utl::function<void()> function(nullptr);
  EXPECT_FALSE(function);
  EXPECT_TRUE(!function);
}

TEST_F(test_utl_function_case_01_default_construction, construct_with_eq_nullptr)
{
  utl::function<void()> function = nullptr;
  EXPECT_FALSE(function);
  EXPECT_TRUE(!function);
}
