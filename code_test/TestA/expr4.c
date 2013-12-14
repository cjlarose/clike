



void printint(int);

void main()
{
  char c;
  int x;
  int y;

  c = -120;  // Does sign extension work?
  x = 100;

  y = c + x;
  printint(y) ; // Precedence work?
}
  
