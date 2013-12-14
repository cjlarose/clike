
void printint(int);

int sideffect()
{
   printint(666);  // Will see this if this gets evaulated
   return 1;
}


void main()
{
  int x, y;

  x = 17; 
  y = 16;

  if (x==y && sideffect()==1) {
       printint(777);  // shouldn't see this
  };
}

