/**********************************************************
 * Copyright (C) Srdjan Veljkovic, 2014
 *
 * This code is licensed to you as per the "New BSD" license,
 * which should be in the LICENSE file.
 **********************************************************/

#if !defined INC_LOCKSTRAP
#define      INC_LOCKSTRAP


/* These internal macros do "step macro development". Don't use them
   outside this header.  But, if you ever make a class with more than
   elements (thus steps) present here , just do some copy pasting and
   add more steps...
 */
#define LCKSTRPDCL1(ME, x) decltype(ME::x) &x
#define LCKSTRPDCL2(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL1(ME, __VA_ARGS__)
#define LCKSTRPDCL3(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL2(ME, __VA_ARGS__)
#define LCKSTRPDCL4(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL3(ME, __VA_ARGS__)
#define LCKSTRPDCL5(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL4(ME, __VA_ARGS__)
#define LCKSTRPDCL6(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL5(ME, __VA_ARGS__)
#define LCKSTRPDCL7(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL6(ME, __VA_ARGS__)
#define LCKSTRPDCL8(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL7(ME, __VA_ARGS__)
#define LCKSTRPDCL9(ME, x, ...) decltype(ME::x) &x; LCKSTRPDCL8(ME, __VA_ARGS__)

/* Ditto... */
#define LCKSTRPINIT1(ME, x), x(me.x)
#define LCKSTRPINIT2(ME, x, ...), x(me.x) LCKSTRPINIT1(ME, __VA_ARGS__)
#define LCKSTRPINIT3(ME, x, ...), x(me.x) LCKSTRPINIT2(ME, __VA_ARGS__)
#define LCKSTRPINIT4(ME, x, ...), x(me.x) LCKSTRPINIT3(ME, __VA_ARGS__)
#define LCKSTRPINIT5(ME, x, ...), x(me.x) LCKSTRPINIT4(ME, __VA_ARGS__)
#define LCKSTRPINIT6(ME, x, ...), x(me.x) LCKSTRPINIT5(ME, __VA_ARGS__)
#define LCKSTRPINIT7(ME, x, ...), x(me.x) LCKSTRPINIT6(ME, __VA_ARGS__)
#define LCKSTRPINIT8(ME, x, ...), x(me.x) LCKSTRPINIT7(ME, __VA_ARGS__)
#define LCKSTRPINIT9(ME, x, ...), x(me.x) LCKSTRPINIT8(ME, __VA_ARGS__)

// The generic macro to help with "step macro development". If you
// ever make a class with more than elements present here, just add
// parameters before the "NAME" parameter
#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9, NAME, ...) NAME

/** Define a safe lock "strap" (or band, binder...) over data of a
    class. It will add a "lock object" of the type of your choice to
    your class and enable you to access the data only under that lock.

    This is the macro to be used outside this header, like this:

    class MyClass {
        int my_int;
	float my_float;
	LOCKSTRAP(MyClass, std::mutex, my_int, my_float);
    };

    Then, use the class like this:

    MyClass x;
    auto xl = x.access();
    xl.my_int = 5;
    std::cout << xl.my_int << std::endl;
    x.with([](MyClass::locker l) { l.my_int = 6 });
    // or, in C++14:
    x.with([](auto l) { l.my_int = 6 });

    Remember to keep the GET_MACRO "calls" up-to-date with the maximum
    number of steps supported.

    @param ME The name of the class we are adding a "lock strap" to
    @param LOCK The type of the lock (implements the "Lockable"
    concept - i.e., has a "lock()" and "unlock()" member functions.)
 */
#define LOCKSTRAP(ME, LOCK, ...)					\
  private: LOCK d_locker;						\
public: class locker {							\
    ME &d_me;								\
public:									\
 GET_MACRO(__VA_ARGS__, LCKSTRPDCL9,LCKSTRPDCL8,LCKSTRPDCL7,LCKSTRPDCL6,LCKSTRPDCL5,LCKSTRPDCL4,LCKSTRPDCL3,LCKSTRPDCL2,LCKSTRPDCL1)(ME, __VA_ARGS__); \
 locker(ME &me) : d_me(me) GET_MACRO(__VA_ARGS__, LCKSTRPINIT9,LCKSTRPINIT8,LCKSTRPINIT7,LCKSTRPINIT6,LCKSTRPINIT5,LCKSTRPINIT4,LCKSTRPINIT3,LCKSTRPINIT2,LCKSTRPINIT1)(ME, __VA_ARGS__) \
  { me.d_locker.lock(); }						\
 ~locker() { d_me.d_locker.unlock(); }					\
};									\
  locker access() { return locker(*this); }				\
  template <typename F> void with(F f) { f(locker(*this)); }


#endif // !defined INC_LOCKSTRAP
