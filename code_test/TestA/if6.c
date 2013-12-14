



void printint(int);

void main()
{
  int x;
  int a, b, c;

  a = 2;
  b = 3;
  c = 4;


  if (a==2) {
    printint(a);    // Not true!
  } else {
    printint(666);  // expecting 666
  };
}
  
