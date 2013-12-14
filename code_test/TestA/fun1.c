
void printint(int);
void printchar(char);

int f(x)
   int x;
{
  return x + 1;
}


void main()
{
  int x, y;

  x = 17;
  y = f(x);

  printint(y);
}

