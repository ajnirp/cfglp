int f(int);
int g(int);
int h(int,int);
main(){
	int a=10;
	a=h(f(a),g(a));
}

int f(int a){
	return a+1;
}

int g(int a){
	return a-4;
}

int h(int a, int b){
	return a*b;
}
