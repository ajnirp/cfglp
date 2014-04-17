float sum_of_n1_no(float);
main()
{
	float n1=21,c;
	
	
	c=sum_of_n1_no(n1);

}

float sum_of_n1_no (float n)
{
int t=0,i;
	
for(i=1;i<=n;i++)
	t=t+i;	
	
return t;
}
