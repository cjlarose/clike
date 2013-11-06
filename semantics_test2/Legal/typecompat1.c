
void f(i,j, x,y, c,d)
int i,j;
double x,y;
char c,d;
{
  // TYpe compatible with +, -, *, /, unary -
  i = (i+j) + (i*j) + (i-j) + (i/j) + (-i);

  x = (x+y) + (x*y) + (x-y) + (x/y) + (-x);

  c = (c+d) + (c*d) + (c-d) + (c/d) + (-c);
}

