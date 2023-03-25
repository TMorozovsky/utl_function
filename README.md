# Unnamed Template Library: function
The `utl::function<R(Ts...)>` template is pretty much similar to `std::function` but doesn't require its underlying functor to be copyable.

It's a C++20-based alternative to `std::move_only_function` from C++23.

Lacks some functionality that is present in its `std` counterparts, but is likely to compile and perform faster.
