

// 4 args

void printint(int);
void printchar(char);

int f(x, y, z, c)
   int x, y, z;
   char c;
{
  return x + y * z + c;
}


void main()
{
  int x, y;

  x = 17;
  y = f(1, x, x*2, 100);

  printint(y);
}

