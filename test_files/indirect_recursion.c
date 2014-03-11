#include <stdio.h>

int f();

void g() {
  int g1, g2;
  g1 = g2;
  f();
}

int f() {
  g();
  return 0;
}

int main() {
  int a, b, x, y, t, gcd, lcm;
  
  a = 5;
  b = 6;
 
  while (b != 0) {
    t = b;
    b = a /  b;
    a = t;
  }
 
  gcd = f();
  lcm = (x*y)/gcd;
 
  return 0;
}
