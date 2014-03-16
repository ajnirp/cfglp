float f(float);
main(){
	float x=10;
	float i;
	i=f(x);
}

float f(float a){
	if(a<=0) return 0;
	return a/2+f(a-2);
}
