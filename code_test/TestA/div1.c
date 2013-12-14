

void printchar(char);
void printint(int);

void main()
{
  int x;
  int a, b, c;

  a = 200;
  b = 3;
  c = 4;

  
  x = a/c;
  printint(x);
  printchar(10); // newline

  x = a/b;
  printint(x);
  printchar(10);
  
  x = b/c;
  printint(x);
  printchar(10);
  
}
  
