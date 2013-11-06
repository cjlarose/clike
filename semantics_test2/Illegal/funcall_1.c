
int id(x)
 int x;
{ return x; }

something (x) 
  int x;
{
  x = id(x);

  return x;
}

