int Fibonacci(int n);
 
main()
{
   int n, i = 0, c;
 
   for ( c = 1 ; c <= 13 ; c++ )
   {
      Fibonacci(i);
      i++; 
   }
 
   return 0;
}
 
int Fibonacci(int n)
{
   if ( n == 0 )
      return 0;
   else if ( n == 1 )
      return 1;
   else{
      int x,y;
      x =Fibonacci(n-1);
      y = Fibonacci(n-2);
      return (x+y);
   }
} 
