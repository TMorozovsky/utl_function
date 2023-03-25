#pragma once

#include "override_global_new_and_delete.hpp"
#include "tested_functions_and_function_objects.hpp"

#include <gtest/gtest.h>

class basic_utl_function_test : public ::testing::Test
{
public:
  basic_utl_function_test()
  {
    reset_all_counters();
  }

  ~basic_utl_function_test()
  {
    check_counters();
    reset_all_counters();
  }

  static void check_counters()
  {
    EXPECT_EQ(small_function_object::default_constructed_counter + small_function_object::copy_constructed_counter + small_function_object::move_constructed_counter,
              small_function_object::destroyed_counter); // the number of calls to default+copy+move constructors should match the number of calls of a respective destructor
    EXPECT_EQ(throwing_function_object::default_constructed_counter + throwing_function_object::copy_constructed_counter + throwing_function_object::move_constructed_counter,
              throwing_function_object::destroyed_counter);
    EXPECT_EQ(large_function_object::default_constructed_counter + large_function_object::copy_constructed_counter + large_function_object::move_constructed_counter,
              large_function_object::destroyed_counter);
    EXPECT_EQ(overaligned_function_object::default_constructed_counter + overaligned_function_object::copy_constructed_counter + overaligned_function_object::move_constructed_counter,
              overaligned_function_object::destroyed_counter);
    EXPECT_EQ(original_in_place_function_object::default_constructed_counter + original_in_place_function_object::copy_constructed_counter + original_in_place_function_object::move_constructed_counter,
              original_in_place_function_object::destroyed_counter);
    EXPECT_EQ(original_dynamically_allocated_function_object::default_constructed_counter + original_dynamically_allocated_function_object::copy_constructed_counter + original_dynamically_allocated_function_object::move_constructed_counter,
              original_dynamically_allocated_function_object::destroyed_counter);

    EXPECT_EQ(small_function_object::copy_assigned_counter + small_function_object::move_assigned_counter, 0); // assignment operators should not be invoked at all
    EXPECT_EQ(throwing_function_object::copy_assigned_counter + throwing_function_object::move_assigned_counter, 0);
    EXPECT_EQ(large_function_object::copy_assigned_counter + large_function_object::move_assigned_counter, 0);
    EXPECT_EQ(overaligned_function_object::copy_assigned_counter + overaligned_function_object::move_assigned_counter, 0);
    EXPECT_EQ(original_in_place_function_object::copy_assigned_counter + original_in_place_function_object::move_assigned_counter, 0);
    EXPECT_EQ(original_dynamically_allocated_function_object::copy_assigned_counter + original_dynamically_allocated_function_object::move_assigned_counter, 0);
  }
};
