#include "override_global_new_and_delete.hpp"

#include "utl/function.hpp"

#include "tested_functions_and_function_objects.hpp"
#include "basic_utl_function_test_fixture.hpp"

#include <cassert>
#include <type_traits>
#include <functional>

class test_utl_function_case_02_construction_from_callables_and_invocation : public basic_utl_function_test
{
public:
  template<typename CallableStruct>
  void do_test_with_callable_struct(const expected_storage_type expected_storage) const
  {
    do_test_with_callable_struct_impl<      utl::function<void()>, CallableStruct>(expected_storage);
    do_test_with_callable_struct_impl<const utl::function<void()>, CallableStruct>(expected_storage);
  }

  template<typename PlainFunction>
  void do_test_with_plain_function(PlainFunction& plain_function) const
  {
    do_test_with_plain_function_impl<      utl::function<void()>>(plain_function);
    do_test_with_plain_function_impl<const utl::function<void()>>(plain_function);
  }

  template<typename CallableStruct>
  void do_test_with_std_reference_wrapper_of_callable_struct() const
  {
    do_test_with_std_reference_wrapper_of_callable_struct_impl<      utl::function<void()>, CallableStruct>();
    do_test_with_std_reference_wrapper_of_callable_struct_impl<const utl::function<void()>, CallableStruct>();
  }

  template<typename PlainFunction>
  void do_test_with_std_reference_wrapper_of_plain_function(PlainFunction& plain_function) const
  {
    do_test_with_std_reference_wrapper_of_plain_function_impl<      utl::function<void()>>(plain_function);
    do_test_with_std_reference_wrapper_of_plain_function_impl<const utl::function<void()>>(plain_function);
  }

private:
  template<typename UtlFunctionToCreate, typename CallableStruct>
  void do_test_with_callable_struct_impl(const expected_storage_type expected_storage) const
  {
    static_assert(std::is_same_v<UtlFunctionToCreate,       utl::function<void()>> ||
                  std::is_same_v<UtlFunctionToCreate, const utl::function<void()>>);

    static_assert(std::is_class_v<CallableStruct> && std::is_same_v<CallableStruct, std::decay_t<CallableStruct>>);

    using function_t = UtlFunctionToCreate;

    {
      // construct a function using the `{}` syntax with an rvalue argument
      reset_related_counters<CallableStruct>();
      function_t function{CallableStruct{}};
      do_test_constructed_object<CallableStruct>(function, expected_storage);
    }
    {
      // construct a function using the `=` syntax with an rvalue argument
      reset_related_counters<CallableStruct>();
      function_t function = CallableStruct{};
      do_test_constructed_object<CallableStruct>(function, expected_storage);
    }
    {
      // construct a function using the `{}` syntax with a non-const lvalue argument
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      function_t function{callable};
      do_test_constructed_object<CallableStruct>(function, expected_storage);
    }
    {
      // construct a function using the `=` syntax with a non-const lvalue argument
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      function_t function = callable;
      do_test_constructed_object<CallableStruct>(function, expected_storage);
    }
    {
      // construct a function using the `{}` syntax with a const lvalue argument
      reset_related_counters<CallableStruct>();
      const CallableStruct callable{};
      function_t function{callable};
      do_test_constructed_object<CallableStruct>(function, expected_storage);
    }
    {
      // construct a function using the `=` syntax with a const lvalue argument
      reset_related_counters<CallableStruct>();
      const CallableStruct callable{};
      function_t function = callable;
      do_test_constructed_object<CallableStruct>(function, expected_storage);
    }
  }

  template<typename UtlFunctionToCreate, typename PlainFunction>
  void do_test_with_plain_function_impl(PlainFunction& plain_function) const
  {
    static_assert(std::is_same_v<UtlFunctionToCreate,       utl::function<void()>> ||
                  std::is_same_v<UtlFunctionToCreate, const utl::function<void()>>);

    static_assert(std::is_same_v<PlainFunction, void()> ||
                  std::is_same_v<PlainFunction, void() noexcept>);

    using function_t = UtlFunctionToCreate;
    unsigned int& plain_function_invoked_counter = access_invoked_counter<PlainFunction>();

    // constructing a function from a plain function must always avoid dynamic memory allocations
    {
      // construct a function using the `{}` syntax with a reference to the plain function
      plain_function_invoked_counter = 0;
      function_t function{plain_function};
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a reference to the plain function
      plain_function_invoked_counter = 0;
      function_t function = plain_function;
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with a pointer to the plain function
      plain_function_invoked_counter = 0;
      function_t function{&plain_function};
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a pointer to the plain function
      plain_function_invoked_counter = 0;
      function_t function = &plain_function;
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
  }

  template<typename UtlFunctionToCreate, typename CallableStruct>
  void do_test_with_std_reference_wrapper_of_callable_struct_impl() const
  {
    static_assert(std::is_same_v<UtlFunctionToCreate,       utl::function<void()>> ||
                  std::is_same_v<UtlFunctionToCreate, const utl::function<void()>>);

    static_assert(std::is_class_v<CallableStruct> && std::is_same_v<CallableStruct, std::decay_t<CallableStruct>>);

    using function_t = UtlFunctionToCreate;

    // constructing a function from a reference wrapper must always avoid dynamic memory allocations
    {
      // construct a function using the `{}` syntax with an rvalue reference wrapper of a non-const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      function_t function{std::ref(callable)};
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with an rvalue reference wrapper of a non-const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      function_t function = std::ref(callable);
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with an rvalue reference wrapper of a const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      function_t function{std::cref(callable)};
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with an rvalue reference wrapper of a const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      function_t function = std::cref(callable);
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with a non-const lvalue reference wrapper of a non-const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      auto callable_ref = std::ref(callable);
      function_t function{callable_ref};
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a non-const lvalue reference wrapper of a non-const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      auto callable_ref = std::ref(callable);
      function_t function = callable_ref;
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with a non-const lvalue reference wrapper of a const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      auto callable_ref = std::cref(callable);
      function_t function{callable_ref};
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a non-const lvalue reference wrapper of a const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      auto callable_ref = std::cref(callable);
      function_t function = callable_ref;
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with a const lvalue reference wrapper of a non-const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      const auto callable_ref = std::ref(callable);
      function_t function{callable_ref};
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a const lvalue reference wrapper of a non-const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      const auto callable_ref = std::ref(callable);
      function_t function = callable_ref;
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with a const lvalue reference wrapper of a const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      const auto callable_ref = std::cref(callable);
      function_t function{callable_ref};
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a const lvalue reference wrapper of a const callable
      reset_related_counters<CallableStruct>();
      CallableStruct callable{};
      const auto callable_ref = std::cref(callable);
      function_t function = callable_ref;
      do_test_constructed_object<CallableStruct>(function, expected_storage_type::in_place);
    }
  }

  template<typename UtlFunctionToCreate, typename PlainFunction>
  void do_test_with_std_reference_wrapper_of_plain_function_impl(PlainFunction& plain_function) const
  {
    static_assert(std::is_same_v<UtlFunctionToCreate,       utl::function<void()>> ||
                  std::is_same_v<UtlFunctionToCreate, const utl::function<void()>>);

    static_assert(std::is_same_v<PlainFunction, void()> ||
                  std::is_same_v<PlainFunction, void() noexcept>);

    using function_t = UtlFunctionToCreate;
    unsigned int& plain_function_invoked_counter = access_invoked_counter<PlainFunction>();

    // constructing a function from a reference wrapper must always avoid dynamic memory allocations
    {
      // construct a function using the `{}` syntax with an rvalue reference wrapper of the plain function
      plain_function_invoked_counter = 0;
      function_t function{std::ref(plain_function)};
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with an rvalue reference wrapper of the plain function
      plain_function_invoked_counter = 0;
      function_t function = std::ref(plain_function);
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with a non-const lvalue reference wrapper of the plain function
      plain_function_invoked_counter = 0;
      auto plain_function_reference_wrapper = std::ref(plain_function);
      function_t function{plain_function_reference_wrapper};
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a non-const lvalue reference wrapper of the plain function
      plain_function_invoked_counter = 0;
      auto plain_function_reference_wrapper = std::ref(plain_function);
      function_t function = plain_function_reference_wrapper;
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `{}` syntax with a const lvalue reference wrapper of the plain function
      plain_function_invoked_counter = 0;
      const auto plain_function_reference_wrapper = std::ref(plain_function);
      function_t function{plain_function_reference_wrapper};
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // construct a function using the `=` syntax with a const lvalue reference wrapper of the plain function
      plain_function_invoked_counter = 0;
      const auto plain_function_reference_wrapper = std::ref(plain_function);
      function_t function = plain_function_reference_wrapper;
      do_test_constructed_object<PlainFunction>(function, expected_storage_type::in_place);
    }
    {
      // hack required because multiple compilers fire unnecessary [[nodiscard]]-related warnings in the `decltype` context
      constexpr auto silence_nodiscard = [](auto&& ref) constexpr -> decltype(ref) { return std::forward<decltype(ref)>(ref); };
      // we do not check `std::cref()` separately because all calls to `std::cref()` are equivalent to
      // the respective calls to `std::ref()` when dealing with plain functions
      static_assert(std::is_same_v<decltype(silence_nodiscard(std::ref(plain_function))), decltype(silence_nodiscard(std::cref(plain_function)))>);
    }
  }

  template<typename CallableUsedForConstruction>
  void reset_related_counters() const
  {
    counter_of_allocations_done_by_operator_new = 0;
    counter_of_deallocations_done_by_operator_delete = 0;
    access_invoked_counter<CallableUsedForConstruction>() = 0;
  }

  template<typename CallableUsedForConstruction, typename ConstructedUtlFunction>
  void do_test_constructed_object(ConstructedUtlFunction& constructed_utl_function, const expected_storage_type expected_storage) const
  {
    ASSERT_TRUE(constructed_utl_function);
    ASSERT_FALSE(!constructed_utl_function);

    assert((expected_storage == expected_storage_type::in_place) || (expected_storage == expected_storage_type::dynamic));
    if (expected_storage == expected_storage_type::in_place)
      EXPECT_EQ(counter_of_allocations_done_by_operator_new, 0);
    // else: do not check anything at all because the allocation might still have been optimized by the compiler...
#ifndef NDEBUG
    // ...unless we're in Debug :)
    else
      EXPECT_EQ(counter_of_allocations_done_by_operator_new, 1);
#endif

    const auto original_counter_of_allocations = counter_of_allocations_done_by_operator_new;

    EXPECT_EQ(access_invoked_counter<CallableUsedForConstruction>(), 0);
    constructed_utl_function();
    EXPECT_EQ(access_invoked_counter<CallableUsedForConstruction>(), 1);

    // must still be valid after being invoked, thus it can be invoked again
    ASSERT_TRUE(constructed_utl_function);
    ASSERT_FALSE(!constructed_utl_function);

    constructed_utl_function();
    EXPECT_EQ(access_invoked_counter<CallableUsedForConstruction>(), 2);

    EXPECT_EQ(counter_of_allocations_done_by_operator_new, original_counter_of_allocations);
  }
};

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation, construction_from_a_small_function_object)
{
  do_test_with_callable_struct<small_function_object>(expected_storage_type::in_place);
}

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation,
       construction_from_a_function_object_with_throwing_move_constructor)
{
  do_test_with_callable_struct<throwing_function_object>(expected_storage_type::dynamic);
}

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation, construction_from_a_large_function_object)
{
  do_test_with_callable_struct<large_function_object>(expected_storage_type::dynamic);
}

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation, construction_from_an_overaligned_function_object)
{
  do_test_with_callable_struct<overaligned_function_object>(expected_storage_type::dynamic);
}

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation, construction_from_a_plain_function)
{
  do_test_with_plain_function(global_function);
  do_test_with_plain_function(global_function_noexcept);
}

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation, construction_from_a_std_reference_wrapper_of_any_function_object)
{
  do_test_with_std_reference_wrapper_of_callable_struct<small_function_object>();
  do_test_with_std_reference_wrapper_of_callable_struct<throwing_function_object>();
  do_test_with_std_reference_wrapper_of_callable_struct<large_function_object>();
  do_test_with_std_reference_wrapper_of_callable_struct<overaligned_function_object>();
}

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation, construction_from_a_std_reference_wrapper_of_plain_function)
{
  do_test_with_std_reference_wrapper_of_plain_function(global_function);
  do_test_with_std_reference_wrapper_of_plain_function(global_function_noexcept);
}

TEST_F(test_utl_function_case_02_construction_from_callables_and_invocation, test_deduction_guides)
{
  {
    [[maybe_unused]] utl::function function = small_function_object{};
    static_assert(std::is_same_v<decltype(function), utl::function<void()>>);
  }
  {
    [[maybe_unused]] utl::function function = large_function_object{};
    static_assert(std::is_same_v<decltype(function), utl::function<void()>>);
  }
  {
    [[maybe_unused]] utl::function function = global_function;
    static_assert(std::is_same_v<decltype(function), utl::function<void()>>);
  }
  {
    [[maybe_unused]] utl::function function = global_function_noexcept;
    static_assert(std::is_same_v<decltype(function), utl::function<void()>>);
  }
  {
    [[maybe_unused]] utl::function function = [](int x, float y) -> double
    {
      return static_cast<double>(x) + static_cast<double>(y);
    };
    static_assert(std::is_same_v<decltype(function), utl::function<double(int, float)>>);
  }
}
