

void printint(int);
void printchar(char);

int f(x, y)
   int x, y;
{
  return x * y;
}


void main()
{
  int x, y;

  x = 17;
  y = f(x, 1+1+3);

  printint(y);
}

