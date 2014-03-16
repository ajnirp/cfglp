fn(int a)
{
	a = a + 1;
	return a;
}

main()
{
	int a = 2, b = 3, c = 4;
	float d = 2.3, e = 3.4, f = 4.5;

	a = a + fn(2) / e / c * d / fn(4) - e;
}
