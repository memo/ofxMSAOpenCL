//--------------------------------------------------------------
__kernel void inverse(__global float *a, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = 1.0f/a[gid];
}

//--------------------------------------------------------------
__kernel void square(__global float *a, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = a[gid] * a[gid];
}

//--------------------------------------------------------------
__kernel void multiplyScaler(__global float *a, float b, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = a[gid] * b;
}

