



void printint(int);
void printchar(char);

int f1(x,y,z,a,b)
   int x,y,z,a,b;
{
  return x + y + z + a + b;
}


int cascade()
{
  int x, y;

  y = f1(1,2,3,4,5);

  return y;
}



int f(x, y, z, a,b,c,d,e)
   int x, y, z;
   char a,b,c,d,e;
{
  return  (e)*(a + b * (c - d) + x * y + z);
}


// Mix it up!
void main()
{
  int x, y;

  x = 17;
  y = f(x, x*2, x/3, 1,2,3,4, 5);

  printint(y + cascade() + f1(1,1,1,1,1));

}


 
