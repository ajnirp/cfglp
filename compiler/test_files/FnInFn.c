fn(int a)
{
	a = a - 1;
	return a;
}

main()
{
	int x;
	x = fn(2);
	x = fn(x);
	x = fn(x);
}
