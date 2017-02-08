#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cFreqAnalysis.h"
#include "HelpFunc.h"
#include "Vectors.h"
#include "ImageLoader.h"
#include "IntensityStretch.h"
#include <QFileDialog>
#include <QVector>

namespace Ui {
class MainWindow;
}

struct image_data
{
    char img_name[256];
    char f_name[256];
    int fr_n;
    float* f;
    float mark;
    int ideal_fr_n;
    float *ideal_f;
    char ideal_name[256];

    image_data()
    {
        img_name[0]=0;
        f_name[0]=0;
        ideal_name[0] = 0;
        ideal_fr_n = 0;
        fr_n=0;
        f=NULL;
        ideal_f = NULL;
    }    
    ~image_data()
    {
        delete[] f;
        delete[] ideal_f;
    }
};

struct output_data
{
    QString vlc;
    float w0, w1, w2;
    float b;
    float err;
};

//Вместо масштбаного фактора используются раздельные веса для высоких, средних и низких частот
struct weights_data
{
    vec3 ww[13]; //Веса весов
    vec3 w;  //Настраиваемые веса
    weights_data()
    {
        w[0]=1.0;
        w[1]=1.0;
        w[2]=1.0;        
        SetWW();
    }
    weights_data(float w0, float w1, float w2)
    {        
        w[0]=w0;
        w[1]=w1;
        w[2]=w2;
        SetWW();
    }
    void SetWW()
    {
        ww[0][0]=1.0;   ww[0][1]=0.0;   ww[0][2]=0.0;
        ww[1][0]=1.0;   ww[1][1]=0.0;   ww[1][2]=0.0;
        ww[2][0]=0.66;  ww[2][1]=0.34;  ww[2][2]=0.0;
        ww[3][0]=0.34;  ww[3][1]=0.66;  ww[3][2]=0.0;
        ww[4][0]=0.0;   ww[4][1]=1.0;   ww[4][2]=0.0;
        ww[5][0]=0.0;   ww[5][1]=1.0;   ww[5][2]=0.0;
        ww[6][0]=0.0;   ww[6][1]=0.66;  ww[6][2]=0.34;
        ww[7][0]=0.0;   ww[7][1]=0.34;  ww[7][2]=0.66;
        ww[8][0]=0.0;   ww[8][1]=0.0;   ww[8][2]=1.0;
        ww[9][0]=0.0;   ww[9][1]=0.0;   ww[9][2]=1.0;
        ww[10][0]=0.0;  ww[10][1]=0.0;  ww[10][2]=1.0;
        ww[11][0]=0.0;  ww[11][1]=0.0;  ww[11][2]=1.0;
        ww[12][0]=0.0;  ww[12][1]=0.0;  ww[12][2]=1.0;
    }

    void set_w(float w0, float w1, float w2)
    {
        w[0]=w0;
        w[1]=w1;
        w[2]=w2;
    }   
    float get_w(int id)
    {
        return w[0]*ww[id][0]+w[1]*ww[id][1]+w[2]*ww[id][2];
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_folder_button_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    char* folder_name;
    int img_n;
    image_data *test_img;
    image_data ideal_img;
    vec2 *expert_marks; //Диапазоны оценок эксперта

    QVector<output_data> ouput;
    QVector<QString> folders;

    float* mask;
    int mw;
    int mh;

    float max_scale; //Максимальное растяжение контраста
    bool use_range; //Использовать диапазон оценок    


    void ComputeIdeals(QString folder);
    void ComputeNames();
    void ComputeFreq();
    void ReadVectors();
    void ReadExpertMarks();
    void ReadExpertMarksRange();
    void ParseDirNameIntoRange(QString name, vec2 &res);

    int CountImagesInFolder(char *name, bool recursive);

    QVector<QString> GetFoldersInFolder(QString fldr);
    QVector<QString> GetImagesInFolder(QString fldr);

    void CalcErrors();
    float ComputeMarks(float b0, float b1, float w00, float w01, float w10, float w11, float w20, float w21,
                      int bn, int w0n, int w1n, int w2n, float& best_bias, weights_data& best_W);
    void AssignMarks(float bias, weights_data& W);
    float ComputeMarksError();
    void PrintMarkInfo(float bias, weights_data& W);
    static void ReadF(char* filename, int &fn, float*&f);
    void RemoveFile(QString name);

    void SearchMinimum(float b0, float b1, float w00, float w01, float w10, float w11,
                       float w20,float w21);
    float SearchFromPoint(float &bias, weights_data& W, float biasR, float wR);
};

#endif // MAINWINDOW_H
