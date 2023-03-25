#include "override_global_new_and_delete.hpp"
#include "tested_functions_and_function_objects.hpp"

#include "utl/function.hpp"

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <type_traits>

namespace
{
  struct global_static_assertions_tester
  {
    constexpr global_static_assertions_tester()
    {
      run_static_assertions_testing();
    }

    constexpr void run_static_assertions_testing()
    {
      { // all instances of the `function` template must have the same size and alignment
        constexpr auto function_size      = sizeof (utl::function<void()>);
        constexpr auto function_alignment = alignof(utl::function<void()>);

        using other_function_t = utl::function<std::vector<int>(const std::map<int, int>&, const char*, std::unique_ptr<std::string>&&)>;

        static_assert(sizeof (other_function_t) == function_size);
        static_assert(alignof(other_function_t) == function_alignment);
      }

      { // basic sanity checks related to the size and alignment of a `function`
        static_assert(sizeof (utl::function<void()>[5]) == (5 * sizeof(utl::function<void()>)));
        static_assert(alignof(utl::function<void()>[5]) == alignof(utl::function<void()>));

        static_assert(sizeof(utl::function<void()>) >= sizeof(void*));
        static_assert(sizeof(utl::function<void()>) % sizeof(void*) == 0);
        static_assert(alignof(utl::function<void()>) >= alignof(void*));
      }

      { // a `function` must be nothrow-constructible and nothrow-assignable from `nullptr`
        using function_t = utl::function<void()>;

        static_assert(std::is_nothrow_constructible_v<function_t, std::nullptr_t>);
        static_assert(std::is_nothrow_assignable_v<function_t&, std::nullptr_t>);
      }

      { // a `function` must be constructible from another `function` only if it's a non-cv-qualified prvalue or xvalue
        using function_t = utl::function<void()>;

        static_assert(std::is_nothrow_constructible_v<function_t, function_t>);
        static_assert(!std::is_constructible_v<function_t, const function_t>);
        static_assert(!std::is_constructible_v<function_t, volatile function_t>);
        static_assert(!std::is_constructible_v<function_t, const volatile function_t>);

        static_assert(!std::is_constructible_v<function_t, function_t&>);
        static_assert(!std::is_constructible_v<function_t, const function_t&>);
        static_assert(!std::is_constructible_v<function_t, volatile function_t&>);
        static_assert(!std::is_constructible_v<function_t, const volatile function_t&>);

        static_assert(std::is_nothrow_constructible_v<function_t, function_t&&>);
        static_assert(!std::is_constructible_v<function_t, const function_t&&>);
        static_assert(!std::is_constructible_v<function_t, volatile function_t&&>);
        static_assert(!std::is_constructible_v<function_t, const volatile function_t&&>);

        static_assert(std::is_nothrow_constructible_v<const function_t, function_t>);
        static_assert(std::is_nothrow_constructible_v<const function_t, function_t&&>);
      }

      { // a `function` must be assignable from another `function` only if it's a non-cv-qualified prvalue or xvalue
        using function_t = utl::function<void()>;

        static_assert(std::is_nothrow_assignable_v<function_t&, function_t>);
        static_assert(!std::is_assignable_v<function_t&, const function_t>);
        static_assert(!std::is_assignable_v<function_t&, volatile function_t>);
        static_assert(!std::is_assignable_v<function_t&, const volatile function_t>);

        static_assert(!std::is_assignable_v<function_t&, function_t&>);
        static_assert(!std::is_assignable_v<function_t&, const function_t&>);
        static_assert(!std::is_assignable_v<function_t&, volatile function_t&>);
        static_assert(!std::is_assignable_v<function_t&, const volatile function_t&>);

        static_assert(std::is_nothrow_assignable_v<function_t&, function_t&&>);
        static_assert(!std::is_assignable_v<function_t&, const function_t&&>);
        static_assert(!std::is_assignable_v<function_t&, volatile function_t&&>);
        static_assert(!std::is_assignable_v<function_t&, const volatile function_t&&>);

        static_assert(!std::is_assignable_v<const function_t&, function_t>);
        static_assert(!std::is_assignable_v<const function_t&, function_t&&>);
      }

      { // a `function` must be nothrow-constructible and nothrow-assignable from a small ("fitting") function object which has a `noexcept` move constructor
        using function_t = utl::function<void()>;

        static_assert(std::is_nothrow_constructible_v<function_t, small_function_object>);
        static_assert(std::is_nothrow_constructible_v<function_t, small_function_object&&>);

        static_assert(std::is_nothrow_assignable_v<function_t&, small_function_object>);
        static_assert(std::is_nothrow_assignable_v<function_t&, small_function_object&&>);

        // construction that would require to make a copy of the argument should be also allowed,
        // but it's not `noexcept` if the internal function's copy constructor is not `noexcept`
        static_assert(std::is_constructible_v<function_t, const small_function_object&>);
        static_assert(std::is_constructible_v<function_t, const small_function_object&&>);
        static_assert(!std::is_nothrow_constructible_v<function_t, const small_function_object&>);
        static_assert(!std::is_nothrow_constructible_v<function_t, const small_function_object&&>);

        static_assert(std::is_assignable_v<function_t&, const small_function_object&>);
        static_assert(std::is_assignable_v<function_t&, const small_function_object&&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const small_function_object&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const small_function_object&&>);

        // however, if the small internal function has a throwing move constructor, the construction should be `noexcept(false)` as well
        static_assert(std::is_constructible_v<function_t, throwing_function_object>);
        static_assert(std::is_constructible_v<function_t, throwing_function_object&&>);
        static_assert(!std::is_nothrow_constructible_v<function_t, throwing_function_object>);
        static_assert(!std::is_nothrow_constructible_v<function_t, throwing_function_object&&>);

        static_assert(std::is_assignable_v<function_t&, const throwing_function_object&>);
        static_assert(std::is_assignable_v<function_t&, const throwing_function_object&&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const throwing_function_object&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const throwing_function_object&&>);

        // same for the function objects that cannot fit into the small buffer because of their size or alignment
        static_assert(std::is_constructible_v<function_t, large_function_object>);
        static_assert(std::is_constructible_v<function_t, large_function_object&&>);
        static_assert(!std::is_nothrow_constructible_v<function_t, large_function_object>);
        static_assert(!std::is_nothrow_constructible_v<function_t, large_function_object&&>);

        static_assert(std::is_assignable_v<function_t&, const large_function_object&>);
        static_assert(std::is_assignable_v<function_t&, const large_function_object&&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const large_function_object&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const large_function_object&&>);

        static_assert(std::is_constructible_v<function_t, overaligned_function_object>);
        static_assert(std::is_constructible_v<function_t, overaligned_function_object&&>);
        static_assert(!std::is_nothrow_constructible_v<function_t, overaligned_function_object>);
        static_assert(!std::is_nothrow_constructible_v<function_t, overaligned_function_object&&>);

        static_assert(std::is_assignable_v<function_t&, const overaligned_function_object&>);
        static_assert(std::is_assignable_v<function_t&, const overaligned_function_object&&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const overaligned_function_object&>);
        static_assert(!std::is_nothrow_assignable_v<function_t&, const overaligned_function_object&&>);
      }
    }
  };

  [[maybe_unused]] static constexpr global_static_assertions_tester global_static_assertions_tester_instance{};
}
