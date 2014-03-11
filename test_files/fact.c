int f(int);
main(){
	int x=4;
	int fib;
	fib=f(x);
}

int f(int a){
	if(a==0)
		return 1;
	else
		return a*f(a-1);
}
