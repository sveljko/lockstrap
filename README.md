lockstrap
=========

A few macros to keep data and it's lock together, for C++11 and
later.

In multithreaded programming, locks (mostly mutexes) are used a lot.
Actually, a lot more than they should be, but that is another topic.

The problem with locks is that there is no way to associate a lock
with some data. Let's say you have class like this:

	class User {
		int a;
		float b;
		std::string c;
		std::vector<long> x;
		
		std::mutex m;
		// ...
	};
	
Quick question: Which data does 'm' protect?

You can't tell. It might be all, but then it might not. It would be
even worse if there were two mutexes in that class. You have to
actually look at the code to figure it out. And code can be a mess.
You can resort to comments (good luck with that) or naming conventions
(which quickly get out of hand).

But even if your comments are good or naming conventions are feasable,
there is nothing preventing bad usage - protecting data when it
shouldn't be and vice versa.

Well, these "lock-strap" macros provide a way to fix that.  I know, I
wish they were not macros, but I'm pretty certain that with
C++11/C++14 there is no way to avoid macros (and keep a nice syntax
for users).


Usage
-----

Using the same example as before:

	#include "lockstrap.h"
	class User {
		class Data {
			int a;
			float b;
			std::string c;
			LOCKSTRAP(Data, std::mutex, a,b,c);
		} d;
		std::vector<long> x;
		// ...
	};

So, we introduced a class to hold the data which is protected by a
mutex (which is declared in the 'Data' class by the "LOCKSTRAP"
macro). The vector 'x' is, now obviously, not protected by the mutex.

Since 'Data' is a class, obviously you can't access 'a', 'b' and 'c'
from the outside. The 'LOCKSTRAP' macro defines two helper member
functions for such purposes - the 'access' and the template 'with'.

This is how you would use them in some User member function(s):

	void User::f()
	{
		auto al = d.access();
		// at this point, mutex is locked, you can access and do..
		// whatever.
		al.a = 3;
		al.b = al.a / 2;
		// mutex will be unloced when 'al' goes out of scope here
	}

	void User::g()
	{
		// In C++14, 'auto' can replace 'User::locker'
		d.with([](User::locker l) {
			// at this point, mutex is locked, you can access and do..
			// whatever.
			l.a = 33;
			l.c.append(std::to_string(l.b));
			// mutex will be unloced when 'l' goes out of scope here
			});
		// Of course, 'with' can accept any callable object,
		// even a function pointer.
	}

You can combine both 'access' and 'with' in the same function, but
that would be strange.

The simple implementation
-------------------------

Simple implementation has very similar usage. Include "lockstrap_simple.h"
instead of "lockstrap.h" and use "LCKSTRAPSIMP" instead of "LOCKSTRAP"
macro, and when accessing data always use function call syntax. Here's
the full example with simple implementation:

	#include "lockstrap_simple.h"
	class User {
		class Data {
			int a;
			float b;
			std::string c;
			LCKSTRAPSIMP(Data, std::mutex, a,b,c);
		} d;
		std::vector<long> x;
		// ...
	};

	void User::f()
	{
		auto al = d.access();
		al.a() = 3;
		al.b() = al.a() / 2;
	}

	void User::g()
	{
		d.with([](User::locker l) { l.c().append(std::to_string(l.b())); });
	}

You can use the "simple" implementation for some classes and the
regular for others, but doing that in the same file would be a little
confusing to the reader.


Why use simple at all?
----------------------

Well, you may prefer this syntax with all those (), as it hints
that this is not your regular access.

Even if you don't, It compiles faster. How faster depends, on how you
use it. But, in basic tests when code did little else but access this
data, it was about 15% faster. With more code non-lock related, the
relative speedup will be smaller, but, OTOH, if you have a lot of code
with locking than it might be a significant absolute speedup.

In theory, the "regular" implementation may generate worse code,
because it declares a "shadow" reference for each protected data
member.  In all tests, especially with optimizations on, the generated
code is actually the same, as everything is inlined.

Also, for most compilers, simple will give somewhat nicer errors on
bad usage.


Remarks
-------

The lock doesn't have to be a mutex. It can be anything that
implements the "Lockable" concept, that is, has a "lock()" and
"unlock()" member functions.

Since you have to "mention" every data member, you may make a mistake:

* If you omit a member, compiler will give you a "class XXX::locker
does not have a member ..." error which is a good hint that you have
to add it.

* If you give bad data member name, compiler will give an error like
"XXX::bad_name doesn't exist", which is also a good hint

The initial implementation can handle up to 9 data members. It is easy
to add more, look at the comments in the headers.

Obviously, this was not designed to handle static data (though it will
work, to an extent) or member functions - which will give strange
compiler errors - well, you know that member functions can't be
protected by a mutex, right?


Discussion
----------

Implementation is nothing special:

* The macro generates a lock data member in class itself
* Then it generates a nested class named "locker" which will be used
  to access the data
* For simple implementation, the "locker" has a reference to the
  "real" object and has a bunch of member functions with the same
  names as the data members of the "real" class
* For regular implementation, the "locker" has a reference for
  each data member of the real class, with exactly the same name
* In any case, "locker" will lock the lock on construction and
  unlock on destruction (yup, RAII)
* The macro generates an "access" member function which will
  return a "locker" object
* The macro generates a "with" member template function which will
  accept a templatized callable parameter and create a "locker"
  object which it will pass to that parameter

Why can't this be done with templates?

Well, you may do something like a "smart pointer" if you make the data
"public" in the "real" class, but that defies the purpose, as the data
is now public an anyone can use it without the lock.

If you keep the data private, then you may use something like a tuple,
but that doesn't generate the symbol names, so you would use different
(and rather ugly) syntax. There are other tricks to be done here, but
all with the same "tuple" problem.

C++11 makes the implementation and usage a lot easier, with variable
arguments macros and decltype. Actually, with C++14, you can use
'auto' instead of 'decltype(ME::x)' when declaring the "forwarding
functions" in the "simple" implementation.

One could do similar stuff in C++03, but would have to re-declare the
types of all data (and call the macro "version" with the right number
of data):

	class Data {
		int a;
		float b;
		std::string c;
		LCKSTRAPSIMP3(Data, std::mutex, int, a, float, b, std::string, c);
	} d;

or restore to something like:

	DECL_LOCKSTRAP_CLASS(User)
	DECL_LOCKSTRAP_MEMBER(int, a);
	DECL_LOCKSTRAP_MEMBER(float, b);
	END_LOCKSTRAP_CLASS(User)

