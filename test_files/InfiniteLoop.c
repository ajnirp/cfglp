main()
{
   int a;

<bb 2>:
   a = 1;
   goto <bb 4>;

<bb 3>:
   goto <bb 5>;

<bb 4>:
   goto <bb 3>;

<bb 5>:
   return;
}