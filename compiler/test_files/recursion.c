float f(float);
main(){
	float x=10;
	float i;
	i=f(x);
}

float f(float a){
	float c;
	if(a<=0) return 0;
	c = f(a-2);
	c = c + a/2;
	return c;
}
