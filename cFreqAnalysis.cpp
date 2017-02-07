#include "cFreqAnalysis.h"


cFreqAnalysis::cFreqAnalysis()
{
    img=NULL; //Входное изображение
    iw=0; //Ширина входного изображения
    ih=0; //Высота входного изображения

    fr_data=NULL; //Изображения различных частот
    fr_w=NULL; //Ширины изображений различных частот
    fr_h=NULL; //Высоты изображений различных частот

    ibuf1=NULL; //Первый буффер для расчёта частот
    ibuf2=NULL; //Второй буффер для расчёта частот

    fam=0; //Количество частот
    apl=NULL; //Относительные амплитуды частот

    auto_size_mode=0; //0-off, 1-less, 2-more;
    base_w_pot=10;
    base_h_pot=10;
}
void cFreqAnalysis::EnableAutoSizeLess()
{
    auto_size_mode=1;
}

void cFreqAnalysis::EnableAutoSizeMore()
{
    auto_size_mode=2;
}

void cFreqAnalysis::DisableAutoSize()
{
    auto_size_mode=0;
}

void cFreqAnalysis::SetBaseSizePow(int width_pot, int height_pot) //Размер в степенях двойки!
{
    if (width_pot<1) width_pot=1;
    if (height_pot<1) height_pot=1;
    if (width_pot>13) width_pot=13;
    if (height_pot>13) height_pot=13;
    base_w_pot=width_pot;
    base_h_pot=height_pot;
}

void cFreqAnalysis::ReserveMemory()
{
    FreeData();
    int xres,yres;
    //Находим ближайший размер по x:
    int mx=1;
    do
    {
        mx++;
    } while (1<<mx<=iw);
    if (auto_size_mode==1) mx--;
    else if (auto_size_mode==0) mx=base_w_pot;
    xres=1<<mx;    

    //Находим ближайший размер по
    int my=1;
    do
    {
        my++;
    } while (1<<my<=ih);
    if (auto_size_mode==1) my--;
    else if (auto_size_mode==0) my=base_h_pot;
    yres=1<<my;

    //Определяем количество частот
    fam=(mx<my?mx:my)+1;

    fr_w=new int[fam];
    fr_h=new int[fam];
    fr_w[0]=xres;
    fr_h[0]=yres;
    for (int i=1; i<fam; i++)
    {
        fr_w[i]=fr_w[i-1]/2;
        fr_h[i]=fr_h[i-1]/2;
    }

    fr_data=new float*[fam];
    for (int i=0; i<fam; i++) fr_data[i]=new float[fr_w[i]*fr_h[i]];

    apl=new float[fam];
    ibuf1=new float[xres*yres];
    ibuf2=new float[xres*yres];
}
void cFreqAnalysis::BuildLevel(int id)
{
    if (id==fam-1) //Последний уровень
    {
        memcpy(fr_data[id],ibuf1,sizeof(float)*fr_w[id]*fr_h[id]);
        return;
    }
    //Уменьшаем ibuf1 в два раза и записываем в ibuf2
    int w=fr_w[id];
    int h=fr_h[id];
    int sw=w/2;
    int sh=h/2;
    for (int i=0; i<sh; i++)
        for (int j=0; j<sw; j++)
            ibuf2[i*sw+j]=0.25f*(ibuf1[i*2*w+j*2]+ibuf1[(i*2+1)*w+j*2]+ibuf1[(i*2+1)*w+j*2+1]+ibuf1[i*2*w+j*2+1]);
    //SaveImage("debug_sizing.bmp",ibuf2,sw,sh);
    //Находим разницу пикселей и сохраняем в fr_data[id];
    float* d=fr_data[id];
    for (int i=0; i<h; i++)
        for (int j=0; j<w; j++)
            d[i*w+j]=ibuf1[i*w+j]-ibuf2[(i>>1)*sw+(j>>1)];
    //Меняем местами буфферы
    d=ibuf1;
    ibuf1=ibuf2;
    ibuf2=d;
}
void cFreqAnalysis::CalculateAmplitudes()
{
    for (int i=0; i<fam; i++)
    {
        apl[i]=0.0f;
        for (int j=0; j<fr_w[i]*fr_h[i]; j++) apl[i]+=fabs(fr_data[i][j]);
        apl[i]/=(float)(fr_w[i]*fr_h[i]);
    }
}

void cFreqAnalysis::CalculateMaskedAmplitudes()
{
    float* mask_buf=new float[fr_w[0]*fr_h[0]];
    float* mask_reduced=new float[fr_w[0]/2*fr_h[0]/2];

    if (mw!=fr_w[0] || mh!=fr_h[0]) Resizer::ScaleImage(mask,mw,mh,fr_w[0],fr_h[0],mask_buf);
    else memcpy(mask_buf,mask,mw*mh*sizeof(float));

    for (int i=0; i<fam; i++)
    {
        apl[i]=0.0f;
        float sum=0.0f;
        for (int j=0; j<fr_w[i]*fr_h[i]; j++)
        {
            apl[i]+=fabs(fr_data[i][j])*mask_buf[j];
            sum+=mask_buf[j];
        }
        apl[i]/=sum;

        if (i!=fam-1) //Уменьшаем маску в два раза
        {
            for (int y=0; y<fr_h[i+1]; y++)
                for (int x=0; x<fr_w[i+1]; x++)
                    mask_reduced[y*fr_w[i+1]+x]=0.25f*(mask_buf[(y*2)*fr_w[i]+x*2]+
                            mask_buf[(y*2)*fr_w[i]+x*2+1]+mask_buf[(y*2+1)*fr_w[i]+x*2+1]+
                            mask_buf[(y*2+1)*fr_w[i]+x*2]);
            float* temp=mask_buf;
            mask_buf=mask_reduced;
            mask_reduced=temp;

        }
    }
    delete[] mask_buf;
    delete[] mask_reduced;
}

void cFreqAnalysis::FreeAuxMem()
{
    if (fr_data) for (int i=0; i<fam; i++) delete[] fr_data[i];
    delete[] fr_data;
    fr_data=NULL;
    delete[] fr_w;
    fr_w=NULL;
    delete[] fr_h;
    fr_h=NULL;
    delete[] ibuf1;
    ibuf1=NULL;
    delete[] ibuf2;
    ibuf2=NULL;
}
bool cFreqAnalysis::compute(float* data, int w, int h, float* mask_img, int mask_w, int mask_h)
{
    if (w<2 || h<2) return false; //Слишком маленькое изображение

    mask=mask_img;
    mw=mask_w;
    mh=mask_h;

    iw=w;
    ih=h;
    img=data;
    ReserveMemory();
    if (iw!=fr_w[0] || ih!=fr_h[0]) //Нужно масштабировать изображение
        Resizer::ScaleImage(img,iw,ih,fr_w[0],fr_h[0],ibuf1);
    else memcpy(ibuf1,img,w*h*sizeof(float));

    for (int i=0; i<fam; i++) BuildLevel(i);
    if (FA_DEBUG_MODE) DebugSaveFreq();

    if (mask) CalculateMaskedAmplitudes();
    else CalculateAmplitudes();

    FreeAuxMem();

    return true;
}
bool cFreqAnalysis::compute(char* filename, float* mask_img, int mask_w, int mask_h)
{
    img=LoadImage1f(filename,iw,ih);
    if (!img) return false;
    bool res=compute(img,iw,ih,mask_img,mask_w,mask_h);
    delete[] img;
    return res;
}
float* cFreqAnalysis::getFreqPtr()
{
    return apl;
}
float* cFreqAnalysis::getFreq()
{
    if (!apl) return NULL;
    float* buf=new float[fam];
    memcpy(buf,apl,sizeof(float)*fam);
    return buf;
}
int cFreqAnalysis::getFAM()
{
    return fam;
}
void cFreqAnalysis::DebugSaveFreq()
{
    char fname[255];
    for (int i=0; i<fam; i++)
    {
        sprintf(fname,"freq_deb/img_%d.bmp",i);
        float min=1E30;
        float max=-1E30;
        for (int k=0; k<fr_w[i]*fr_h[i]; k++)
        {
            if (fr_data[i][k]<min) min=fr_data[i][k];
            if (fr_data[i][k]>max) max=fr_data[i][k];
        }
        for (int k=0; k<fr_w[i]*fr_h[i]; k++)
            ibuf1[k]=(fr_data[i][k]-min)/(max-min);
            //ibuf1[k]=(fr_data[i][k]-min);
        SaveImage(fname,ibuf1,fr_w[i],fr_h[i]);
    }
    memset(ibuf2,0,sizeof(float)*fr_w[0]*fr_h[0]);
    int x,y,id;
    for (int i=0; i<fr_h[0]; i++)
    {
        for (int j=0; j<fr_w[0]; j++)
        {
            id=i*fr_w[0]+j;
            x=j;
            y=i;
            for (int k=0; k<fam; k++)
            {
                ibuf2[id]+=fr_data[k][y*fr_w[k]+x];
                x/=2;
                y/=2;
            }
        }
    }
    SaveImage("freq_deb/summa.bmp",ibuf2,fr_w[0],fr_h[0]);
}

void cFreqAnalysis::FreeData()
{
    FreeAuxMem();
    delete[] apl;
    apl=NULL;
    fam=0;
}
cFreqAnalysis::~cFreqAnalysis()
{
    FreeData();
}
