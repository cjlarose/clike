
void f(i,j, x,y, c,d)
int i,j;
double x,y;
char c,d;
{
  // TYpe compatible with +, -, *, /, unary -
  i = (c+j) + (c*j) + (c-j) + (c/j) + (-c);

  x = (x+y) + (x*y) + (x-y) + (x/y) + (-x);

  c = (c+i) + (c*i) + (c-i) + (c/i) + (-i);
}

