#include "override_global_new_and_delete.hpp"

#include "utl/function.hpp"

#include "tested_functions_and_function_objects.hpp"
#include "basic_utl_function_test_fixture.hpp"

#include <cassert>
#include <type_traits>
#include <utility>

class test_utl_function_case_03_move_construction : public basic_utl_function_test
{
public:
  template<typename FunctionOrFunctionObject>
  void do_test(FunctionOrFunctionObject& input_callable, const expected_storage_type expected_storage) const
  {
    static_assert(!std::is_const_v<FunctionOrFunctionObject>);

    if constexpr (is_std_reference_wrapper<FunctionOrFunctionObject>())
    {
      using wrapped_type = typename FunctionOrFunctionObject::type;
      static_assert(!std::is_const_v<wrapped_type>);

      assert(expected_storage == expected_storage_type::in_place);
      do_test_impl(input_callable, expected_storage);
      do_test_impl(std::as_const(input_callable), expected_storage);
      if constexpr (!std::is_function_v<wrapped_type>)
      {
        std::reference_wrapper<const wrapped_type> copied_wrapper_of_const_callable = input_callable;
        do_test_impl(copied_wrapper_of_const_callable, expected_storage);
        do_test_impl(std::as_const(copied_wrapper_of_const_callable), expected_storage);
      }
    }
    else if constexpr (std::is_class_v<FunctionOrFunctionObject>)
    {
      do_test_impl(input_callable, expected_storage);
      do_test_impl(std::as_const(input_callable), expected_storage);
    }
    else
    {
      static_assert(std::is_function_v<FunctionOrFunctionObject>);
      assert(expected_storage == expected_storage_type::in_place);
      auto function_pointer = &input_callable;
      do_test_impl(input_callable, expected_storage);
      do_test_impl(function_pointer, expected_storage);
      do_test_impl(std::as_const(function_pointer), expected_storage);
    }
  }

private:
  template<typename FunctionOrFunctionObject>
  void do_test_impl(FunctionOrFunctionObject& input_callable, const expected_storage_type expected_storage) const
  {
    unsigned int &      callable_invoked_counter          = access_invoked_counter<FunctionOrFunctionObject>();
    unsigned int *const callable_move_constructed_counter = access_move_constructed_counter_if_applicable<FunctionOrFunctionObject>();

    const unsigned int expected_counter_of_allocations = (expected_storage == expected_storage_type::in_place) ? 0 : 1;

    const unsigned int expected_move_constructed_counter_increment_if_applicable = (callable_move_constructed_counter == nullptr) ? 0
      : ((expected_storage == expected_storage_type::in_place) ? 1 : 0);

    // reset the global counters which are going to be modified by this test
    callable_invoked_counter = 0;
    counter_of_allocations_done_by_operator_new = 0;

    // create a new instance of the `function` template from the input callable (plain function / function object / reference wrapper)
    utl::function<void()> original_function = input_callable;

    EXPECT_EQ(counter_of_allocations_done_by_operator_new, expected_counter_of_allocations);
    EXPECT_TRUE(static_cast<bool>(original_function));

    const unsigned int original_move_constructed_counter =
      (callable_move_constructed_counter != nullptr) ? *callable_move_constructed_counter : 0;

    // perform the move construction of the `function` template instance
    auto copy_of_function = std::move(original_function);

    // for the objects allocated inside the small buffer of a `function`,
    // their move constructor should be called whenever moving a `function`;
    // for the objects which are allocated dynamically, their move constructor should not be called
    // because the `function` will just move a pointer around;
    // cannot perform this check in case the wrapped callable is a plain function pointer or a reference wrapper
    // because those don't have (observable) move constructors
    if (callable_move_constructed_counter != nullptr)
    {
      const auto expected_counter_value = original_move_constructed_counter + expected_move_constructed_counter_increment_if_applicable;
      EXPECT_EQ(*callable_move_constructed_counter, expected_counter_value);
    }

    EXPECT_EQ(callable_invoked_counter, 0);
    EXPECT_FALSE(static_cast<bool>(original_function));
    ASSERT_TRUE(static_cast<bool>(copy_of_function));
    copy_of_function();
    EXPECT_EQ(callable_invoked_counter, 1);
    ASSERT_TRUE(static_cast<bool>(copy_of_function)); // must still be valid after being invoked, thus it can be invoked again
    copy_of_function();
    EXPECT_EQ(callable_invoked_counter, 2);
    EXPECT_TRUE(static_cast<bool>(copy_of_function));

    EXPECT_EQ(counter_of_allocations_done_by_operator_new, expected_counter_of_allocations); // should not have changed
  }
};

TEST_F(test_utl_function_case_03_move_construction,
       function_can_be_move_constructed_and_move_ctor_is_invoked_for_small_objects_but_not_for_dynamically_allocated_ones)
{
  {
    small_function_object input_callable{};
    do_test(input_callable, expected_storage_type::in_place);
  }
  {
    throwing_function_object input_callable{};
    do_test(input_callable, expected_storage_type::dynamic);
  }
  {
    large_function_object input_callable{};
    do_test(input_callable, expected_storage_type::dynamic);
  }
  {
    overaligned_function_object input_callable{};
    do_test(input_callable, expected_storage_type::dynamic);
  }
  {
    do_test(global_function,          expected_storage_type::in_place);
    do_test(global_function_noexcept, expected_storage_type::in_place);
  }
  {
    small_function_object input_callable{};
    auto callable_ref = std::ref(input_callable);
    do_test(callable_ref, expected_storage_type::in_place);
  }
  {
    throwing_function_object input_callable{};
    auto callable_ref = std::ref(input_callable);
    do_test(callable_ref, expected_storage_type::in_place);
  }
  {
    large_function_object input_callable{};
    auto callable_ref = std::ref(input_callable);
    do_test(callable_ref, expected_storage_type::in_place);
  }
  {
    overaligned_function_object input_callable{};
    auto callable_ref = std::ref(input_callable);
    do_test(callable_ref, expected_storage_type::in_place);
  }
  {
    auto callable_ref = std::ref(global_function);
    do_test(callable_ref, expected_storage_type::in_place);
  }
  {
    auto callable_ref = std::ref(global_function_noexcept);
    do_test(callable_ref, expected_storage_type::in_place);
  }
}
