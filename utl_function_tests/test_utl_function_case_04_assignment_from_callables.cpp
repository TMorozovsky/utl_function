#include "override_global_new_and_delete.hpp"

#include "utl/function.hpp"

#include "tested_functions_and_function_objects.hpp"
#include "basic_utl_function_test_fixture.hpp"

#include <cassert>
#include <type_traits>
#include <utility>

class test_utl_function_case_04_assignment_from_callables : public basic_utl_function_test
{
public:
  template<typename FunctionOrFunctionObjectToAssign>
  void do_test(FunctionOrFunctionObjectToAssign& callable_to_assign, const expected_storage_type expected_storage_of_assigned_object) const
  {
    static_assert(!std::is_reference_v<FunctionOrFunctionObjectToAssign>);
    static_assert(!std::is_const_v<FunctionOrFunctionObjectToAssign>);
    static_assert(!std::is_volatile_v<FunctionOrFunctionObjectToAssign>);

    const original_in_place_function_object original_in_place_object{};
    const original_dynamically_allocated_function_object original_dynamic_object{};

    do_test_with_original(original_in_place_object, callable_to_assign, expected_storage_of_assigned_object);
    do_test_with_original(original_dynamic_object,  callable_to_assign, expected_storage_of_assigned_object);
  }

  template<typename OriginalFunctionObject, typename FunctionOrFunctionObjectToAssign>
  void do_test_with_original(const OriginalFunctionObject& original_callable,
                             FunctionOrFunctionObjectToAssign& callable_to_assign, const expected_storage_type expected_storage_of_assigned_object) const
  {
    if constexpr (is_std_reference_wrapper<FunctionOrFunctionObjectToAssign>())
    {
      using wrapped_type = typename FunctionOrFunctionObjectToAssign::type;
      static_assert(!std::is_const_v<wrapped_type> && !std::is_volatile_v<wrapped_type>);

      assert(expected_storage_of_assigned_object == expected_storage_type::in_place);
      do_test_impl(original_callable, callable_to_assign, expected_storage_of_assigned_object);
      do_test_impl(original_callable, std::as_const(callable_to_assign), expected_storage_of_assigned_object);
      auto callable_to_assign_copy = callable_to_assign;
      do_test_impl(original_callable, std::move(callable_to_assign_copy), expected_storage_of_assigned_object);
      if constexpr (!std::is_function_v<wrapped_type>)
      {
        std::reference_wrapper<const wrapped_type> copied_wrapper_of_const_callable = callable_to_assign;
        do_test_impl(original_callable, copied_wrapper_of_const_callable, expected_storage_of_assigned_object);
        do_test_impl(original_callable, std::as_const(copied_wrapper_of_const_callable), expected_storage_of_assigned_object);
        do_test_impl(original_callable, std::move(copied_wrapper_of_const_callable), expected_storage_of_assigned_object);
      }
    }
    else if constexpr (std::is_class_v<FunctionOrFunctionObjectToAssign>)
    {
      do_test_impl(original_callable, callable_to_assign, expected_storage_of_assigned_object);
      do_test_impl(original_callable, std::as_const(callable_to_assign), expected_storage_of_assigned_object);
      auto callable_to_assign_copy = callable_to_assign;
      do_test_impl(original_callable, std::move(callable_to_assign_copy), expected_storage_of_assigned_object);
    }
    else
    {
      static_assert(std::is_function_v<FunctionOrFunctionObjectToAssign>);
      assert(expected_storage_of_assigned_object == expected_storage_type::in_place);
      auto function_pointer = &callable_to_assign;
      do_test_impl(original_callable, callable_to_assign, expected_storage_of_assigned_object);
      do_test_impl(original_callable, function_pointer, expected_storage_of_assigned_object);
      do_test_impl(original_callable, std::as_const(function_pointer), expected_storage_of_assigned_object);
      do_test_impl(original_callable, std::move(function_pointer), expected_storage_of_assigned_object);
    }
  }

private:
  template<typename OriginalFunctionObject, typename FunctionOrFunctionObjectToAssign>
  void do_test_impl(const OriginalFunctionObject& callable_to_initialize_with,
                    FunctionOrFunctionObjectToAssign&& callable_to_assign, const expected_storage_type expected_storage_of_assigned_object) const
  {
    const expected_storage_type expected_storage_of_original_object = get_expected_storage_of_original_function_object<OriginalFunctionObject>();
    unsigned int &      invoked_counter_of_original_object = OriginalFunctionObject::invoked_counter;

    using decayed_assigned_type = std::decay_t<FunctionOrFunctionObjectToAssign>;
    unsigned int &      invoked_counter_of_assigned_object          = access_invoked_counter<decayed_assigned_type>();
    unsigned int* const copy_constructed_counter_of_assigned_object = access_copy_constructed_counter_if_applicable<decayed_assigned_type>();
    unsigned int* const move_constructed_counter_of_assigned_object = access_move_constructed_counter_if_applicable<decayed_assigned_type>();

    const unsigned int initial_copy_constructed_counter_of_assigned_object =
      (copy_constructed_counter_of_assigned_object != nullptr) ? *copy_constructed_counter_of_assigned_object : 0u;

    const unsigned int initial_move_constructed_counter_of_assigned_object =
      (move_constructed_counter_of_assigned_object != nullptr) ? *move_constructed_counter_of_assigned_object : 0u;

    utl::function<void()> function = callable_to_initialize_with;

    invoked_counter_of_original_object = 0;
    invoked_counter_of_assigned_object = 0;

    ASSERT_TRUE(static_cast<bool>(function));
    function();
    EXPECT_EQ(invoked_counter_of_original_object, 1);
    EXPECT_EQ(invoked_counter_of_assigned_object, 0);

    invoked_counter_of_original_object = 0;
    invoked_counter_of_assigned_object = 0;

    counter_of_allocations_done_by_operator_new = 0;
    counter_of_deallocations_done_by_operator_delete = 0;

    function = std::forward<FunctionOrFunctionObjectToAssign>(callable_to_assign);

    if (expected_storage_of_original_object == expected_storage_type::dynamic)
      EXPECT_EQ(counter_of_deallocations_done_by_operator_delete, 1);
    else
      EXPECT_EQ(counter_of_deallocations_done_by_operator_delete, 0);

    if (expected_storage_of_assigned_object == expected_storage_type::dynamic)
      EXPECT_EQ(counter_of_allocations_done_by_operator_new, 1);
    else
      EXPECT_EQ(counter_of_allocations_done_by_operator_new, 0);

    if (std::is_lvalue_reference_v<FunctionOrFunctionObjectToAssign>)
    {
      if (copy_constructed_counter_of_assigned_object)
        EXPECT_EQ(*copy_constructed_counter_of_assigned_object, (initial_copy_constructed_counter_of_assigned_object + 1));
    }
    else
    {
      if (move_constructed_counter_of_assigned_object)
        EXPECT_GE(*move_constructed_counter_of_assigned_object, (initial_move_constructed_counter_of_assigned_object + 1));
      if (copy_constructed_counter_of_assigned_object)
        EXPECT_EQ(*copy_constructed_counter_of_assigned_object, initial_copy_constructed_counter_of_assigned_object);
    }

    ASSERT_TRUE(static_cast<bool>(function));
    function();
    EXPECT_EQ(invoked_counter_of_original_object, 0);
    EXPECT_EQ(invoked_counter_of_assigned_object, 1);

    ASSERT_TRUE(static_cast<bool>(function));
    function();
    EXPECT_EQ(invoked_counter_of_original_object, 0);
    EXPECT_EQ(invoked_counter_of_assigned_object, 2);

    EXPECT_TRUE(static_cast<bool>(function));

    // also test that a `function` can be assigned from `nullptr`, which should effectively clear it
    auto deallocations_counter_before_assigning_nullptr = counter_of_deallocations_done_by_operator_delete;
    function = nullptr;
    EXPECT_FALSE(static_cast<bool>(function));
    if (expected_storage_of_assigned_object == expected_storage_type::dynamic)
      EXPECT_EQ(counter_of_deallocations_done_by_operator_delete, (deallocations_counter_before_assigning_nullptr + 1));
    else
      EXPECT_EQ(counter_of_deallocations_done_by_operator_delete, deallocations_counter_before_assigning_nullptr);
  }
};

TEST_F(test_utl_function_case_04_assignment_from_callables, function_can_be_assigned_from_various_callables)
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
