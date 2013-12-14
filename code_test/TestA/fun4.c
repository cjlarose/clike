

void printint(int);
void printchar(char);

int f(x,y,z,a,b)
   int x,y,z,a,b;
{
  return x + y + z + a + b;
}


void main()
{
  int x, y;

  y = f(1,2,3,4,5);

  printint(y);
}

