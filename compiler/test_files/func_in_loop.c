int f(int);
main(){
	int x=0;
	int i;
	for(i=0;i<10;i++){
		x=f(x);
	}
}

int f(int a){
	a=(a+1)*2;
	return a;
}
