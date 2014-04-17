fn(int a)
{
	a = a + 1;
	return a;
}

main()
{
	int a = 2, b = 3, c = 4;
	float d = 2.3, e = 3.4, f = 4.5;
	int x,y;
	x = fn(2);
	y = fn(4);
	a = a + x / e / c * d / y - e;
}
