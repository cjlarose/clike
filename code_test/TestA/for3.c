

void printint(int);

void main()
{
  int i,j, sum;

  sum = 0;
  i = 0;
  for (; i<10; i = i + 1) {
     for (j=i; j<10; j = j+1) {
        sum = sum + j;
     };

  };
  printint(sum);
}

