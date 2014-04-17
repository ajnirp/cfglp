int add(int, int);
int sub(int, int);
main()
{
	int a=1, b=2, c;
	c=add(a,b);
	
}
int sub(int a, int b)
{
	int c;
	c=a-b;
	return c;
}

int add(int a,int b)
{
	int c,d;
	c=a+b;
	d=sub(c,b);
	return (d+c); 
	
}
