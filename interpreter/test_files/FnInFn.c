fn(int a)
{
	a = a - 1;
	return a;
}

main()
{
	fn(fn(fn(2)));
}
