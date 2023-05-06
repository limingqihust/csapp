#include<stdio.h>
int conditional(int x, int y, int z) {
  return (y+z)>>1 + ((y-z)>>1)^(0-!x) + !x; 
}
int main()
{
    printf("%d",conditional(1,2,3));
}