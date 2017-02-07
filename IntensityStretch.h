#ifndef INTENSITYSTRETCH_H
#define INTENSITYSTRETCH_H

#include "Vectors.h"
#include <QDebug>

//Расятянуть диапазон яркости img так, чтобы центральные 100-2*side_cut процента диапазона
//были отображены на значения [0;1]. После растяжения значения не обрезаются,
//таким образом, могут быть как менее нуля, так и более единицы. Рассятежение не может
//происходить более, чем в max_stretch раз
float StretchIntensity(float* img, int iw, int ih, float side_cut=5.0f, float max_stretch=2.5, float* mask=NULL);


#endif // INTENSITYSTRETCH_H
