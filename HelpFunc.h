#ifndef HELPFUNC_H
#define HELPFUNC_H

#include "stdlib.h"
#include "math.h"

#define MIN_FLOAT 0.0001f

typedef float vector3f[3];
typedef float vector4f[4];
typedef float vector2f[2];
typedef float vector2i[2];
typedef int vector3i[3];
typedef bool vector3b[3];
typedef float matrix3f[3][3];
typedef char charvalue[20];


float clamp(float obj, float min, float max);
int   clamp(int obj, int min, int max);
float f_abs(float am);
int i_abs(int am);
float DegTo360(float am);
float DegTo180(float am);
unsigned int GetFileLength(char* name);
char* LoadFileToStr(char* filename, unsigned int &len);
int MoveToTheEndOf(char* obj, char* str, unsigned int &pos);
int ReadAmountString(char* str, charvalue &amstr, unsigned int &pos);
int ReadAmount(char* str, int &amount, unsigned int &pos);
int ReadAmount(char* str, float &amount, unsigned int &pos);
int ReadString(char* str, char* data, unsigned int &pos, int cut=255);
double Rand(double Value, double vary);
int Rand(int Value, int vary);
bool BelongToArea(float x, float y, float xa, float ya, float width, float height);
void MultiplayOnMatrix(matrix3f mat, vector3f vec, vector3f &res); 

inline float f_abs(float am)
{
	return am<0.0?-am:am;	
}
inline double d_abs(double am)
{
	return am<0.0?-am:am;	
}
inline int i_abs(int am)
{
	return am<0?-am:am;	
}
inline float f_min(float v1, float v2)
{
	return v1<v2?v1:v2;
}
inline int GetRand(int &from, int &to)
{
	return from+(int)(((float)rand()/(float)RAND_MAX)*(float)(to-from)+0.5);	
}

#endif
