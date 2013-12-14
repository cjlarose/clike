


// 8 args

void printint(int);
void printchar(char);

int f(x, y, z, a,b,c,d,e)
   int x, y, z;
   char a,b,c,d,e;
{
  return  (e)*(a + b * (c - d) + x * y + z);
}


void main()
{
  int x, y;

  x = 17;
  y = f(x, x*2, x/3, 1,2,3,4, 5);

  printint(y);
}

