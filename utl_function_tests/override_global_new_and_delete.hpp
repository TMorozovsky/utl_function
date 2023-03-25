#pragma once

// This file must be included in every *.cpp file of the unit tests' project before ANY other #include directives!

inline thread_local unsigned int counter_of_allocations_done_by_operator_new = 0;
inline thread_local unsigned int counter_of_deallocations_done_by_operator_delete = 0;

namespace memory_management_detail
{
  using size_t = decltype(sizeof(0));

  [[nodiscard]] inline void* impl_operator_new(size_t size);
  inline void impl_operator_delete(void* address) noexcept;
}

[[nodiscard]] inline void* operator new(memory_management_detail::size_t size)
{
  return memory_management_detail::impl_operator_new(size);
}

[[nodiscard]] inline void* operator new[](memory_management_detail::size_t size)
{
  return memory_management_detail::impl_operator_new(size);
}

inline void operator delete(void* address) noexcept
{
  memory_management_detail::impl_operator_delete(address);
}

inline void operator delete[](void* address) noexcept
{
  memory_management_detail::impl_operator_delete(address);
}

#include <cstdlib>
#include <new>

[[nodiscard]] inline void* memory_management_detail::impl_operator_new(size_t size)
{
  auto result = std::malloc(size);
  if (!result) [[unlikely]]
    throw std::bad_alloc{};
  ++counter_of_allocations_done_by_operator_new;
  return result;
}

inline void memory_management_detail::impl_operator_delete(void* address) noexcept
{
  std::free(address);
  ++counter_of_deallocations_done_by_operator_delete;
}
