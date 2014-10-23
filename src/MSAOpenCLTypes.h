#pragma once

#include "ofMain.h"

    typedef ofVec2f float2;
    typedef ofVec3f float3;
    typedef ofVec4f float4;
    
namespace msa {
    
    class OpenCL;
    
    class OpenCLKernel;
    typedef shared_ptr<OpenCLKernel> OpenCLKernelPtr;
    
    class OpenCLProgram;
    typedef shared_ptr<OpenCLProgram> OpenCLProgramPtr;
}
//
//class float2 {
//public:
//
//	float x, y;
//
//    float2( float _x=0.0f, float _y=0.0f );
//    float2( const float2& pnt );
//
//
//    // Getters and Setters.
//    //
//    void set( float _x, float _y );
//    void set( const float2& vec );
//    float &operator[]( const int& i );
//
//
//    // Check similarity/equality.
//    //
//    bool operator==( const float2& vec );
//    bool operator!=( const float2& vec );
//    bool match( const float2& vec, float tollerance=0.0001 );
//    /**
//	 * Checks if vectors look in the same direction.
//	 * Tollerance is specified in degree.
//	 */
//    bool align( const float2& vec, float tollerance=0.0001 ) const;
//    bool alignRad( const float2& vec, float tollerance=0.0001 ) const;
//
//
//    // Overloading for any type to any type
//    //
//    void 	  operator=( const float2& vec );
//    float2  operator+( const float2& vec ) const;
//    float2& operator+=( const float2& vec );
//    float2  operator-( const float2& vec ) const;
//    float2& operator-=( const float2& vec );
//    float2  operator*( const float2& vec ) const;
//    float2& operator*=( const float2& vec );
//    float2  operator/( const float2& vec ) const;
//    float2& operator/=( const float2& vec );
//
//
//    //operator overloading for float
//    //
//    void 	  operator=( const float f);
//    float2  operator+( const float f ) const;
//    float2& operator+=( const float f );
//    float2  operator-( const float f ) const;
//    float2& operator-=( const float f );
//    float2  operator-() const;
//    float2  operator*( const float f ) const;
//    float2& operator*=( const float f );
//    float2  operator/( const float f ) const;
//    float2& operator/=( const float f );
//
//
//    // Scaling
//    //
//    float2  getScaled( const float length ) const;
//    float2& scale( const float length );
//
//
//    // Rotation
//    //
//    float2  getRotated( float angle ) const;
//    float2  getRotatedRad( float angle ) const;
//    float2& rotate( float angle );
//    float2& rotateRad( float angle );
//
//
//    // Rotation - point around pivot
//    //
//    float2  getRotated( float angle, const float2& pivot ) const;
//    float2& rotate( float angle, const float2& pivot );
//    float2  getRotatedRad( float angle, const float2& pivot ) const;
//    float2& rotateRad( float angle, const float2& pivot );
//
//
//    // Map point to coordinate system defined by origin, vx, and vy.
//    //
//    float2 getMapped( const float2& origin,
//					   const float2& vx,
//					   const float2& vy ) const;
//    float2& map( const float2& origin,
//				  const float2& vx, const float2& vy );
//
//
//    // Distance between two points.
//    //
//    float distance( const float2& pnt) const;
//    float squareDistance( const float2& pnt ) const;
//
//
//    // Linear interpolation.
//    //
//    //
//    /**
//	 * p==0.0 results in this point, p==0.5 results in the
//	 * midpoint, and p==1.0 results in pnt being returned.
//	 */
//    float2   getInterpolated( const float2& pnt, float p ) const;
//    float2&  interpolate( const float2& pnt, float p );
//    float2   getMiddle( const float2& pnt ) const;
//    float2&  middle( const float2& pnt );
//    float2&  average( const float2* points, int num );
//
//
//    // Normalization
//    //
//    float2  getNormalized() const;
//    float2& normalize();
//
//
//    // Limit length.
//    //
//	float2  getLimited(float max) const;
//    float2& limit(float max);
//
//
//    // Perpendicular normalized vector.
//    //
//    float2  getPerpendicular() const;
//    float2& perpendicular();
//
//
//    // Length
//    //
//    float length() const;
//    float squareLength() const;
//
//
//    /**
//	 * Angle (deg) between two vectors.
//	 * This is a signed relative angle between -180 and 180.
//	 */
//    float angle( const float2& vec ) const;
//    float angleRad( const float2& vec ) const;
//
//
//    /**
//	 * Dot Product.
//	 */
//    float dot( const float2& vec ) const;
//
//
//
//    //---------------------------------------------------
//    // this methods are deprecated in 006 please use:
//
//    // getScaled
//    float2 rescaled( const float length ) const;
//
//    // scale
//    float2& rescale( const float length );
//
//    // getRotated
//    float2 rotated( float angle ) const;
//
//    // getNormalized
//    float2 normalized() const;
//
//    // getLimited
//    float2 limited(float max) const;
//
//    // getPerpendicular
//    float2 perpendiculared() const;
//
//    // squareLength
//    float lengthSquared() const;
//
//    // getInterpolated
//    float2 interpolated( const float2& pnt, float p ) const;
//
//    // getMiddled
//    float2 middled( const float2& pnt ) const;
//
//    // getMapped
//    float2 mapped( const float2& origin, const float2& vx, const float2& vy ) const;
//
//    // squareDistance
//    float distanceSquared( const float2& pnt ) const;
//
//    // use getRotated
//    float2 rotated( float angle, const float2& pivot ) const;
//};
//
//
//
//
//
//// Non-Member operators
////
//float2 operator+( float f, const float2& vec );
//float2 operator-( float f, const float2& vec );
//float2 operator*( float f, const float2& vec );
//float2 operator/( float f, const float2& vec );
//
//
//
//
//
//
//
///////////////////
//// Implementation
///////////////////
//
//
//inline float2::float2( float _x, float _y ) {
//	x = _x;
//	y = _y;
//}
//
//inline float2::float2( const float2& pnt ) {
//	x = pnt.x;
//	y = pnt.y;
//}
//
//
//
//// Getters and Setters.
////
////
//inline void float2::set( float _x, float _y ) {
//	x = _x;
//	y = _y;
//}
//
//inline void float2::set( const float2& vec ) {
//	x = vec.x;
//	y = vec.y;
//}
//
//inline float& float2::operator[]( const int& i ) {
//	switch(i) {
//		case 0:  return x;
//		case 1:  return y;
//		default: return x;
//	}
//}
//
//
//
//// Check similarity/equality.
////
////
//inline bool float2::operator==( const float2& vec ) {
//	return (x == vec.x) && (y == vec.y);
//}
//
//inline bool float2::operator!=( const float2& vec ) {
//	return (x != vec.x) || (y != vec.y);
//}
//
//inline bool float2::match( const float2& vec, float tollerance ) {
//	return (fabs(x - vec.x) < tollerance)
//	&& (fabs(y - vec.y) < tollerance);
//}
//
///**
// * Checks if vectors look in the same direction.
// * Tollerance is specified in degree.
// */
//inline bool float2::align( const float2& vec, float tollerance ) const {
//	return  fabs( this->angle( vec ) ) < tollerance;
//}
//
//inline bool float2::alignRad( const float2& vec, float tollerance ) const {
//	return  fabs( this->angleRad( vec ) ) < tollerance;
//}
//
//
//// Overloading for any type to any type
////
////
//inline void float2::operator=( const float2& vec ){
//	x = vec.x;
//	y = vec.y;
//}
//
//inline float2 float2::operator+( const float2& vec ) const {
//	return float2( x+vec.x, y+vec.y);
//}
//
//inline float2& float2::operator+=( const float2& vec ) {
//	x += vec.x;
//	y += vec.y;
//	return *this;
//}
//
//inline float2 float2::operator-( const float2& vec ) const {
//	return float2(x-vec.x, y-vec.y);
//}
//
//inline float2& float2::operator-=( const float2& vec ) {
//	x -= vec.x;
//	y -= vec.y;
//	return *this;
//}
//
//inline float2 float2::operator*( const float2& vec ) const {
//	return float2(x*vec.x, y*vec.y);
//}
//
//inline float2& float2::operator*=( const float2& vec ) {
//	x*=vec.x;
//	y*=vec.y;
//	return *this;
//}
//
//inline float2 float2::operator/( const float2& vec ) const {
//	return float2( vec.x!=0 ? x/vec.x : x , vec.y!=0 ? y/vec.y : y);
//}
//
//inline float2& float2::operator/=( const float2& vec ) {
//	vec.x!=0 ? x/=vec.x : x;
//	vec.y!=0 ? y/=vec.y : y;
//	return *this;
//}
//
//
////operator overloading for float
////
////
//inline void float2::operator=( const float f){
//	x = f;
//	y = f;
//}
//
//inline float2 float2::operator+( const float f ) const {
//	return float2( x+f, y+f);
//}
//
//inline float2& float2::operator+=( const float f ) {
//	x += f;
//	y += f;
//	return *this;
//}
//
//inline float2 float2::operator-( const float f ) const {
//	return float2( x-f, y-f);
//}
//
//inline float2& float2::operator-=( const float f ) {
//	x -= f;
//	y -= f;
//	return *this;
//}
//
//inline float2 float2::operator-() const {
//	return float2(-x, -y);
//}
//
//inline float2 float2::operator*( const float f ) const {
//	return float2(x*f, y*f);
//}
//
//inline float2& float2::operator*=( const float f ) {
//	x*=f;
//	y*=f;
//	return *this;
//}
//
//inline float2 float2::operator/( const float f ) const {
//	if(f == 0) return float2(x, y);
//
//	return float2(x/f, y/f);
//}
//
//inline float2& float2::operator/=( const float f ) {
//	if(f == 0) return *this;
//
//	x/=f;
//	y/=f;
//	return *this;
//}
//
//inline float2 float2::rescaled( const float length ) const {
//	return getScaled(length);
//}
//
//inline float2 float2::getScaled( const float length ) const {
//	float l = (float)sqrt(x*x + y*y);
//	if( l > 0 )
//		return float2( (x/l)*length, (y/l)*length );
//	else
//		return float2();
//}
//
//inline float2& float2::rescale( const float length ){
//	return scale(length);
//}
//
//inline float2& float2::scale( const float length ) {
//	float l = (float)sqrt(x*x + y*y);
//	if (l > 0) {
//		x = (x/l)*length;
//		y = (y/l)*length;
//	}
//	return *this;
//}
//
//
//
//// Rotation
////
////
//inline float2 float2::rotated( float angle ) const {
//	return getRotated(angle);
//}
//
//inline float2 float2::getRotated( float angle ) const {
//	float a = (float)(angle*DEG_TO_RAD);
//	return float2( x*cos(a) - y*sin(a),
//					x*sin(a) + y*cos(a) );
//}
//
//inline float2 float2::getRotatedRad( float angle ) const {
//	float a = angle;
//	return float2( x*cos(a) - y*sin(a),
//					x*sin(a) + y*cos(a) );
//}
//
//inline float2& float2::rotate( float angle ) {
//	float a = (float)(angle * DEG_TO_RAD);
//	float xrot = x*cos(a) - y*sin(a);
//	y = x*sin(a) + y*cos(a);
//	x = xrot;
//	return *this;
//}
//
//inline float2& float2::rotateRad( float angle ) {
//	float a = angle;
//	float xrot = x*cos(a) - y*sin(a);
//	y = x*sin(a) + y*cos(a);
//	x = xrot;
//	return *this;
//}
//
//
//
//// Rotate point by angle (deg) around pivot point.
////
////
//
//// This method is deprecated in 006 please use getRotated instead
//inline float2 float2::rotated( float angle, const float2& pivot ) const {
//	return getRotated(angle, pivot);
//}
//
//inline float2 float2::getRotated( float angle, const float2& pivot ) const {
//	float a = (float)(angle * DEG_TO_RAD);
//	return float2( ((x-pivot.x)*cos(a) - (y-pivot.y)*sin(a)) + pivot.x,
//					((x-pivot.x)*sin(a) + (y-pivot.y)*cos(a)) + pivot.y );
//}
//
//inline float2& float2::rotate( float angle, const float2& pivot ) {
//	float a = (float)(angle * DEG_TO_RAD);
//	float xrot = ((x-pivot.x)*cos(a) - (y-pivot.y)*sin(a)) + pivot.x;
//	y = ((x-pivot.x)*sin(a) + (y-pivot.y)*cos(a)) + pivot.y;
//	x = xrot;
//	return *this;
//}
//
//inline float2 float2::getRotatedRad( float angle, const float2& pivot ) const {
//	float a = angle;
//	return float2( ((x-pivot.x)*cos(a) - (y-pivot.y)*sin(a)) + pivot.x,
//					((x-pivot.x)*sin(a) + (y-pivot.y)*cos(a)) + pivot.y );
//}
//
//inline float2& float2::rotateRad( float angle, const float2& pivot ) {
//	float a = angle;
//	float xrot = ((x-pivot.x)*cos(a) - (y-pivot.y)*sin(a)) + pivot.x;
//	y = ((x-pivot.x)*sin(a) + (y-pivot.y)*cos(a)) + pivot.y;
//	x = xrot;
//	return *this;
//}
//
//
//
//// Map point to coordinate system defined by origin, vx, and vy.
////
////
//
//// This method is deprecated in 006 please use getMapped instead
//inline float2 float2::mapped( const float2& origin,
//								 const float2& vx,
//								 const float2& vy ) const{
//	return getMapped(origin, vx, vy);
//}
//
//inline float2 float2::getMapped( const float2& origin,
//									const float2& vx,
//									const float2& vy ) const
//{
//	return float2( origin.x + x*vx.x + y*vy.x,
//					origin.y + x*vx.y + y*vy.y );
//}
//
//inline float2& float2::map( const float2& origin,
//							   const float2& vx, const float2& vy )
//{
//	float xmap = origin.x + x*vx.x + y*vy.x;
//	y = origin.y + x*vx.y + y*vy.y;
//	x = xmap;
//	return *this;
//}
//
//
//// Distance between two points.
////
////
//inline float float2::distance( const float2& pnt) const {
//	float vx = x-pnt.x;
//	float vy = y-pnt.y;
//	return (float)sqrt(vx*vx + vy*vy);
//}
//
////this method is deprecated in 006 please use squareDistance
//inline float float2::distanceSquared( const float2& pnt ) const {
//	return squareDistance(pnt);
//}
//
//inline float float2::squareDistance( const float2& pnt ) const {
//	float vx = x-pnt.x;
//	float vy = y-pnt.y;
//	return vx*vx + vy*vy;
//}
//
//
//
//// Linear interpolation.
////
////
///**
// * p==0.0 results in this point, p==0.5 results in the
// * midpoint, and p==1.0 results in pnt being returned.
// */
//
//// this method is deprecated in 006 please use getInterpolated
//inline float2 float2::interpolated( const float2& pnt, float p ) const{
//	return getInterpolated(pnt, p);
//}
//
//inline float2 float2::getInterpolated( const float2& pnt, float p ) const {
//	return float2( x*(1-p) + pnt.x*p, y*(1-p) + pnt.y*p );
//}
//
//inline float2& float2::interpolate( const float2& pnt, float p ) {
//	x = x*(1-p) + pnt.x*p;
//	y = y*(1-p) + pnt.y*p;
//	return *this;
//}
//
//// this method is deprecated in 006 please use getMiddle
//inline float2 float2::middled( const float2& pnt ) const{
//	return getMiddle(pnt);
//}
//
//inline float2 float2::getMiddle( const float2& pnt ) const {
//	return float2( (x+pnt.x)/2.0f, (y+pnt.y)/2.0f );
//}
//
//inline float2& float2::middle( const float2& pnt ) {
//	x = (x+pnt.x)/2.0f;
//	y = (y+pnt.y)/2.0f;
//	return *this;
//}
//
//
//
//// Average (centroid) among points.
//// Addition is sometimes useful for calculating averages too.
////
////
//inline float2& float2::average( const float2* points, int num ) {
//	x = 0.f;
//	y = 0.f;
//	for( int i=0; i<num; i++) {
//		x += points[i].x;
//		y += points[i].y;
//	}
//	x /= num;
//	y /= num;
//	return *this;
//}
//
//
//
//// Normalization
////
////
//inline float2 float2::normalized() const {
//	return getNormalized();
//}
//
//inline float2 float2::getNormalized() const {
//	float length = (float)sqrt(x*x + y*y);
//	if( length > 0 ) {
//		return float2( x/length, y/length );
//	} else {
//		return float2();
//	}
//}
//
//inline float2& float2::normalize() {
//	float length = (float)sqrt(x*x + y*y);
//	if( length > 0 ) {
//		x /= length;
//		y /= length;
//	}
//	return *this;
//}
//
//
//
//// Limit length.
////
////
//inline float2 float2::limited(float max) const{
//	return getLimited(max);
//}
//
//inline float2 float2::getLimited(float max) const {
//    float2 limited;
//    float lengthSquared = (x*x + y*y);
//    if( lengthSquared > max*max && lengthSquared > 0 ) {
//        float ratio = max/(float)sqrt(lengthSquared);
//        limited.set( x*ratio, y*ratio);
//    } else {
//        limited.set(x,y);
//    }
//    return limited;
//}
//
//inline float2& float2::limit(float max) {
//    float lengthSquared = (x*x + y*y);
//    if( lengthSquared > max*max && lengthSquared > 0 ) {
//        float ratio = max/(float)sqrt(lengthSquared);
//        x *= ratio;
//        y *= ratio;
//    }
//    return *this;
//}
//
//
//
//// Perpendicular normalized vector.
////
////
//inline float2 float2::perpendiculared() const {
//	return getPerpendicular();
//}
//
//inline float2 float2::getPerpendicular() const {
//	float length = (float)sqrt( x*x + y*y );
//	if( length > 0 )
//		return float2( -(y/length), x/length );
//	else
//		return float2();
//}
//
//inline float2& float2::perpendicular() {
//	float length = (float)sqrt( x*x + y*y );
//	if( length > 0 ) {
//		float _x = x;
//		x = -(y/length);
//		y = _x/length;
//	}
//	return *this;
//}
//
//
//// Length
////
////
//inline float float2::length() const {
//	return (float)sqrt( x*x + y*y );
//}
//
//inline float float2::lengthSquared() const {
//	return squareLength();
//}
//
//inline float float2::squareLength() const {
//	return (float)(x*x + y*y);
//}
//
//
//
///**
// * Angle (deg) between two vectors.
// * This is a signed relative angle between -180 and 180.
// */
//inline float float2::angle( const float2& vec ) const {
//	return (float)(atan2( x*vec.y-y*vec.x, x*vec.x + y*vec.y )*RAD_TO_DEG);
//}
//
///**
// * Angle (deg) between two vectors.
// * This is a signed relative angle between -180 and 180.
// */
//inline float float2::angleRad( const float2& vec ) const {
//	return atan2( x*vec.y-y*vec.x, x*vec.x + y*vec.y );
//}
//
//
///**
// * Dot Product.
// */
//inline float float2::dot( const float2& vec ) const {
//	return x*vec.x + y*vec.y;
//}
//
//
//
//
//
//
//
//// Non-Member operators
////
////
//inline float2 operator+( float f, const float2& vec ) {
//    return float2( f+vec.x, f+vec.y);
//}
//
//inline float2 operator-( float f, const float2& vec ) {
//    return float2( f-vec.x, f-vec.y);
//}
//
//inline float2 operator*( float f, const float2& vec ) {
//    return float2( f*vec.x, f*vec.y);
//}
//
//inline float2 operator/( float f, const float2& vec ) {
//    return float2( f/vec.x, f/vec.y);
//}
//
//
//
////---------------------------------------------------------
//class float4 {
//public:
//	cl_float x, y, z, w;
//
//	float4() {
//	}
//
//	float4( float _x, float _y, float _z, float _w = 0.0f) {
//        x = _x;
//        y = _y;
//        z = _z;
//		w = _w;
//    }
//
//    float4( const float4 & pnt){
//        x = pnt.x;
//        y = pnt.y;
//        z = pnt.z;
//		w = pnt.w;
//    }
//
//    void set(float _x, float _y, float _z, float _w = 0.0f){
//        x = _x;
//        y = _y;
//        z = _z;
//		w = _w;
//    }
//
//
//	//------ Operators:
//
//  	//Negative
//    float4 operator-() const {
//        return float4( -x, -y, -z, -w );
//    }
//
//    //equality
//    bool operator==( const float4& pnt ) {
//        return (x == pnt.x) && (y == pnt.y) && (z == pnt.z) && (z == pnt.w);
//    }
//
//	//inequality
//    bool operator!=( const float4& pnt ) {
//        return (x != pnt.x) || (y != pnt.y) || (z != pnt.z) || (w != pnt.w);
//    }
//
//	//Set
//	float4 & operator=( const float4& pnt ){
//		x = pnt.x;
//		y = pnt.y;
//		z = pnt.z;
//		w = pnt.w;
//		return *this;
//	}
//
//	float4 & operator=( const float& val ){
//		x = val;
//		y = val;
//		z = val;
//		w = val;
//		return *this;
//	}
//
//	// Add
//    float4 operator+( const float4& pnt ) const {
//        return float4( x+pnt.x, y+pnt.y, z+pnt.z, w+pnt.w );
//    }
//
//    float4 operator+( const float& val ) const {
//        return float4( x+val, y+val, z+val, w+val );
//    }
//
//	float4 & operator+=( const float4& pnt ) {
//        x+=pnt.x;
//        y+=pnt.y;
//        z+=pnt.z;
//		w+=pnt.w;
//        return *this;
//    }
//
//	float4 & operator+=( const float & val ) {
//        x+=val;
//        y+=val;
//        z+=val;
//		z+=val;
//        return *this;
//    }
//
//	// Subtract
//    float4 operator-(const float4& pnt) const {
//        return float4( x-pnt.x, y-pnt.y, z-pnt.z, w-pnt.w );
//    }
//
//    float4 operator-(const float& val) const {
//        return float4( x-val, y-val, z-val, w-val);
//    }
//
//    float4 & operator-=( const float4& pnt ) {
//        x -= pnt.x;
//        y -= pnt.y;
//        z -= pnt.z;
//		w -= pnt.w;
//        return *this;
//    }
//
//    float4 & operator-=( const float & val ) {
//        x -= val;
//        y -= val;
//        z -= val;
//		w -= val;
//        return *this;
//    }
//
//	// Multiply
//    float4 operator*( const float4& pnt ) const {
//        return float4( x*pnt.x, y*pnt.y, z*pnt.z, w*pnt.w );
//    }
//
//    float4 operator*(const float& val) const {
//        return float4( x*val, y*val, z*val, w*val);
//    }
//
//    float4 & operator*=( const float4& pnt ) {
//        x*=pnt.x;
//        y*=pnt.y;
//        z*=pnt.z;
//		w*=pnt.w;
//        return *this;
//    }
//
//    float4 & operator*=( const float & val ) {
//        x*=val;
//        y*=val;
//        z*=val;
//		w*=val;
//        return *this;
//    }
//
//
//	// Divide
//    float4 operator/( const float4& pnt ) const {
//        return float4( pnt.x!=0 ? x/pnt.x : x , pnt.y!=0 ? y/pnt.y : y, pnt.z!=0 ? z/pnt.z : z, pnt.w!=0 ? w/pnt.w : w );
//    }
//
//    float4 operator/( const float &val ) const {
//		if( val != 0){
//			return float4( x/val, y/val, z/val, w/val );
//		}
//        return float4(x, y, z, w );
//    }
//
//    float4& operator/=( const float4& pnt ) {
//        pnt.x!=0 ? x/=pnt.x : x;
//        pnt.y!=0 ? y/=pnt.y : y;
//        pnt.z!=0 ? z/=pnt.z : z;
//        pnt.w!=0 ? w/=pnt.w : w;
//
//        return *this;
//    }
//
//    float4& operator/=( const float &val ) {
//		if( val != 0 ){
//			x /= val;
//			y /= val;
//			z /= val;
//			w /= val;
//		}
//
//		return *this;
//    }
//};
//
//
//
//inline float4 operator+( float f, const float4& vec ) {
//    return float4( f+vec.x, f+vec.y, f+vec.z, f+vec.w );
//}
//
//inline float4 operator-( float f, const float4& vec ) {
//    return float4( f-vec.x, f-vec.y, f-vec.z, f-vec.w );
//}
//
//inline float4 operator*( float f, const float4& vec ) {
//    return float4( f*vec.x, f*vec.y, f*vec.z, f*vec.w );
//}
//
//inline float4 operator/( float f, const float4& vec ) {
//    return float4( f/vec.x, f/vec.y, f/vec.z, f/vec.w);
//}
//
