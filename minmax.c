float min(float a, float b){
	if(a<b){
		return a;
	}
	else{
		return b;
	}
}

float max(float a, float b){
	if(a>b){
		return a;
	}
	else{
		return b;
	}
}


float abs_float(float x){
	if(x>=0){
		return x;
	}
	else{
		return -x;
	}
}

float min6(float x0, float x1, float x2, float x3, float x4, float x5){
	return min(min(min(x0, x1), min(x2, x3)), min(x4, x5));
}
