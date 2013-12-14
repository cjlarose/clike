

int a[10];


void printint(int);
void printchar(char);

void main()
{
  int n, i,j, temp;

  // initialize and print before
  i = 0;
  n = 10;
  while (i<10) {
    a[i] = 10 - i;

    printint(a[i]);
    printchar(10);   // newline

    i = i + 1;
  };
  printchar(33); // !
  printchar(10); // newline

  // Selection sort
  i = 0;
  while (i<n) {
    j = i;
    while (j<n) {
      if (a[j] < a[i]) {
        // Swap 
	temp = a[i];
        a[i] = a[j];
        a[j] = temp;
      };
      j = j + 1;
    };
    i = i + 1;
  };

  // Print after all sorted
  printchar(35); // #
  printchar(10); // newline
  i = 0;
  while (i<n) {
    printint(a[i]);
    printchar(10);
    i = i + 1;
  };   
}
  
