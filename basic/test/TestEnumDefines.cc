#include "../EnumDefines.h"
#include <iostream>
#include <thread>
#include <vector>

#define X 1
const int Y = 2;

DefineEnums(A, a);
DefineEnums(B, a, b, c);
DefineEnums(C, a = 1, b, c);
DefineEnums(D, a=1, b, c);
DefineEnums(E, a=1, b=2, c=3);
DefineEnums(F, x=50505, y, z);

// not working for G
DefineEnums(G, a=Y, b, c);

void TestEnum2Str() {
  std::cout << A2Str(A::a) << std::endl;
  std::cout << B2Str(B::a) << " " << B2Str(B::b) << " " << B2Str(B::c) << std::endl;
  std::cout << B2Str(B::a) << " " << B2Str(B::b) << " " << B2Str(B::c) << std::endl;
  std::cout << C2Str(C::a) << " " << C2Str(C::b) << " " << C2Str(C::c) << std::endl;
  std::cout << D2Str(D::a) << " " << D2Str(D::b) << " " << D2Str(D::c) << std::endl;
  std::cout << E2Str(E::a) << " " << E2Str(E::b) << " " << E2Str(E::c) << std::endl;
  std::cout << F2Str(F::x) << " " << F2Str(F::y) << " " << F2Str(F::z) << std::endl;
  // std::cout << StrG(G::a) << " " << StrG(G::b) << " " << StrG(G::c) << std::endl;

  /*
  std::cout << StrA(A::a).Str() << std::endl;
  std::cout << StrB(B::a).Str() << " " << StrB(B::b).Str() << " " << StrB(B::c).Str() << std::endl;
  std::cout << StrB(B::a).Str() << " " << StrB(B::b).Str() << " " << StrB(B::c).Str() << std::endl;
  std::cout << StrC(C::a).Str() << " " << StrC(C::b).Str() << " " << StrC(C::c).Str() << std::endl;
  std::cout << StrD(D::a).Str() << " " << StrD(D::b).Str() << " " << StrD(D::c).Str() << std::endl;
  std::cout << StrE(E::a).Str() << " " << StrE(E::b).Str() << " " << StrE(E::c).Str() << std::endl;
  std::cout << StrF(F::x).Str() << " " << StrF(F::y).Str() << " " << StrF(F::z).Str() << std::endl;
  */
}

void TestMultiThread() {
  std::vector<std::thread> v;
  for (int i = 0; i < 10; ++i) {
    v.push_back(std::thread([] {
      TestEnum2Str();
    }));
  }
  for (int i = 0; i < 10; ++i) {
    v[i].join();
  }
}

int main() {
  TestEnum2Str();
}
