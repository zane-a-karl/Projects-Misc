#include <iostream>
using namespace std;

int computeSign(int v) {
   int CHAR_BIT = 8;

   int sign; // the result will go here
   // CHAR_BIT is the number of bits/byte (normally 8)
   sign = -(v < 0); // if v < 0 then -1, else 0
      cout << sign << endl;
   // to avoid branching on CPUs with flag registers (IA32)
   sign = -(int)((unsigned int)((int)v) >> (sizeof(int) * CHAR_BIT-1));
      cout << sign << endl;
   // for one less instruction (but not portable)
   sign = v >> (sizeof(int) * CHAR_BIT -1);
      cout << sign << endl;
// Note the last expression above avaluates to sign = v>>31 for 32-bit ints.
// This is one operation faster than the obvious way, the first one above.
// This trick works because when signed integers are shifted right, the value
// of the far left bit is copied to the other bits. The value of the far left
// bit is one when the value is negative and 0 otherwise. All 1 bits gives -1.

   // If you prefer the result be -1 or 1:
   sign = +1 | (v >> (sizeof(int) * CHAR_BIT - 1)); // v<0 => -1, else +1
      cout << sign << endl;
   // If you want the result to be -1, 0, 1:
  sign = (v != 0) | -(int)((unsigned int)((int)v) >> (sizeof(int) * CHAR_BIT-1));
      cout << sign << endl;
   // more speed less portability
   sign = (v != 0) | (v >> (sizeof(int) * CHAR_BIT -1)); //-1,0, or 1
      cout << sign << endl;
   // for protability, brevity, and (perhaps) speed:
   sign = (v>0) - (v<0); // -1,0, or 1
      cout << sign << endl;
   return sign;
}

int main() {
   int v = 0;
   cout << "Please input an integer" << endl;
   cin >> v;
   computeSign(v);
   return 0;
}
