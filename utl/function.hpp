#pragma once

#include <new>
#include <utility>
#include <type_traits>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <cassert>

#ifdef _MSC_VER
#  include <xstddef>
#endif

namespace utl
{
  namespace detail::function
  {
    // Size of the small buffer used by the `function` class template. If a wrapped function object can fit in such a small buffer,
    // it will be allocated in-place, so that the unnecessary dynamic memory allocation could be avoided.
    inline constexpr auto small_buffer_size = 4 * sizeof(void*) - sizeof(unsigned int);

    // Alignment of the small buffer used by the `function` class template.
    inline constexpr auto small_buffer_alignment = alignof(void*);

    // Interface of the internal function wrapper used by the `function` class template.
    // Each non-empty `function` object owns an internal function wrapper via a pointer to this type.
    // The owned wrapper can be either stored in-place inside its parent `function` object or allocated dynamically,
    // depending on its size, alignment, and whether it is nothrow move-constructible.
    template<typename Result, typename... Args>
    struct internal_function_wrapper_interface
    {
      virtual Result invoke(Args... args) = 0;
      virtual internal_function_wrapper_interface* move_construct_at(void* destination) && noexcept = 0;
      virtual ~internal_function_wrapper_interface() = default;
    };

    // Implementation of the internal function wrapper interface that stores a function object of a particular type.
    template<typename WrappedFunction, typename Result, typename... Args>
    struct internal_function_wrapper final : internal_function_wrapper_interface<Result, Args...>
    {
      static_assert(!std::is_reference_v<WrappedFunction>);

      template<typename CtorArg>
      constexpr explicit internal_function_wrapper(CtorArg&& ctor_arg) noexcept(std::is_nothrow_constructible_v<WrappedFunction, CtorArg&&>)
        : wrapped_function{std::forward<CtorArg>(ctor_arg)}
      {
      }

      virtual Result invoke(Args... args) override
      {
        if constexpr (std::is_void_v<Result>)
          this->wrapped_function(std::forward<Args>(args)...);
        else
          return this->wrapped_function(std::forward<Args>(args)...);
      }

      // Invokes the move constructor of `*this` object at the `destination` address.
      // The size and the alignment of the destination chunk of memory must be sufficient to store a respective Wrapper instance;
      // construction of a new Wrapper with an rvalue reference to a wrapped function must never throw
      // (otherwise, the owner `function` object should not have called this virtual function, preferring to move around a pointer to a dynamically allocated wrapper instead).
      virtual internal_function_wrapper_interface<Result, Args...>* move_construct_at(void* destination) && noexcept override
      {
        using ThisWrapper = internal_function_wrapper<WrappedFunction, Result, Args...>;
        assert(reinterpret_cast<std::uintptr_t>(destination) % alignof(ThisWrapper) == 0); // `destination` must be properly aligned
        assert((reinterpret_cast<std::uintptr_t>(destination) + sizeof(ThisWrapper) < reinterpret_cast<std::uintptr_t>(this)) ||
               (reinterpret_cast<std::uintptr_t>(destination) > reinterpret_cast<std::uintptr_t>(this) + sizeof(ThisWrapper))); // `destination` must not overlap with `*this`
        assert((std::is_nothrow_constructible_v<ThisWrapper, WrappedFunction&&>)); // in-place construction must be chosen only for nothrow-move-constructible function objects
        auto* new_wrapper = ::new (destination) ThisWrapper{std::move(this->wrapped_function)};
        return new_wrapper; // implicit cast to a pointer to the parent interface
      }

    private:
      WrappedFunction wrapped_function;
    };

    // Concept that tells whether an lvalue of type `Function` can be called with a pack of `Args`.
    template<typename Function, typename... Args>
    concept callable_with_args = requires(Function& function, Args&&... args)
    {
      { function(std::forward<Args>(args)...) };
    };

    // Concept that tells whether it's okay for a `function<Result(Args...)>` to store a callable object
    // of the type `DecayedCallable` inside its internal small buffer
    // (as the name suggests, `DecayedCallable` must be a type to which the `std::decay` type trait has been already applied).
    template<typename DecayedCallable, typename Result, typename... Args>
    concept fitting_in_small_buffer = (sizeof(internal_function_wrapper<DecayedCallable, Result, Args...>) <= small_buffer_size) &&
                                      (alignof(internal_function_wrapper<DecayedCallable, Result, Args...>) <= small_buffer_alignment) &&
                                      std::is_nothrow_move_constructible_v<DecayedCallable>;

    template<typename T>
    concept member_function_pointer = std::is_member_function_pointer_v<T>;

    // Concept that tells whether the expression `&T::operator()` is well-formed and non-ambiguous,
    // where `T` is the argument type with its reference and top-level cv-qualifiers removed.
    template<typename FunctionObjectOrReference>
    concept object_or_reference_with_non_ambiguous_call_operator = requires
    {
      { &std::remove_cv_t<std::remove_reference_t<FunctionObjectOrReference>>::operator() } -> member_function_pointer;
    };

    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...))                           { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const)                     { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) volatile)                  { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const volatile)            { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) noexcept)                  { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const noexcept)            { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) volatile noexcept)         { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const volatile noexcept)   { return std::type_identity<R(A...)>{}; }

    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) &)                         { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const &)                   { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) volatile &)                { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const volatile &)          { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) & noexcept)                { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const & noexcept)          { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) volatile & noexcept)       { return std::type_identity<R(A...)>{}; }
    template<typename F, typename R, typename... A> constexpr auto get_deduced_signature_identity(R(F::*)(A...) const volatile & noexcept) { return std::type_identity<R(A...)>{}; }

    // Note: we do not deduce signatures for function object types with &&-qualified operator() since using such objects with our `function` template is not supported.
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) &&)                         = delete;
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) const &&)                   = delete;
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) volatile &&)                = delete;
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) const volatile &&)          = delete;
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) && noexcept)                = delete;
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) const && noexcept)          = delete;
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) volatile && noexcept)       = delete;
    template<typename F, typename R, typename... A> constexpr void get_deduced_signature_identity(R(F::*)(A...) const volatile && noexcept) = delete;

    template<typename FunctionObjectOrReference>
    using deduced_signature_t = typename decltype(get_deduced_signature_identity(&std::remove_cv_t<std::remove_reference_t<FunctionObjectOrReference>>::operator()))::type;
  }

  // The generic `function` template.
  // Essentially, it is a simplified version of `std::function` that does not impose the "copyable" requirement on its internal function objects.
  // Thus, unlike `std::function`, this `function` can be used with move-only types.
  // Trying to invoke an empty `function` is undefined behavior (unlike `std::function` which throws an exception in that case).
  template<typename Signature>
  class function;

  template<typename Result, typename... Args>
  class function<Result(Args...)> final
  {
    using internal_function_wrapper_interface_t = detail::function::internal_function_wrapper_interface<Result, Args...>;

  public:
    function() noexcept
      : offset{static_cast<unsigned int>(-1)}
    { // Empty state: offset = -1 (meaning that the buffer should be treated as a storage of a pointer); the stored pointer is null.
      this->set_pointer_stored_in_buffer(nullptr);
    }

    /* implicit */ function(std::nullptr_t) noexcept
      : function{}
    {
    }

    function(function&& other) noexcept
    {
      this->move_initialize_from(std::move(other));
    }

    template<typename CallableOrReference>
    requires detail::function::callable_with_args<std::decay_t<CallableOrReference>, Args...> &&
             (!std::is_same_v<std::decay_t<CallableOrReference>, function<Result(Args...)>>) &&
             detail::function::fitting_in_small_buffer<std::decay_t<CallableOrReference>, Result, Args...>
      /* implicit */ function(CallableOrReference&& callable)
      noexcept(std::is_nothrow_constructible_v<detail::function::internal_function_wrapper<std::decay_t<CallableOrReference>, Result, Args...>,
                                               CallableOrReference&&>)
    {
      // Construct a wrapper for this callable entity in-place.
      using internal_function_wrapper_t = detail::function::internal_function_wrapper<std::decay_t<CallableOrReference>, Result, Args...>;

      auto* constructed_wrapper = ::new (&this->buffer[0]) internal_function_wrapper_t{std::forward<CallableOrReference>(callable)};
      internal_function_wrapper_interface_t* constructed_wrapper_interface = constructed_wrapper; // cast to an interface which might introduce a non-zero offset

      this->offset = static_cast<unsigned int>(reinterpret_cast<unsigned char*>(constructed_wrapper_interface) -
                                               reinterpret_cast<unsigned char*>(&this->buffer[0]));
    }

    template<typename CallableOrReference>
    requires detail::function::callable_with_args<std::decay_t<CallableOrReference>, Args...> &&
             (!std::is_same_v<std::decay_t<CallableOrReference>, function<Result(Args...)>>) &&
             (!detail::function::fitting_in_small_buffer<std::decay_t<CallableOrReference>, Result, Args...>)
      /* implicit */ function(CallableOrReference&& callable)
      noexcept(false)
    {
      // Allocate a wrapper for this callable entity dynamically.
      using internal_function_wrapper_t = detail::function::internal_function_wrapper<std::decay_t<CallableOrReference>, Result, Args...>;

      auto* new_wrapper = new internal_function_wrapper_t{std::forward<CallableOrReference>(callable)};

      this->offset = static_cast<unsigned int>(-1);
      this->set_pointer_stored_in_buffer(new_wrapper);
    }

    function& operator=(function&& other) noexcept
    {
      if (std::addressof(other) != this)
      {
        this->clear();
        this->move_initialize_from(std::move(other));
      }
      return *this;
    }

    template<typename CallableOrReference>
    requires detail::function::callable_with_args<std::decay_t<CallableOrReference>, Args...> &&
             (!std::is_same_v<std::decay_t<CallableOrReference>, function<Result(Args...)>>)
      function& operator=(CallableOrReference&& callable)
      noexcept(detail::function::fitting_in_small_buffer<std::decay_t<CallableOrReference>, Result, Args...> &&
               std::is_nothrow_constructible_v<detail::function::internal_function_wrapper<std::decay_t<CallableOrReference>, Result, Args...>,
                                               CallableOrReference&&>)
    {
      function<Result(Args...)> temporary_function{std::forward<CallableOrReference>(callable)};
      return (*this = std::move(temporary_function));
    }

    function& operator=(std::nullptr_t) noexcept
    {
      this->clear();
      return *this;
    }

    ~function()
    {
      this->clear();
    }

    Result operator()(Args... args) const
    {
      auto* wrapper_interface = this->get_internal_function_wrapper_interface();
      assert(wrapper_interface);
      return wrapper_interface->invoke(std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept
    {
      return (this->get_internal_function_wrapper_interface() != nullptr);
    }

  private:
    void move_initialize_from(function&& other) noexcept // preconditions: &other != this; *this object is empty
    {
      if (auto* other_wrapper = other.get_internal_function_wrapper_interface())
      { // If the other object has an internal function wrapper, it might have been either constructed in-place or allocated dynamically.
        if (other.offset != static_cast<unsigned int>(-1))
        { // Other's internal function wrapper has been constructed in-place (so its wrapped function should be nothrow move-constructible) - move it to this object's buffer.
          internal_function_wrapper_interface_t* constructed_wrapper = std::move(*other_wrapper).move_construct_at(&this->buffer[0]);

          this->offset = other.offset; // Offset can be taken directly from the other object since their data representation is the same.
          assert(reinterpret_cast<unsigned char*>(constructed_wrapper) - reinterpret_cast<unsigned char*>(&this->buffer[0]) == this->offset); // the offset value copied from `other` must be correct
          (void)constructed_wrapper; // used only in the assertion above

          other.clear();
        }
        else
        { // Other's internal function wrapper has been allocated dynamically - "steal" the pointer from the other object.
          this->offset = static_cast<unsigned int>(-1);
          this->set_pointer_stored_in_buffer(other_wrapper);
          other.set_pointer_stored_in_buffer(nullptr);
        }
      }
      else
      { // Other object is empty - make this object empty as well.
        this->offset = static_cast<unsigned int>(-1);
        this->set_pointer_stored_in_buffer(nullptr);
      }
    }

    internal_function_wrapper_interface_t* get_internal_function_wrapper_interface() const noexcept
    { // Note: this function is `const`, but it returns a non-const pointer to the internal data of this object!
      if (this->offset != (unsigned int)(-1))
        return reinterpret_cast<internal_function_wrapper_interface_t*>(this->buffer + this->offset);
      else
        return *reinterpret_cast<internal_function_wrapper_interface_t**>(&this->buffer[0]);
    }

    void set_pointer_stored_in_buffer(internal_function_wrapper_interface_t* new_pointer_value) noexcept
    {
      std::memcpy(&this->buffer[0], &new_pointer_value, sizeof(internal_function_wrapper_interface_t*));
    }

    void clear() noexcept
    {
      if (this->offset != static_cast<unsigned int>(-1))
      { // The buffer stores an internal function wrapper object in-place.
        auto* wrapper = reinterpret_cast<internal_function_wrapper_interface_t*>(this->buffer + this->offset);
        wrapper->~internal_function_wrapper_interface_t();
        this->offset = static_cast<unsigned int>(-1);
      }
      else
      { // The buffer represents a pointer to the internal function wrapper which is either null or references a dynamically allocated object.
        auto* wrapper = *reinterpret_cast<internal_function_wrapper_interface_t**>(&this->buffer[0]);
        delete wrapper;
      }
      set_pointer_stored_in_buffer(nullptr);
    }

  private:
    // Buffer which represents either an in-place storage of an internal function wrapper, or a pointer to such wrapper that has been allocated dynamically.
    // Mutable because this object's call operator might actually change its internal state (if the wrapper is stored in-place) despite being qualified as `const`.
    alignas(detail::function::small_buffer_alignment) mutable unsigned char buffer[detail::function::small_buffer_size];

    // If this integer value is equal to -1, the buffer stores a pointer to an internal function wrapper which has been allocated dynamically, or that pointer is null;
    // otherwise such a function wrapper has been constructed in-place in this object, and this value represents the offset to that wrapper's interface inside the buffer.
    unsigned int offset;
  };

#ifndef _MSC_VER
  template<typename Result, typename... Args>
  function(Result(*)(Args...)) -> function<Result(Args...)>;
#else
#define UTL_FUNCTION_DEDUCTION_GUIDE_FOR_FUNCTION_POINTER(CALL_OPT, X1, X2, X3) \
  template<typename Result, typename... Args>                        \
  function(Result(CALL_OPT*)(Args...)) -> function<Result(Args...)>;

  _NON_MEMBER_CALL(UTL_FUNCTION_DEDUCTION_GUIDE_FOR_FUNCTION_POINTER, X1, X2, X3)
#undef UTL_FUNCTION_DEDUCTION_GUIDE_FOR_FUNCTION_POINTER
#endif

  template<detail::function::object_or_reference_with_non_ambiguous_call_operator FunctionObjectOrReference>
  function(FunctionObjectOrReference) -> function<detail::function::deduced_signature_t<FunctionObjectOrReference>>;
}
