#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <iostream>


#include "lockstrap_simple.h"

class Simple {
  int a;
  float b;
  std::string c;
  std::vector<long> x;
  LCKSTRAPSIMP(Simple, std::mutex, a,b,c,x);
};

void TestSimple()
{
  Simple simp;
  auto al = simp.access();
  // At this point, 'simp' data is locked and we can access it.  It
  // will be unlocked when 'al' gets out of scope.
  al.a() = 2;
  al.b() = 2.1;
  al.c() = "2.X";
  al.x().emplace_back(2L);
  std::cout << al.a() << " " << al.b() << " " << al.c() << " " << al.x().size() << std::endl;
  al.a() = 3;
  al.b() = 3.1;
  al.c() = "3.X";
  al.x().emplace_back(3L);

  // With C++14 compiler, you can replace 'Simple::locker' with 'auto'
  simp.with([](Simple::locker l) {
      // Within the "with block" the locker holds the lock, so it will be
      // unlocked at the end of this lambda.
      std::cout << l.a() << " " << l.b() << " " << l.c() << " " << l.x().size() << std::endl;
      l.a() = 4;
      l.b() = 4.1;
      l.c() = "4.X";
      l.x().emplace_back(4L);
    });
  std::cout << al.a() << " " << al.b() << " " << al.c() << " " << al.x().size() << std::endl;
}

#include "lockstrap.h"

class User {
  int a;
  float b;
  std::string c;
  std::vector<long> x;
  LOCKSTRAP(User, std::mutex, a,b,c,x);
};

void TestRegular()
{
  User usr;
  // Except for a more "natural" syntax, that is, avoiding "()", the
  // usage is the same for the "regular" as for the "simple"
  // lock-strap.
  auto al = usr.access();
  al.a = 2;
  al.b = 2.2;
  al.c = "2.Y";
  al.x.emplace_back(2L);
  std::cout << al.a << " " << al.b << " " << al.c << " " << al.x.size() << std::endl;
  al.a = 3;
  al.b = 3.2;
  al.c = "3.Y";
  al.x.emplace_back(3L);

  // With C++14 compiler, you can replace 'User::locker' with 'auto'
  usr.with([](User::locker l) {
      std::cout << l.a << " " << l.b << " " << l.c << " " << l.x.size() << std::endl;
      l.a = 4;
      l.b = 4.2;
      l.c = "4.Y";
      l.x.emplace_back(4L);
    });
  std::cout << al.a << " " << al.b << " " << al.c << " " << al.x.size() <<std::endl;
}


int main()
{
  TestSimple();
  TestRegular();

  return 0;
}
