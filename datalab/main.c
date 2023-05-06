#include<stdio.h>
void howManyBits(int x)
{
  int flag=x>>31;     //符号位
  x=x^flag;
  int b16,b8,b4,b2,b1,b0;

  b16=!!(x>>16)<<4; //高16位有1为16 没有1为0
  x=x>>b16;

  b8=!!(x>>8)<<3;   //高8位有1为8 没有1为0
  x=x>>b8;

  b4=!!(x>>4)<<2;   //高4位有1为4 没有1为0
  x=x>>b4;

  b2=!!(x>>2)<<1;
  x=x>>b2;

  b1=!!(x>>1);
  x=x>>b1;

  b0=!!(x);

  printf("%d\n",b16+b8+b4+b2+b1+b0+1);
}
int main()
{

  howManyBits(298);    // 10
  howManyBits(-5);     // 4
}