/* taken from compiler-rt source code */

unsigned long int __popcountdi2(unsigned long long a){
  a = a - ((a >> 1) & 0x5555555555555555uLL);
  a = ((a >> 2) & 0x3333333333333333uLL) + (a & 0x3333333333333333uLL);
  a = (a + (a >> 4)) & 0x0F0F0F0F0F0F0F0FuLL;
  unsigned long int x = (unsigned long int)(a + (a >> 32));
  x = x + (x >> 16);
  return (x + (x >> 8)) & 0x0000007F;
}
