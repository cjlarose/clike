


void printint(int);
void printchar(char);

void main()
{
  int x, i, n;
  int f_0, f_1;

  f_0 = 0;
  f_1 = 1;
  n   = 10;
 
  // compute fibonaccis iteratively
  i = 0;  // compute n times
  while (i<n) {
     x = f_0 + f_1;
     printint(x);  // print fib number
     printchar(10);  // newline
     
     // update f_0 and f_1
     f_0 = f_1;
     f_1 = x;

     i = i + 1;
    
  };

}
  
