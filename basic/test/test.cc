#include "../EnumDefines.h"
#include <iostream>

DefineEnums(A, a);
DefineEnums(B, a, b, c);
DefineEnums(C, a = 1, b, c);
DefineEnums(D, a=1, b, c);
DefineEnums(E, a=1, b=2, c=3);
DefineEnums(F, x=50505, y, z);

enum class X {};
int main() {
  std::cout << StrA(A::a) << std::endl;
  std::cout << StrB(B::a) << " " << StrB(B::b) << " " << StrB(B::c) << std::endl;
  std::cout << StrC(C::a) << " " << StrC(C::b) << " " << StrC(C::c) << std::endl;
  std::cout << StrD(D::a) << " " << StrD(D::b) << " " << StrD(D::c) << std::endl;
  std::cout << StrE(E::a) << " " << StrE(E::b) << " " << StrE(E::c) << std::endl;
  std::cout << StrF(F::x) << " " << StrF(F::y) << " " << StrF(F::z) << std::endl;
}
