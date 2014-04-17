int fun1(int, int);
int fun2(int);
main()
{
	int x;
	int y;
	x = fun1(5,2);
	y = fun1(7,3);
	x = fun1 (x,y);
}

int fun1(int a, int b)
{
	int x;
	x=a-b;
	x = fun2(x);
	return x;
} 

int fun2(int a)
{
	return a*a;
}
