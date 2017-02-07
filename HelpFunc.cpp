#include "HelpFunc.h"
#include "stdio.h"
#include "stdlib.h"

//Функция отсечения
float clamp(float obj, float min, float max)
{
	if (obj<min) obj=min;
	if (obj>max) obj=max;
	return obj;
}
int clamp(int obj, int min, int max)
{
	if (obj<min) obj=min;
	if (obj>max) obj=max;
	return obj;
}
//Функция отсекает градусы по диапазону от 0 до 360
float DegTo360(float am)
{
	int FullTurns;
	FullTurns=(int)(am/360.0f);
	am=am-FullTurns*360.0f;
	if (am>=0) return  am;
	return 360.0f+am;
}
//Функция отсекает градусы по диапазону от -180 до 180
float DegTo180(float am)
{
	am=DegTo360(am);
	if (am<=180.0f) return am;
	return am-360.0f;
}
//Функция определения размера файла
unsigned int GetFileLength(char* name)
{
	FILE* f=fopen(name,"rb");
	if (!f) return 0;
	fseek(f,0,SEEK_END);
	int size=ftell(f);
	fclose(f);
	return size;
}
//Функции разбора строк:
//Функция для поиска подстроки в строке
int MoveToTheEndOf(char* obj, char* str, unsigned int &pos)
{
	int i=pos;
	int ObjLen;
	for (ObjLen=0;obj[ObjLen]!=0;ObjLen++);
	bool flag;
	while (str[i]!=0)
	{
		flag=true;
		for (int j=i; j<i+ObjLen; j++)
		{
			if (str[j]!=obj[j-i])
			{
				flag=false;
				break;
			}
		}
		if (flag) 
		{
			pos=i+ObjLen;
			return 0;
		}
		i++;
	}
	pos=i;
	return 1;
}
//Функция, считывающая строку, содержащую нужное число
int ReadAmountString(char* str, charvalue &amstr, unsigned int &pos)
{
	int i=pos;
	amstr[0]=0;
	while (str[i]!='0'&& str[i]!='1'&& str[i]!='2'&& str[i]!='3'&& str[i]!='4'&&
		   str[i]!='5'&& str[i]!='6'&& str[i]!='7'&& str[i]!='8'&& str[i]!='9'&&
		   str[i]!='-'&& str[i]!=0){i++;}
	if (str[i]==0) {pos=i; return 1;}
	int j=0;
	while (str[i]=='0'|| str[i]=='1'|| str[i]=='2'|| str[i]=='3'|| str[i]=='4'||
		   str[i]=='5'|| str[i]=='6'|| str[i]=='7'|| str[i]=='8'|| str[i]=='9'||
		   str[i]=='-'|| str[i]=='.')
	{
		amstr[j]=str[i];
		j++;
		i++;
	}
	amstr[j]=0;
	pos=i;
    return 0;
}
//Функция для считывания целого числа
int ReadAmount(char* str, int &amount, unsigned int &pos)
{
	charvalue amstr;
	if (ReadAmountString(str,amstr,pos)) return 1;
	amount=atoi(amstr);
	return 0;
}
//Функция для считывания дробного числа
int ReadAmount(char* str, float &amount, unsigned int &pos)
{
	charvalue amstr;
	if (ReadAmountString(str,amstr,pos)) return 1;
	amount=(float)atof(amstr);
	return 0;
}
int ReadString(char* str, char* data, unsigned int &pos, int cut)
{
	while (str[pos]!='\"' && str[pos]!=0)
	{
		pos++;
	}
	if (str[pos]==0) return 1;
	pos++;
	int id=0;
	while (str[pos]!='\"' && str[pos]!=0 && id<cut-1)
	{
		data[id]=str[pos];
		pos++;
		id++;
	}
	pos++;
	data[id]=0;
	if (str[pos]==0) return 1;
	return 0;
	
}
//Функция чтения файла в строку
char* LoadFileToStr(char* filename, unsigned int &len)
{
	char* str;
    len=GetFileLength(filename);
    FILE* f;
	f=fopen(filename,"rb");
	if (f==NULL) return NULL;
    str=new char[len+1];
	fread(str,len,1,f);
	str[len]=0;
	fclose(f);
	return str;
}
//Функция нахождения случайной величины
double Rand(double Value, double vary)
{
	double DV=(((2.0*rand())/RAND_MAX)-1.0)*vary;
	return Value+DV;
}
int Rand(int Value, int vary)
{
	double DV=(((2.0*rand())/RAND_MAX)-1.0)*vary;
	int IDV;
	if (DV<0.0) IDV=(int)(DV-0.5);
	else IDV=(int)(DV+0.5);
	return Value+IDV;
}
//Проверяет принадлежность координат прямоугольной области
bool BelongToArea(float x, float y, float xa, float ya, float width, float height)
{
    if (x>xa && x<xa+width && y>ya && y<ya+height) return true;
	return false;
}
//Перемножение на матрицу
void MultiplayOnMatrix(matrix3f mat, vector3f vec, vector3f &res)
{
	for (int i=0; i<3; i++)
	{
		res[i]=0;
		for (int j=0; j<3; j++)
		{
			res[i]+=mat[i][j]*vec[j];
		}
	}
}
