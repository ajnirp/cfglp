int add(int);
main()
{
	int f,b=1,n=5;
	f=fact(n);

}

int fact (int n1)
{
	if (n1==1||n1==0)
		return 1;
	else
		return fact(n1-1);

}
