

double x,y;
char d,e;
double zz,c;



void f()
{
  int x;
  int y;
  double d,e;
  char c, zz;

  // these are type compatible as we use the locals
  x = y + 1;
  d = d+ 17.0;
  c = zz + c;

}

// SHould be able to reuse all these variables
void g()
{
  // These all become type imcompatible as we use the globals
  x = y + 1;
  d = d+ 17.0;
  c = zz + c;
}

