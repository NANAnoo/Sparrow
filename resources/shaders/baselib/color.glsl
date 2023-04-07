
float rgb2luma(vec3 rgb){
	return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}