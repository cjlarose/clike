
void printint(int);
void printchar(char);

int fib(n)
  int n;
{
  if (n<=1) return n;

  return fib(n-1) + fib(n-2);
}

void main()
{

  int x;

  x = fib(0);
  printint(x);
  printchar(10);

  x = fib(1);
  printint(x);
  printchar(10);

  x = fib(2);
  printint(x);
  printchar(10);

  x = fib(3);
  printint(x);
  printchar(10);

  x = fib(4);
  printint(x);
  printchar(10);

  x = fib(5);
  printint(x);
  printchar(10);

  x = fib(6);
  printint(x);
  printchar(10);
}
