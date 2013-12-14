

void printdouble(double);
void printchar(char);

double x;

double multi (x,y)
  double x,y;
{
  return x * y;
}


void main()
{
  double y,z;
  double a;

  x = 3.0;
  y = 4.5;
  z = 5.0;
 
  a = multi(x,y) + z;
  printdouble(a);
  printchar(10);  // newline
}
  
