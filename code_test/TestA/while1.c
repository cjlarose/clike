

void printint(int);
void printchar(char);

void main()
{
  int x, i;

  i = 10;
  while (i>=0) {
     printint(i);
     i = i - 2;
     printchar(10);  // newline
  };

}
  
