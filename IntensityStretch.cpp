#include "IntensityStretch.h"


float StretchIntensity(float* img, int iw, int ih, float side_cut, float max_stretch, float *mask)
{
    if (side_cut>25.0) side_cut=25.0; //Не отрезаем более 50% от диапазона

    const int N=500;

    int bin[N];
    memset(bin,0,sizeof(int)*N);
    float min_v=1E30f;
    float max_v=-1E30f;

    int pix_n=0;
    for (int i=0; i<ih*iw; i++)
    {
        if (mask && mask[i]<0.5f) continue;
        if (img[i]<min_v) min_v=img[i];
        if (img[i]>max_v) max_v=img[i];
        pix_n++;
    }

    float from=min_v-0.0001f; //Начало диапазона интенсивностей
    float to=max_v+0.0001f; //Конец диапазона интенсивностей
    float sf=(float)N/(to-from);

    //Заполняем гистограмму
    for (int i=0; i<ih*iw; i++)
    {
        if (mask && mask[i]<0.5f) continue;
        float fb=(img[i]-min_v)*sf;
        bin[(int)fb]++;
    }
    float cut_n=(float)pix_n*((float)side_cut/100.0f);

    //Ищем начало диапазона обрезки
    float total=0;
    float left=max_v;
    for (int i=0; i<N; i++)
    {
        total+=(float)bin[i];
        if (total>=cut_n)
        {
            float l=(float)(i)/sf+from;
            float r=l+1/sf;
            float k=(total-cut_n)/(float)bin[i];
            left=l*k+r*(1.0f-k);
            break;
        }
    }
    //Ищем конец диапазона обрезки
    total=0;
    float right=min_v;
    for (int i=N-1; i>=0; i--)
    {
        total+=(float)bin[i];
        if (total>=cut_n)
        {
            float l=(float)(i)/sf+from;
            float r=l+1/sf;
            float k=(total-cut_n)/(float)bin[i];
            right=r*k+l*(1.0f-k);
            break;
        }
    }
    float critical_len=1.0/max_stretch;
    float scale=right-left>critical_len?1.0/(right-left):max_stretch;

    qDebug("Stretch: d[%f %f], cut[%f %f], scale=%f",min_v,max_v,left,right,scale);

    float center=(right-left)*0.5f; //Центр текущего диапазона

    //Растягиваем диапазон
    for (int i=0; i<iw*ih; i++)
        img[i]=(img[i]-center)*scale+0.5f;


    return scale;
}
