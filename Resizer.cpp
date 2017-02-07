#include "Resizer.h"

namespace Resizer
{

void ReduceImage(float* img, int w, int h, int &nw, int &nh, float* res, float* buff)
{
    float* buf;
    if (buff) buf=buff;
    else buf=new float[nw*h];

    float* line;
    float l;
    float r;  
    float k=(float)w/(float)nw;
    float cc;
    float value;	
    for (int i=0; i<h; i++)
    {
        line=&img[i*w];
        for (int j=0; j<nw; j++)
        {
            cc=(j+0.5f)*k; //Центр точки на исходном изображении
            l=cc-0.5f*k; //Левый край интервала
            r=cc+0.5f*k; //Правый край интервала
            value=0.0f;
		
            for (int p=(int)l; p<(int)(r+1.0f); p++)
            {
                    if (p>w-1) break;
                    if (p==(int)l) value+=line[p]*(1.0f-(l-(int)l));                                                                
                    else if (p==(int)(r+1.0)-1) value+=line[p]*(r-(int)r);
                    else value+=line[p];
            }
            buf[i*nw+j]=value/k;                    
        }
    }

    float t;
    float b;    
    k=(float)h/(float)nh;    
    for (int j=0; j<nw; j++)
    {
        for (int i=0; i<nh; i++)
        {
            cc=(i+0.5f)*k; //Центр точки на исходном изображении
            t=cc-0.5f*k; //Левый край интервала
            b=cc+0.5f*k; //Правый край интервала
            value=0.0f;
            for (int p=(int)t; p<(int)(b+1.0f); p++)
            {
                if (p>h-1) break;
                if (p==(int)t) value+=buf[p*nw+j]*(1.0f-(t-(int)t));
                else if (p==(int)(b+1.0)-1) value+=buf[p*nw+j]*(b-(int)b);                        
                else value+=buf[p*nw+j];                                   
            }
            res[i*nw+j]=value/k;          
        }
    }
    if (!buff) delete[] buf;
}
void IncreaseImage(float* img, int w, int h, int &nw, int &nh, float* &res, float* img_buf)
{
    float* ib;
    if (img_buf) ib=img_buf;
    else ib=new float[nw*h];

    float ws=(float)w/(float)nw;
    float hs=(float)h/(float)nh;

    float rx;
    int r_id;
    int n_id;
    int nid;
    float fk,ik;
    //Масштабируем по x
    for (int i=0; i<h; i++)
    {
        for (int j=0; j<nw; j++)
        {
            rx=(j+0.5f)*ws;
            r_id=(int)rx;
            nid=r_id+0.5>rx?-1:1;
            n_id=r_id+nid;
            if (n_id<0 || n_id>w-1) n_id=r_id;
            ik=f_abs(r_id+0.5f-rx);
            fk=1.0f-ik;
            ib[i*nw+j]=img[i*w+r_id]*fk+img[i*w+n_id]*ik;
        }
    }
    //Масштабируем по y
    for (int j=0; j<nw; j++)
    {
        for (int i=0; i<nh; i++)
        {
            rx=(i+0.5f)*hs;
            r_id=(int)rx;
            nid=r_id+0.5>rx?-1:1;
            n_id=r_id+nid;
            if (n_id<0 || n_id>h-1) n_id=r_id;
            ik=f_abs(r_id+0.5f-rx);
            fk=1.0f-ik;
            res[i*nw+j]=ib[r_id*nw+j]*fk+ib[n_id*nw+j]*ik;
        }
    }
    if (!img_buf) delete[] ib;
}

void ScaleImage(float* img, int w, int h, int &nw, int &nh, float* res)
{
    float* ib=new float[nw*h];
    if (w<nw) //Необходимо увеличивать изображение по x
    {
        float ws=(float)w/(float)nw;
        float rx;
        int r_id;
        int n_id;
        int nid;
        float fk,ik;
        //Масштабируем по x
        for (int i=0; i<h; i++)
        {
            for (int j=0; j<nw; j++)
            {
                rx=(j+0.5f)*ws;
                r_id=(int)rx;
                nid=r_id+0.5>rx?-1:1;
                n_id=r_id+nid;
                if (n_id<0 || n_id>w-1) n_id=r_id;
                ik=f_abs(r_id+0.5f-rx);
                fk=1.0f-ik;
                ib[i*nw+j]=img[i*w+r_id]*fk+img[i*w+n_id]*ik;
            }
        }
    }
    else if (w>nw) //Необходимо уменьшать изображение по x
    {
        float* line;
        float l;
        float r;
        float k=(float)w/(float)nw;
        float cc;
        float value;
        for (int i=0; i<h; i++)
        {
            line=&img[i*w];
            for (int j=0; j<nw; j++)
            {
                cc=(j+0.5f)*k; //Центр точки на исходном изображении
                l=cc-0.5f*k; //Левый край интервала
                r=cc+0.5f*k; //Правый край интервала
                value=0.0f;

                for (int p=(int)l; p<(int)(r+1.0f); p++)
                {
                        if (p>w-1) break;
                        if (p==(int)l) value+=line[p]*(1.0f-(l-(int)l));
                        else if (p==(int)(r+1.0)-1) value+=line[p]*(r-(int)r);
                        else value+=line[p];
                }
                ib[i*nw+j]=value/k;
            }
        }
    }
    else memcpy(ib,img,sizeof(float)*w*h); //Не нужно ничего делать по ширине

    if (h<nh) //Необходимо увеличить
    {
        float hs=(float)h/(float)nh;
        float rx;
        int r_id;
        int n_id;
        int nid;
        float fk,ik;

        for (int j=0; j<nw; j++)
        {
            for (int i=0; i<nh; i++)
            {
                rx=(i+0.5f)*hs;
                r_id=(int)rx;
                nid=r_id+0.5>rx?-1:1;
                n_id=r_id+nid;
                if (n_id<0 || n_id>h-1) n_id=r_id;
                ik=f_abs(r_id+0.5f-rx);
                fk=1.0f-ik;
                res[i*nw+j]=ib[r_id*nw+j]*fk+ib[n_id*nw+j]*ik;
            }
        }
    }
    else if (h>nh) //Необходимо уменьшить
    {
        float cc;
        float value;

        float t;
        float b;
        float k=(float)h/(float)nh;
        for (int j=0; j<nw; j++)
        {
            for (int i=0; i<nh; i++)
            {
                cc=(i+0.5f)*k; //Центр точки на исходном изображении
                t=cc-0.5f*k; //Левый край интервала
                b=cc+0.5f*k; //Правый край интервала
                value=0.0f;
                for (int p=(int)t; p<(int)(b+1.0f); p++)
                {
                    if (p>h-1) break;
                    if (p==(int)t) value+=ib[p*nw+j]*(1.0f-(t-(int)t));
                    else if (p==(int)(b+1.0)-1) value+=ib[p*nw+j]*(b-(int)b);
                    else value+=ib[p*nw+j];
                }
                res[i*nw+j]=value/k;
            }
        }
    }
    else memcpy(res,ib,sizeof(float)*nw*h); //Не нужно ничего делать по высоте
    delete[] ib;
}

}
