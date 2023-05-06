#include<stdio.h>
void howManyBits(int x)
{
  int flag=x>>31;     //����λ
  x=x^flag;
  int b16,b8,b4,b2,b1,b0;

  b16=!!(x>>16)<<4; //��16λ��1Ϊ16 û��1Ϊ0
  x=x>>b16;

  b8=!!(x>>8)<<3;   //��8λ��1Ϊ8 û��1Ϊ0
  x=x>>b8;

  b4=!!(x>>4)<<2;   //��4λ��1Ϊ4 û��1Ϊ0
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