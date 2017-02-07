#ifndef RESIZER_H
#define RESIZER_H

#include "Vectors.h"
#include "ImageLoader.h"
#include "HelpFunc.h"

namespace Resizer
{
    void ReduceImage(float* img, int w, int h, int &nw, int &nh, float* res, float* buff=NULL);
    void IncreaseImage(float* img, int w, int h, int &nw, int &nh, float* &res, float* img_buf=NULL);    
    void ScaleImage(float* img, int w, int h, int &nw, int &nh, float* res);
    template<class CT>
    void ScaleImage(CT (*img)[3], int w, int h, int &nw, int &nh, CT (*res)[3])
    {
        typedef CT pix_type[3];
        pix_type* ib=new pix_type[nw*h];
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
                    ib[i*nw+j][0]=(CT)((float)img[i*w+r_id][0]*fk+(float)img[i*w+n_id][0]*ik);
                    ib[i*nw+j][1]=(CT)((float)img[i*w+r_id][1]*fk+(float)img[i*w+n_id][1]*ik);
                    ib[i*nw+j][2]=(CT)((float)img[i*w+r_id][2]*fk+(float)img[i*w+n_id][2]*ik);
                }
            }
        }
        else if (w>nw) //Необходимо уменьшать изображение по x
        {
            vector3ub* line;
            float l;
            float r;
            float k=(float)w/(float)nw;
            float cc;
            vec3 value;
            float kf;

            for (int i=0; i<h; i++)
            {
                line=&img[i*w];
                for (int j=0; j<nw; j++)
                {
                    cc=(j+0.5f)*k; //Центр точки на исходном изображении
                    l=cc-0.5f*k; //Левый край интервала
                    r=cc+0.5f*k; //Правый край интервала
                    value[0]=0.0f;
                    value[1]=0.0f;
                    value[2]=0.0f;

                    for (int p=(int)l; p<(int)(r+1.0f); p++)
                    {
                            if (p>w-1) break;
                            if (p==(int)l)
                            {
                                kf=1.0f-(l-(int)l);
                                value[0]+=(float)line[p][0]*kf;
                                value[1]+=(float)line[p][1]*kf;
                                value[2]+=(float)line[p][2]*kf;
                            }
                            else if (p==(int)(r+1.0)-1)
                            {
                                kf=r-(int)r;
                                value[0]+=(float)line[p][0]*kf;
                                value[1]+=(float)line[p][1]*kf;
                                value[2]+=(float)line[p][2]*kf;
                            }
                            else
                            {
                                value[0]+=(float)line[p][0];
                                value[1]+=(float)line[p][1];
                                value[2]+=(float)line[p][2];
                            }
                    }
                    ib[i*nw+j][0]=(CT)(value[0]/k);
                    ib[i*nw+j][1]=(CT)(value[1]/k);
                    ib[i*nw+j][2]=(CT)(value[2]/k);
                }
            }
        }
        else memcpy(ib,img,sizeof(pix_type)*w*h); //Не нужно ничего делать по ширине

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
                    res[i*nw+j][0]=(CT)((float)ib[r_id*nw+j][0]*fk+(float)ib[n_id*nw+j][0]*ik);
                    res[i*nw+j][1]=(CT)((float)ib[r_id*nw+j][1]*fk+(float)ib[n_id*nw+j][1]*ik);
                    res[i*nw+j][2]=(CT)((float)ib[r_id*nw+j][2]*fk+(float)ib[n_id*nw+j][2]*ik);
                }
            }
        }
        else if (h>nh) //Необходимо уменьшить
        {
            float k=(float)h/(float)nh;
            float cc;
            vec3 value;
            float kf;
            float t;
            float b;

            for (int j=0; j<nw; j++)
            {
                for (int i=0; i<nh; i++)
                {
                    cc=(i+0.5f)*k; //Центр точки на исходном изображении
                    t=cc-0.5f*k; //Левый край интервала
                    b=cc+0.5f*k; //Правый край интервала
                    value[0]=0.0f;
                    value[1]=0.0f;
                    value[2]=0.0f;
                    for (int p=(int)t; p<(int)(b+1.0f); p++)
                    {
                        if (p>h-1) break;
                        if (p==(int)t)
                        {
                            kf=1.0f-(t-(int)t);
                            value[0]+=(float)ib[p*nw+j][0]*kf;
                            value[1]+=(float)ib[p*nw+j][1]*kf;
                            value[2]+=(float)ib[p*nw+j][2]*kf;
                        }
                        else if (p==(int)(b+1.0)-1)
                        {
                            kf=b-(int)b;
                            value[0]+=(float)ib[p*nw+j][0]*kf;
                            value[1]+=(float)ib[p*nw+j][1]*kf;
                            value[2]+=(float)ib[p*nw+j][2]*kf;
                        }
                        else
                        {
                            value[0]+=(float)ib[p*nw+j][0];
                            value[1]+=(float)ib[p*nw+j][1];
                            value[2]+=(float)ib[p*nw+j][2];
                        }
                    }
                    res[i*nw+j][0]=(CT)(value[0]/k);
                    res[i*nw+j][1]=(CT)(value[1]/k);
                    res[i*nw+j][2]=(CT)(value[2]/k);
                }
            }
        }
        else memcpy(res,ib,sizeof(pix_type)*nw*h); //Не нужно ничего делать по высоте
        delete[] ib;
    }
}

#endif
