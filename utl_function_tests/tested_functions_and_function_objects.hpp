#pragma once

#include "override_global_new_and_delete.hpp"

#include "utl/function.hpp"

#include <type_traits>
#include <stdexcept>
#include <functional>

inline thread_local unsigned int global_function_invoked_counter = 0;
inline thread_local unsigned int global_function_noexcept_invoked_counter = 0;

inline void global_function()
{
  ++global_function_invoked_counter;
}

inline void global_function_noexcept() noexcept
{
  ++global_function_noexcept_invoked_counter;
}

// a function object that can be stored in the tested `function` template instance;
// its copy owned by such a `function` is supposed to be allocated in-place
// (inside the small buffer stored within the `function` object)
struct small_function_object
{
  small_function_object()
  {
    ++default_constructed_counter;
  }

  small_function_object(const small_function_object&)
  {
    ++copy_constructed_counter;
  }

  small_function_object(small_function_object&&) noexcept
  {
    ++move_constructed_counter;
  }

  small_function_object& operator=(const small_function_object&)
  {
    ++copy_assigned_counter;
    return *this;
  }

  small_function_object& operator=(small_function_object&&) noexcept
  {
    ++move_assigned_counter;
    return *this;
  }

  ~small_function_object()
  {
    ++destroyed_counter;
  }

  void operator()() const
  {
    ++invoked_counter;
  }

  inline static thread_local unsigned int default_constructed_counter = 0;
  inline static thread_local unsigned int copy_constructed_counter = 0;
  inline static thread_local unsigned int move_constructed_counter = 0;
  inline static thread_local unsigned int copy_assigned_counter = 0;
  inline static thread_local unsigned int move_assigned_counter = 0;
  inline static thread_local unsigned int destroyed_counter = 0;
  inline static thread_local unsigned int invoked_counter = 0;

  static void reset_counters() noexcept
  {
    default_constructed_counter = 0;
    copy_constructed_counter = 0;
    move_constructed_counter = 0;
    copy_assigned_counter = 0;
    move_assigned_counter = 0;
    destroyed_counter = 0;
    invoked_counter = 0;
  }

private:
  void* dummy = nullptr;
};

// a function object that can be stored in the tested `function` template instance;
// its copy owned by such a `function` is supposed to be allocated dynamically because
// this object's move constructor is not `noexcept`,
// and thus it could not have guaranteed the `nothrow_move_constructible`
// property of the `function` itself if it was allocated in its small buffer
struct throwing_function_object
{
  throwing_function_object()
  {
    ++default_constructed_counter;
  }

  throwing_function_object(const throwing_function_object&)
  {
    ++copy_constructed_counter;
  }

  throwing_function_object(throwing_function_object&&) noexcept(false)
  {
    ++move_constructed_counter;
  }

  throwing_function_object& operator=(const throwing_function_object&)
  {
    ++copy_assigned_counter;
    return *this;
  }

  throwing_function_object& operator=(throwing_function_object&&) noexcept(false)
  {
    ++move_assigned_counter;
    return *this;
  }

  ~throwing_function_object()
  {
    ++destroyed_counter;
  }

  void operator()() const
  {
    ++invoked_counter;
  }

  inline static thread_local unsigned int default_constructed_counter = 0;
  inline static thread_local unsigned int copy_constructed_counter = 0;
  inline static thread_local unsigned int move_constructed_counter = 0;
  inline static thread_local unsigned int copy_assigned_counter = 0;
  inline static thread_local unsigned int move_assigned_counter = 0;
  inline static thread_local unsigned int destroyed_counter = 0;
  inline static thread_local unsigned int invoked_counter = 0;

  static void reset_counters() noexcept
  {
    default_constructed_counter = 0;
    copy_constructed_counter = 0;
    move_constructed_counter = 0;
    copy_assigned_counter = 0;
    move_assigned_counter = 0;
    destroyed_counter = 0;
    invoked_counter = 0;
  }

private:
  void* dummy = nullptr;
};

// a function object that can be stored in the tested `function` template instance;
// its copy owned by such a `function` is supposed to be allocated dynamically because
// this object's size is greater than the size of the `function`'s small buffer
struct large_function_object
{
  large_function_object()
  {
    dummy[0] = 0;
    ++default_constructed_counter;
  }

  large_function_object(const large_function_object&)
  {
    dummy[0] = 0;
    ++copy_constructed_counter;
  }

  large_function_object(large_function_object&&) noexcept
  {
    dummy[0] = 0;
    ++move_constructed_counter;
  }

  large_function_object& operator=(const large_function_object&)
  {
    ++copy_assigned_counter;
    return *this;
  }

  large_function_object& operator=(large_function_object&&) noexcept
  {
    ++move_assigned_counter;
    return *this;
  }

  ~large_function_object()
  {
    ++destroyed_counter;
  }

  void operator()() const
  {
    ++invoked_counter;
  }

  inline static thread_local unsigned int default_constructed_counter = 0;
  inline static thread_local unsigned int copy_constructed_counter = 0;
  inline static thread_local unsigned int move_constructed_counter = 0;
  inline static thread_local unsigned int copy_assigned_counter = 0;
  inline static thread_local unsigned int move_assigned_counter = 0;
  inline static thread_local unsigned int destroyed_counter = 0;
  inline static thread_local unsigned int invoked_counter = 0;

  static void reset_counters() noexcept
  {
    default_constructed_counter = 0;
    copy_constructed_counter = 0;
    move_constructed_counter = 0;
    copy_assigned_counter = 0;
    move_assigned_counter = 0;
    destroyed_counter = 0;
    invoked_counter = 0;
  }

private:
  unsigned char dummy[2 * sizeof(utl::function<void()>)];
};

// a function object that can be stored in the tested `function` template instance;
// its copy owned by such a `function` is supposed to be allocated dynamically because
// this object's alignment requirement is greater than the maximum alignment supported by `function`
struct overaligned_function_object
{
  overaligned_function_object()
  {
    ++default_constructed_counter;
  }

  overaligned_function_object(const overaligned_function_object&)
  {
    ++copy_constructed_counter;
  }

  overaligned_function_object(overaligned_function_object&&) noexcept
  {
    ++move_constructed_counter;
  }

  overaligned_function_object& operator=(const overaligned_function_object&)
  {
    ++copy_assigned_counter;
    return *this;
  }

  overaligned_function_object& operator=(overaligned_function_object&&) noexcept
  {
    ++move_assigned_counter;
    return *this;
  }

  ~overaligned_function_object()
  {
    ++destroyed_counter;
  }

  void operator()() const
  {
    ++invoked_counter;
  }

  inline static thread_local unsigned int default_constructed_counter = 0;
  inline static thread_local unsigned int copy_constructed_counter = 0;
  inline static thread_local unsigned int move_constructed_counter = 0;
  inline static thread_local unsigned int copy_assigned_counter = 0;
  inline static thread_local unsigned int move_assigned_counter = 0;
  inline static thread_local unsigned int destroyed_counter = 0;
  inline static thread_local unsigned int invoked_counter = 0;

  static void reset_counters() noexcept
  {
    default_constructed_counter = 0;
    copy_constructed_counter = 0;
    move_constructed_counter = 0;
    copy_assigned_counter = 0;
    move_assigned_counter = 0;
    destroyed_counter = 0;
    invoked_counter = 0;
  }

private:
  alignas(alignof(utl::function<void(void)>) * 2) unsigned char dummy = 0;
};

// an "original function object" for the assignment tests;
// supposed to be stored by the tested `function` template instance in-place
struct original_in_place_function_object
{
  original_in_place_function_object()
  {
    ++default_constructed_counter;
  }

  original_in_place_function_object(const original_in_place_function_object&)
  {
    ++copy_constructed_counter;
  }

  original_in_place_function_object(original_in_place_function_object&&) noexcept
  {
    ++move_constructed_counter;
  }

  original_in_place_function_object& operator=(const original_in_place_function_object&)
  {
    ++copy_assigned_counter;
    return *this;
  }

  original_in_place_function_object& operator=(original_in_place_function_object&&) noexcept
  {
    ++move_assigned_counter;
    return *this;
  }

  ~original_in_place_function_object()
  {
    ++destroyed_counter;
  }

  void operator()() const noexcept
  {
    ++invoked_counter;
  }

  inline static thread_local unsigned int default_constructed_counter = 0;
  inline static thread_local unsigned int copy_constructed_counter = 0;
  inline static thread_local unsigned int move_constructed_counter = 0;
  inline static thread_local unsigned int copy_assigned_counter = 0;
  inline static thread_local unsigned int move_assigned_counter = 0;
  inline static thread_local unsigned int destroyed_counter = 0;
  inline static thread_local unsigned int invoked_counter = 0;

  static void reset_counters() noexcept
  {
    default_constructed_counter = 0;
    copy_constructed_counter = 0;
    move_constructed_counter = 0;
    copy_assigned_counter = 0;
    move_assigned_counter = 0;
    destroyed_counter = 0;
    invoked_counter = 0;
  }

  void* dummy = nullptr;
};

// an "original function object" for the assignment tests;
// supposed to be owned by the tested `function` template instance using a pointer to the dynamically allocated memory
struct original_dynamically_allocated_function_object
{
  original_dynamically_allocated_function_object()
  {
    ++default_constructed_counter;
  }

  original_dynamically_allocated_function_object(const original_dynamically_allocated_function_object&)
  {
    ++copy_constructed_counter;
  }

  original_dynamically_allocated_function_object(original_dynamically_allocated_function_object&&) noexcept(false)
  {
    ++move_constructed_counter;
  }

  original_dynamically_allocated_function_object& operator=(const original_dynamically_allocated_function_object&)
  {
    ++copy_assigned_counter;
    return *this;
  }

  original_dynamically_allocated_function_object& operator=(original_dynamically_allocated_function_object&&) noexcept(false)
  {
    ++move_assigned_counter;
    return *this;
  }

  ~original_dynamically_allocated_function_object()
  {
    ++destroyed_counter;
  }

  void operator()() const
  {
    ++invoked_counter;
  }

  inline static thread_local unsigned int default_constructed_counter = 0;
  inline static thread_local unsigned int copy_constructed_counter = 0;
  inline static thread_local unsigned int move_constructed_counter = 0;
  inline static thread_local unsigned int copy_assigned_counter = 0;
  inline static thread_local unsigned int move_assigned_counter = 0;
  inline static thread_local unsigned int destroyed_counter = 0;
  inline static thread_local unsigned int invoked_counter = 0;

  static void reset_counters() noexcept
  {
    default_constructed_counter = 0;
    copy_constructed_counter = 0;
    move_constructed_counter = 0;
    copy_assigned_counter = 0;
    move_assigned_counter = 0;
    destroyed_counter = 0;
    invoked_counter = 0;
  }

  alignas(2 * alignof(utl::function<void()>)) void* dummy = nullptr;
};

enum class expected_storage_type
{
  in_place,
  dynamic
};

template<typename OriginalFunctionObject>
constexpr expected_storage_type get_expected_storage_of_original_function_object() noexcept
{
  using decayed_type = std::decay_t<OriginalFunctionObject>;

  static_assert(std::is_same_v<decayed_type, original_in_place_function_object> ||
                std::is_same_v<decayed_type, original_dynamically_allocated_function_object>);

  if constexpr (std::is_same_v<decayed_type, original_in_place_function_object>)
    return expected_storage_type::in_place;
  else
    return expected_storage_type::dynamic;
}

inline void reset_all_counters() noexcept
{
  counter_of_allocations_done_by_operator_new = 0;
  counter_of_deallocations_done_by_operator_delete = 0;

  global_function_invoked_counter = 0;
  global_function_noexcept_invoked_counter = 0;

  small_function_object::reset_counters();
  throwing_function_object::reset_counters();
  large_function_object::reset_counters();
  overaligned_function_object::reset_counters();
  original_in_place_function_object::reset_counters();
  original_dynamically_allocated_function_object::reset_counters();
}

namespace detail
{
  template<typename T>
  [[nodiscard]] constexpr bool is_decayed_type_std_reference_wrapper(std::type_identity<T>) noexcept { return false; }

  template<typename T>
  [[nodiscard]] constexpr bool is_decayed_type_std_reference_wrapper(std::type_identity<std::reference_wrapper<T>>) noexcept { return true; }
}

template<typename T>
[[nodiscard]] constexpr bool is_std_reference_wrapper() noexcept
{
  static_assert(!std::is_reference_v<T>);
  return detail::is_decayed_type_std_reference_wrapper(std::type_identity<std::remove_cv_t<T>>{});
}

struct counters_collection
{
  unsigned int &      invoked_counter;
  unsigned int *const default_constructed_counter = nullptr;
  unsigned int *const copy_constructed_counter = nullptr;
  unsigned int *const move_constructed_counter = nullptr;
  unsigned int *const copy_assigned_counter = nullptr;
  unsigned int *const move_assigned_counter = nullptr;
  unsigned int *const destroyed_counter = nullptr;
};

template<typename T> // T - one of the tested functions, function objects, or a reference wrapper to one of those
[[nodiscard]] constexpr counters_collection access_related_counters() noexcept
{
  static_assert(!std::is_reference_v<T>);
  using U = std::remove_cv_t<T>;

  if constexpr (std::is_same_v<U, void()> || std::is_same_v<U, void(*)()>)
  {
    return counters_collection{ global_function_invoked_counter };
  }
  else if constexpr (std::is_same_v<U, void() noexcept> || std::is_same_v<U, void(*)() noexcept>)
  {
    return counters_collection{ global_function_noexcept_invoked_counter };
  }
  else if constexpr (is_std_reference_wrapper<U>())
  {
    // invocation of a reference wrapper invokes the referenced object, but copying, moving or destroying it does not affect the referenced object
    return counters_collection{ access_related_counters<typename U::type>().invoked_counter, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  }
  else
  {
    static_assert(std::is_class_v<U>);
    return counters_collection{ U::invoked_counter,
                                &U::default_constructed_counter,
                                &U::copy_constructed_counter,
                                &U::move_constructed_counter,
                                &U::copy_assigned_counter,
                                &U::move_assigned_counter,
                                &U::destroyed_counter };
  }
}

template<typename FunctionOrFunctionObject>
[[nodiscard]] constexpr unsigned int& access_invoked_counter() noexcept
{
  return access_related_counters<FunctionOrFunctionObject>().invoked_counter;
}

template<typename FunctionOrFunctionObject>
[[nodiscard]] constexpr unsigned int* access_copy_constructed_counter_if_applicable() noexcept
{
  return access_related_counters<FunctionOrFunctionObject>().copy_constructed_counter;
}

template<typename FunctionOrFunctionObject>
[[nodiscard]] constexpr unsigned int* access_move_constructed_counter_if_applicable() noexcept
{
  return access_related_counters<FunctionOrFunctionObject>().move_constructed_counter;
}
