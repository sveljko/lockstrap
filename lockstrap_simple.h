/**********************************************************
 * Copyright (C) Srdjan Veljkovic, 2014
 *
 * This code is licensed to you as per the "New BSD" license,
 * which should be in the LICENSE file.
 **********************************************************/

#if !defined INC_LCKSTRAP_SIMPLE
#define      INC_LCKSTRAP_SIMPLE

/* These internal macros do "step macro development". Don't use them
   outside this header.  But, if you ever make a class with more than
   elements (thus steps) present here , just do some copy pasting and
   add more steps...
 */
#define LCKSTRAPSIMP1(ME, x) decltype(ME::x) &x() {return d_me.x;} 
#define LCKSTRAPSIMP2(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP1(ME, __VA_ARGS__)
#define LCKSTRAPSIMP3(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP2(ME, __VA_ARGS__)
#define LCKSTRAPSIMP4(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP3(ME, __VA_ARGS__)
#define LCKSTRAPSIMP5(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP4(ME, __VA_ARGS__)
#define LCKSTRAPSIMP6(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP5(ME, __VA_ARGS__)
#define LCKSTRAPSIMP7(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP6(ME, __VA_ARGS__)
#define LCKSTRAPSIMP8(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP7(ME, __VA_ARGS__)
#define LCKSTRAPSIMP9(ME, x, ...) decltype(ME::x) &x() {return d_me.x;} LCKSTRAPSIMP8(ME, __VA_ARGS__)

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
	LCKSTRAPSIMP(MyClass, std::mutex, my_int, my_float);
    };

    Then, use the class like this:

    MyClass x;
    auto xl = x.access();
    xl.my_int() = 5;
    std::cout << xl.my_int() << std::endl;
    x.with([](MyClass::locker l) { l.my_int() = 6 });

    Remember to keep the GET_MACRO "call" up-to-date with the maximum
    number of steps supported.

    @param ME The name of the class we are adding a "lock strap" to
    @param LOCK The type of the lock (implements the "Lockable"
    concept - that is, has a "lock()" and "unlock()" member
    functions.)
 */
#define LCKSTRAPSIMP(ME, LOCK, ...)					\
  private: LOCK d_locker;						\
public: class locker {							\
  ME &d_me;								\
  public:locker(ME &me): d_me(me) {me.d_locker.lock();}			\
  ~locker() {d_me.d_locker.unlock();}					\
  GET_MACRO(__VA_ARGS__, LCKSTRAPSIMP9,LCKSTRAPSIMP8,LCKSTRAPSIMP7,LCKSTRAPSIMP6,LCKSTRAPSIMP5,LCKSTRAPSIMP4,LCKSTRAPSIMP3,LCKSTRAPSIMP2,LCKSTRAPSIMP1)(ME, __VA_ARGS__) \
    };									\
  locker access() { return locker(*this); }				\
  template <typename F> void with(F f) { f(locker(*this)); }


#endif //!defined INC_LCKSTRAP_SIMPLE

