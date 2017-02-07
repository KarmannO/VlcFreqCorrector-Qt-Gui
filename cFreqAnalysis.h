#ifndef CFREQANALYSIS_H
#define CFREQANALYSIS_H

#include "Vectors.h"
#include "HelpFunc.h"
#include "ImageLoader.h"
#include "math.h"
#include "Resizer.h"
#include "stdio.h"

#define FA_DEBUG_MODE false

class cFreqAnalysis
{
private:
    float* img; //Входное изображение
    int iw; //Ширина входного изображения
    int ih; //Высота входного изображения

    float* mask; //Маска, если используется
    int mw; //Ширина изображения-маски
    int mh; //Высота изображения-маски

    float** fr_data; //Изображения различных частот
    int* fr_w; //Ширины изображений различных частот
    int* fr_h; //Высоты изображений различных частот

    float* ibuf1; //Первый буффер для расчёта частот
    float* ibuf2; //Второй буффер для расчёта частот

    int fam; //Количество частот
    float* apl; //Относительные амплитуды частот

    int auto_size_mode; //0-off, 1-less, 2-more;
    int base_w_pot;
    int base_h_pot;

    void ReserveMemory();
    void BuildLevel(int id);
    void CalculateAmplitudes();
    void CalculateMaskedAmplitudes();
    void FreeAuxMem();
    void DebugSaveFreq();
public:
    cFreqAnalysis();
    void EnableAutoSizeLess();
    void EnableAutoSizeMore();
    void DisableAutoSize();
    void SetBaseSizePow(int width_pot, int height_pot); //Размер в степенях двойки!
    bool compute(float* data, int w, int h, float* mask_img=NULL, int mask_w=0, int mask_h=0);
    bool compute(char* filename, float* mask_img=NULL, int mask_w=0, int mask_h=0);
    float* getFreqPtr();
    float* getFreq();
    int getFAM();
    void FreeData();
    ~cFreqAnalysis();
};

#endif // CFREQANALYSIS_H
