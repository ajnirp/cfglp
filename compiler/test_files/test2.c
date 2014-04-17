float div(int , int, int, int);

int sum(int, int);

int sub(int, int);

main()
{
	int a=10,b=9,c=20,d=21;
	float m;
	m =div(a,b,c,d);
}

float div(int a, int b, int e, int f)
{
	int c,d;
	float m;
	c=sum(a,b);
	d=sub(d,e);
	m=c/d;
	return(m);
}

int sum(int a, int b)
{
	return(a+b);
}


int sub(int a, int b)
{
	return(a-b);
}
