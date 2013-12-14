

char a[6];
void printchar(char);

void printint(int);

void main()
{
   int x;

   a[0] = -12;
   a[1] = 10;
   a[2] = -8;
   a[3] = 6;
   a[4] = -4;
   a[5] = 2;

   x = a[5];
   printint(x);    // 2
   printchar(10);

   printint(a[0]);  // -12
   printchar(10);

   printint(10+a[1]);  // 20
   printchar(10);

   x = a[2]*10 + a[3] + a[4];
   printint(x);  // should be -78
   printchar(10);

}
  
