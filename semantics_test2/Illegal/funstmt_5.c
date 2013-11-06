
void dosomething(x)
  int x;
{ 
 
}


void work()
{
   dosomething(1);
}


void morework()
{
  dosomething(work());
}
  
