int f(int a, int b)
{
	if(a>b)
	return 1;
	else
	return 0;
}

main()
{
	int x;
	int y;
	x=10;
	y=5;
	int i;
	i = f(x,y);
	if(i)
	return x;
	else
	return y;
}
