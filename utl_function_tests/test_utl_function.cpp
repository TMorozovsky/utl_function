#include "override_global_new_and_delete.hpp"

#include "utl/function.hpp"

#include "tested_functions_and_function_objects.hpp"
#include "basic_utl_function_test_fixture.hpp"

// TODO : continue splitting...
class test_function/*TEMPORARY; TODO : REMOVE!!1*/ : public basic_utl_function_test
{
};




















#if 0


//--------------------------------------------------------------------------------------
TEST_F(TestFunction, CanBeMoveAssigned)
{
  { // test: assign a Function with a small function object to a Function with a large function object
    FunctionVoid source{ small_function_object{} };
    source();
    EXPECT_EQ(small_function_object::invoked_counter, 1);

    FunctionVoid destination{ large_function_object{} };
    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    destination = std::move(source);
    EXPECT_TRUE(destination);
    EXPECT_FALSE(source);

    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 1);
    EXPECT_EQ(small_function_object::invoked_counter, 2);
  }

  _CheckAndResetCounters();

  { // test: assign a Function with a large function object to a Function with an overaligned function object
    FunctionVoid source{ large_function_object{} };
    source();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    FunctionVoid destination{ overaligned_function_object{} };
    destination();
    EXPECT_EQ(overaligned_function_object::invoked_counter, 1);

    destination = std::move(source);
    EXPECT_TRUE(destination);
    EXPECT_FALSE(source);

    destination();
    EXPECT_EQ(overaligned_function_object::invoked_counter, 1);
    EXPECT_EQ(large_function_object::invoked_counter, 2);
  }

  _CheckAndResetCounters();

  { // test: assign a Function with an overaligned function object to a Function with a small function object
    FunctionVoid source{ overaligned_function_object{} };
    source();
    EXPECT_EQ(overaligned_function_object::invoked_counter, 1);

    FunctionVoid destination{ small_function_object{} };
    destination();
    EXPECT_EQ(small_function_object::invoked_counter, 1);

    destination = std::move(source);
    EXPECT_TRUE(destination);
    EXPECT_FALSE(source);

    destination();
    EXPECT_EQ(small_function_object::invoked_counter, 1);
    EXPECT_EQ(overaligned_function_object::invoked_counter, 2);
  }

  _CheckAndResetCounters();

  { // test: assign a Function with a large function object to an empty Function
    FunctionVoid source{ large_function_object{} };
    source();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    FunctionVoid destination{ nullptr };
    destination = std::move(source);
    EXPECT_TRUE(destination);
    EXPECT_FALSE(source);

    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 2);
  }

  _CheckAndResetCounters();

  { // test: assign an empty Function to a Function with a large function object
    FunctionVoid source{ nullptr };
    EXPECT_FALSE(source);

    FunctionVoid destination{ large_function_object{} };
    EXPECT_TRUE(destination);
    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    destination = std::move(source);
    EXPECT_FALSE(destination);
    EXPECT_FALSE(source);
  }

  _CheckAndResetCounters();

  { // test: assign an empty Function to an empty Function
    FunctionVoid source{ nullptr };
    EXPECT_FALSE(source);

    FunctionVoid destination{ nullptr };
    EXPECT_FALSE(source);

    destination = std::move(source);
    EXPECT_FALSE(destination);
    EXPECT_FALSE(source);
  }

  _CheckAndResetCounters();

  { // test: a Function must not change after self-assignment
    FunctionVoid func = small_function_object{};
    func();
    EXPECT_EQ(small_function_object::invoked_counter, 1);
    func = std::move(func);
    EXPECT_TRUE(func);
    func();
    EXPECT_EQ(small_function_object::invoked_counter, 2);
  }
}

//--------------------------------------------------------------------------------------
TEST_F(TestFunction, CanBeAssignedWithADifferentCallable)
{
  { // test: assign a small function object to a Function with a large function object
    small_function_object source{};
    source();
    EXPECT_EQ(small_function_object::invoked_counter, 1);

    FunctionVoid destination{ large_function_object{} };
    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    destination = source; // copy-assign
    EXPECT_TRUE(destination);
    EXPECT_EQ(small_function_object::copy_constructed_counter, 1);

    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 1);
    EXPECT_EQ(small_function_object::invoked_counter, 2);

    destination = std::move(source); // move-assign
    EXPECT_TRUE(destination);
    EXPECT_EQ(small_function_object::copy_constructed_counter, 1);
    EXPECT_GE(small_function_object::move_constructed_counter, 1u); // it is hard to determine how many times exactly was the move constructor called because it's used in internal implementation of other functions

    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 1);
    EXPECT_EQ(small_function_object::invoked_counter, 3);
  }

  _CheckAndResetCounters();

  { // test: assign a large function object to a Function with a small function object
    large_function_object source{};
    source();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    FunctionVoid destination{ small_function_object{} };
    destination();
    EXPECT_EQ(small_function_object::invoked_counter, 1);

    destination = source; // copy-assign
    EXPECT_TRUE(destination);
    EXPECT_EQ(large_function_object::copy_constructed_counter, 1);

    destination();
    EXPECT_EQ(small_function_object::invoked_counter, 1);
    EXPECT_EQ(large_function_object::invoked_counter, 2);

    destination = std::move(source); // move-assign
    EXPECT_TRUE(destination);
    EXPECT_EQ(large_function_object::copy_constructed_counter, 1);
    EXPECT_GE(large_function_object::move_constructed_counter, 1u);

    destination();
    EXPECT_EQ(small_function_object::invoked_counter, 1);
    EXPECT_EQ(large_function_object::invoked_counter, 3);
  }

  _CheckAndResetCounters();

  { // test: assign a large function object to an empty Function
    large_function_object source{};
    source();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    FunctionVoid destination{ nullptr };
    EXPECT_FALSE(destination);

    destination = source; // copy-assign
    EXPECT_TRUE(destination);
    EXPECT_EQ(large_function_object::copy_constructed_counter, 1);

    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 2);

    destination = std::move(source); // move-assign
    EXPECT_TRUE(destination);
    EXPECT_EQ(large_function_object::copy_constructed_counter, 1);
    EXPECT_GE(large_function_object::move_constructed_counter, 1u);

    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 3);
  }

  _CheckAndResetCounters();

  { // test: assign nullptr to a Function with a large function object
    FunctionVoid destination{ large_function_object{} };
    destination();
    EXPECT_EQ(large_function_object::invoked_counter, 1);

    const auto initial_destroyed_counter = large_function_object::destroyed_counter;

    destination = nullptr;
    EXPECT_FALSE(destination);
    EXPECT_EQ(large_function_object::destroyed_counter, (initial_destroyed_counter + 1));
  }
}

//--------------------------------------------------------------------------------------
namespace
{
  static unsigned int g_first_dummy_object_copy_constructed = 0;
  static unsigned int g_first_dummy_object_move_constructed = 0;
  static unsigned int g_first_dummy_object_destroyed = 0;

  static unsigned int g_second_dummy_object_copy_constructed = 0;
  static unsigned int g_second_dummy_object_move_constructed = 0;
  static unsigned int g_second_dummy_object_destroyed = 0;

  inline static void _ResetDummyObjectCounters()
  {
    g_first_dummy_object_copy_constructed = 0;
    g_first_dummy_object_move_constructed = 0;
    g_first_dummy_object_destroyed = 0;

    g_second_dummy_object_copy_constructed = 0;
    g_second_dummy_object_move_constructed = 0;
    g_second_dummy_object_destroyed = 0;
  }

  struct _FirstDummyObject
  {
    _FirstDummyObject() = default;

    _FirstDummyObject(const _FirstDummyObject&)
    {
      ++g_first_dummy_object_copy_constructed;
    }

    _FirstDummyObject(_FirstDummyObject&&) noexcept
    {
      ++g_first_dummy_object_move_constructed;
    }

    ~_FirstDummyObject()
    {
      ++g_first_dummy_object_destroyed;
    }

    int m_value = 0;
  };

  struct _SecondDummyObject
  {
    _SecondDummyObject() = default;

    _SecondDummyObject(const _SecondDummyObject&)
    {
      ++g_second_dummy_object_copy_constructed;
    }

    _SecondDummyObject(_SecondDummyObject&&) noexcept
    {
      ++g_second_dummy_object_move_constructed;
    }

    ~_SecondDummyObject()
    {
      ++g_second_dummy_object_destroyed;
    }

    int m_value = 0;
  };
}

//--------------------------------------------------------------------------------------
TEST_F(TestFunction, TestWithVariousArgumentsAndReturnValue)
{
  struct scope_guard
  {
    ~scope_guard()
    {
      _ResetDummyObjectCounters();
    }
  } scope_guard_instance;

  _FirstDummyObject first_dummy_object{};
  _SecondDummyObject second_dummy_object{};

  utl::function<int(_FirstDummyObject, _SecondDummyObject)> func_values = [](_FirstDummyObject i_first_object, _SecondDummyObject i_second_object) -> int
  {
    i_first_object.m_value = 100;
    i_second_object.m_value = 500;
    return 777;
  };

  auto result = func_values(first_dummy_object, std::move(second_dummy_object));

  EXPECT_EQ(first_dummy_object.m_value, 0);
  EXPECT_EQ(second_dummy_object.m_value, 0);

  EXPECT_EQ(g_first_dummy_object_copy_constructed, 1u);

  EXPECT_EQ(g_second_dummy_object_copy_constructed, 0);
  EXPECT_GE(g_second_dummy_object_move_constructed, 1u);

  EXPECT_EQ(result, 777);

  _ResetDummyObjectCounters();

  utl::function<int(_FirstDummyObject&, _SecondDummyObject&&)> func_refs = [](_FirstDummyObject& io_first_object, _SecondDummyObject&& io_second_object) -> int
  {
    io_first_object.m_value = 100;
    io_second_object.m_value = 500;
    return 888;
  };

  result = func_refs(first_dummy_object, std::move(second_dummy_object));

  EXPECT_EQ(first_dummy_object.m_value, 100);
  EXPECT_EQ(second_dummy_object.m_value, 500);

  EXPECT_EQ(g_first_dummy_object_copy_constructed, 0);
  EXPECT_EQ(g_first_dummy_object_move_constructed, 0);

  EXPECT_EQ(g_second_dummy_object_copy_constructed, 0);
  EXPECT_EQ(g_second_dummy_object_move_constructed, 0);

  EXPECT_EQ(result, 888);

  _ResetDummyObjectCounters();

  std::shared_ptr<_FirstDummyObject>  p_first_object_shared = std::make_shared<_FirstDummyObject>();
  std::unique_ptr<_SecondDummyObject> p_second_object_unique_captured = std::make_unique<_SecondDummyObject>();
  std::unique_ptr<_SecondDummyObject> p_second_object_unique_argument = std::make_unique<_SecondDummyObject>();

  EXPECT_EQ(p_first_object_shared.use_count(), 1);

  utl::function<int(std::unique_ptr<_SecondDummyObject>)> func_smart_pointers =
    [p_first_object_shared, p_second_object = std::move(p_second_object_unique_captured)](std::unique_ptr<_SecondDummyObject> ip_argument) mutable -> int
  {
    p_first_object_shared.reset();
    p_second_object.reset();
    return 999; // `ip_argument` should be deleted here automatically
  };

  EXPECT_EQ(p_first_object_shared.use_count(), 2);

  EXPECT_EQ(g_second_dummy_object_destroyed, 0);

  result = func_smart_pointers(std::move(p_second_object_unique_argument));

  EXPECT_EQ(g_second_dummy_object_destroyed, 2); // 2 because one was passed as an argument, and another one was captured and reset in the lambda

  EXPECT_EQ(p_first_object_shared.use_count(), 1);
  EXPECT_EQ(g_first_dummy_object_destroyed, 0);
  p_first_object_shared.reset();
  EXPECT_EQ(p_first_object_shared.use_count(), 0);
  EXPECT_EQ(g_first_dummy_object_destroyed, 1);

  EXPECT_EQ(result, 999);

  _ResetDummyObjectCounters();
}

//--------------------------------------------------------------------------------------
TEST_F(TestFunction, TestConst)
{
  int i = 5;

  auto local_functor = [&i, j = 7](int k) mutable -> int&
  {
    j += 2 * k;
    i += j;
    return j;
  };

  const utl::function<int& (int)> function{ local_functor };
  auto& ref_result = function(10);

  EXPECT_EQ(i, 32);
  EXPECT_EQ(ref_result, 27);
}

//--------------------------------------------------------------------------------------

namespace
{
  double deduce_me_plain_function(char&, _FirstDummyObject&&) { return 0.0; }
  double deduce_me_plain_function_noexcept(char&, _FirstDummyObject&&) noexcept { return 0.0; }

#ifdef _MSC_VER
   double __cdecl deduce_me_plain_function_cdecl(char&, _FirstDummyObject&&) { return 0.0; }
   double __cdecl deduce_me_plain_function_noexcept_cdecl(char&, _FirstDummyObject&&) noexcept { return 0.0; }

   double __vectorcall deduce_me_plain_function_vectorcall(char&, _FirstDummyObject&&) { return 0.0; }
   double __vectorcall deduce_me_plain_function_noexcept_vectorcall(char&, _FirstDummyObject&&) noexcept { return 0.0; }
#endif

  struct deduce_me                { float operator()(unsigned long long)                { return 0.f; } };
  struct deduce_me_const          { float operator()(unsigned long long) const          { return 0.f; } };
  struct deduce_me_volatile       { float operator()(unsigned long long) volatile       { return 0.f; } };
  struct deduce_me_const_volatile { float operator()(unsigned long long) const volatile { return 0.f; } };

  struct deduce_me_noexcept                { float operator()(unsigned long long) noexcept                { return 0.f; } };
  struct deduce_me_const_noexcept          { float operator()(unsigned long long) const noexcept          { return 0.f; } };
  struct deduce_me_volatile_noexcept       { float operator()(unsigned long long) volatile noexcept       { return 0.f; } };
  struct deduce_me_const_volatile_noexcept { float operator()(unsigned long long) const volatile noexcept { return 0.f; } };

  struct deduce_me_lvalue                { float operator()(unsigned long long) &                { return 0.f; } };
  struct deduce_me_const_lvalue          { float operator()(unsigned long long) const &          { return 0.f; } };
  struct deduce_me_volatile_lvalue       { float operator()(unsigned long long) volatile &       { return 0.f; } };
  struct deduce_me_const_volatile_lvalue { float operator()(unsigned long long) const volatile & { return 0.f; } };

  struct deduce_me_lvalue_noexcept                { float operator()(unsigned long long) & noexcept                { return 0.f; } };
  struct deduce_me_const_lvalue_noexcept          { float operator()(unsigned long long) const & noexcept          { return 0.f; } };
  struct deduce_me_volatile_lvalue_noexcept       { float operator()(unsigned long long) volatile & noexcept       { return 0.f; } };
  struct deduce_me_const_volatile_lvalue_noexcept { float operator()(unsigned long long) const volatile & noexcept { return 0.f; } };

  struct function_object_with_rvalue_call_operator { float operator()(unsigned long long) && { return 0.f; } };
  struct function_object_with_ambiguous_call_operator { float operator()() { return 0.f; } void operator()(int) { } };

  template<typename T>
  constexpr std::remove_reference_t<T>&& move(T&& arg) noexcept
  { // duplicate the implementation of `std::move` here in order to avoid warnings about applying `std::move` to const objects, which is generally a bad idea indeed
    return static_cast<std::remove_reference_t<T>&&>(arg);
  }
}

TEST_F(TestFunction, TestDeductionGuides)
{
  if (true)
  {
    utl::function func_from_plain_function         =  deduce_me_plain_function;
    utl::function func_from_plain_function_pointer = &deduce_me_plain_function;
    utl::function func_from_plain_function_noexcept         =  deduce_me_plain_function_noexcept;
    utl::function func_from_plain_function_pointer_noexcept = &deduce_me_plain_function_noexcept;

    static_assert(std::is_same_v<decltype(func_from_plain_function),                  utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_pointer),          utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_noexcept),         utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_pointer_noexcept), utl::function<double(char&, _FirstDummyObject&&)>>);

    char c = '\0'; _FirstDummyObject dummy{};
    EXPECT_EQ(func_from_plain_function(c, move(dummy)),                  0.0);
    EXPECT_EQ(func_from_plain_function_pointer(c, move(dummy)),          0.0);
    EXPECT_EQ(func_from_plain_function_noexcept(c, move(dummy)),         0.0);
    EXPECT_EQ(func_from_plain_function_pointer_noexcept(c, move(dummy)), 0.0);
  }

#ifdef _MSC_VER
  if (true)
  {
    utl::function func_from_plain_function         =  deduce_me_plain_function_cdecl;
    utl::function func_from_plain_function_pointer = &deduce_me_plain_function_cdecl;
    utl::function func_from_plain_function_noexcept         =  deduce_me_plain_function_noexcept_cdecl;
    utl::function func_from_plain_function_pointer_noexcept = &deduce_me_plain_function_noexcept_cdecl;

    static_assert(std::is_same_v<decltype(func_from_plain_function),                  utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_pointer),          utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_noexcept),         utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_pointer_noexcept), utl::function<double(char&, _FirstDummyObject&&)>>);

    char c = '\0'; _FirstDummyObject dummy{};
    EXPECT_EQ(func_from_plain_function(c, move(dummy)),                  0.0);
    EXPECT_EQ(func_from_plain_function_pointer(c, move(dummy)),          0.0);
    EXPECT_EQ(func_from_plain_function_noexcept(c, move(dummy)),         0.0);
    EXPECT_EQ(func_from_plain_function_pointer_noexcept(c, move(dummy)), 0.0);
  }

  if (true)
  {
    utl::function func_from_plain_function         =  deduce_me_plain_function_vectorcall;
    utl::function func_from_plain_function_pointer = &deduce_me_plain_function_vectorcall;
    utl::function func_from_plain_function_noexcept         =  deduce_me_plain_function_noexcept_vectorcall;
    utl::function func_from_plain_function_pointer_noexcept = &deduce_me_plain_function_noexcept_vectorcall;

    static_assert(std::is_same_v<decltype(func_from_plain_function),                  utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_pointer),          utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_noexcept),         utl::function<double(char&, _FirstDummyObject&&)>>);
    static_assert(std::is_same_v<decltype(func_from_plain_function_pointer_noexcept), utl::function<double(char&, _FirstDummyObject&&)>>);

    char c = '\0'; _FirstDummyObject dummy{};
    EXPECT_EQ(func_from_plain_function(c, move(dummy)),                  0.0);
    EXPECT_EQ(func_from_plain_function_pointer(c, move(dummy)),          0.0);
    EXPECT_EQ(func_from_plain_function_noexcept(c, move(dummy)),         0.0);
    EXPECT_EQ(func_from_plain_function_pointer_noexcept(c, move(dummy)), 0.0);
  }
#endif

  if (deduce_me object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_volatile object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const_volatile object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_volatile_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const_volatile_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_lvalue object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const_lvalue object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_volatile_lvalue object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const_volatile_lvalue object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_lvalue_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const_lvalue_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_volatile_lvalue_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (deduce_me_const_volatile_lvalue_noexcept object{}; true)
  {
    utl::function func_from_object_lvalue        = object;
    utl::function func_from_object_const_lvalue  = std::as_const(object);
    utl::function func_from_object_xvalue        = move(object);
    utl::function func_from_object_const_xvalue  = move(std::as_const(object));
    utl::function func_from_object_prvalue       = decltype(object){};
    utl::function func_from_object_const_prvalue = std::add_const_t<decltype(object)>{};

    static_assert(std::is_same_v<decltype(func_from_object_lvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_lvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_xvalue),        utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_xvalue),  utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_prvalue),       utl::function<float(unsigned long long)>>);
    static_assert(std::is_same_v<decltype(func_from_object_const_prvalue), utl::function<float(unsigned long long)>>);

    EXPECT_EQ(func_from_object_lvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_lvalue(0),  0.f);
    EXPECT_EQ(func_from_object_xvalue(0),        0.f);
    EXPECT_EQ(func_from_object_const_xvalue(0),  0.f);
    EXPECT_EQ(func_from_object_prvalue(0),       0.f);
    EXPECT_EQ(func_from_object_const_prvalue(0), 0.f);
  }

  if (function_object_with_rvalue_call_operator object{}; true)
  {
    // `operator()(...) &&`: non-ambiguous, but we cannot construct a `function` from such an object because it will not be callable from within the `function`
    static_assert(utl::detail::function::object_with_non_ambiguous_call_operator_or_reference_to_such_object<function_object_with_rvalue_call_operator>);
    static_assert(utl::detail::function::object_with_non_ambiguous_call_operator_or_reference_to_such_object<function_object_with_rvalue_call_operator&>);
    static_assert(utl::detail::function::object_with_non_ambiguous_call_operator_or_reference_to_such_object<function_object_with_rvalue_call_operator&&>);
    static_assert(!std::is_constructible_v<utl::function<float(unsigned long long)>, function_object_with_rvalue_call_operator>);
    static_assert(!std::is_constructible_v<utl::function<float(unsigned long long)>, function_object_with_rvalue_call_operator&>);
    static_assert(!std::is_constructible_v<utl::function<float(unsigned long long)>, function_object_with_rvalue_call_operator&&>);
    (void)object;
  }

  if (function_object_with_ambiguous_call_operator object{}; true)
  {
    // ambiguous `operator()`: function object must be usable with our `function` template, but CTAD won't work because of the ambiguity
    static_assert(std::is_constructible_v<utl::function<float()>, function_object_with_ambiguous_call_operator>);
    static_assert(std::is_constructible_v<utl::function<float()>, function_object_with_ambiguous_call_operator&>);
    static_assert(std::is_constructible_v<utl::function<float()>, function_object_with_ambiguous_call_operator&&>);
    static_assert(!utl::detail::function::object_with_non_ambiguous_call_operator_or_reference_to_such_object<function_object_with_ambiguous_call_operator>);
    static_assert(!utl::detail::function::object_with_non_ambiguous_call_operator_or_reference_to_such_object<function_object_with_ambiguous_call_operator&>);
    static_assert(!utl::detail::function::object_with_non_ambiguous_call_operator_or_reference_to_such_object<function_object_with_ambiguous_call_operator&&>);
    utl::function<float()> func = object;
    EXPECT_EQ(func(), 0.f);
  }
}


#endif