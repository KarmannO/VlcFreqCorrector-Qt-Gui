#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    folder_name = NULL;
    max_scale=2.5f;
    use_range=true;

    mask=LoadImage1f("KLU_1/Ideal/mask.bmp",mw,mh);

    ui->setupUi(this);
}

void MainWindow::ComputeNames()
{
    sprintf(ideal_img.f_name,"KLU_1/_KLU1_ideal.txt");

    for (int i=0; i<img_n; i++)
    {
        sprintf(test_img[i].img_name,"KLU_1/volcanoes/KLU1_%d.jpg",i+1);
        sprintf(test_img[i].f_name,"KLU_1/volcanoes/_KLU1_%d.jpg_freq.txt",i+1);
    }
}

void MainWindow::ComputeIdeals()
{
    const int N=5;
    cFreqAnalysis fa;
    FILE* f=fopen("KLU_1/ideal/log.txt","wt");

    char fname[256];

    float f_no_mask[N][14];
    float f_mask[N][14];
    int M[N];

    for (int i=0; i<N; i++)
    {
        sprintf(fname,"KLU_1/ideal/%02d.jpg",i);

        int iw,ih;
        float* img=LoadImage1f(fname,iw,ih);        
        StretchIntensity(img,iw,ih,2.0,max_scale,mask); //Предварительно расстягиваем диапазон интенсивностей!

        fa.compute(img,iw,ih); //Считаем без маски (для сравнения)
        M[i]=fa.getFAM()-1;
        memcpy(f_no_mask[i],fa.getFreqPtr(),sizeof(float)*M[i]);

        fa.compute(img,iw,ih,mask,mw,mh); //Считаем с маской (именно этот результат используется)
        delete[] img;
        memcpy(f_mask[i],fa.getFreqPtr(),sizeof(float)*M[i]);
    }

    for (int i=0; i<N; i++)
    {
        fprintf(f,"Image %02d:  ",i);
        for (int j=0; j<M[i]; j++)
        {
            fprintf(f,"%f(%f)   ",f_no_mask[i][j],f_mask[i][j]);
        }
        fprintf(f,"\n");
    }

    float CF_nm[14]; //Усреднённая характеристика без маски
    float CF_m[14]; //С маской

    for (int i=0; i<14; i++)
    {
        CF_nm[i]=0.0;
        CF_m[i]=0.0;
    }

    for (int i=0; i<N; i++)
    {
        for (int j=0; j<M[i]; j++)
        {
            CF_nm[j]+=f_no_mask[i][j];
            CF_m[j]+=f_mask[i][j];
        }
    }
    for (int i=0; i<14; i++)
    {
        CF_nm[i]/=(float)N;
        CF_m[i]/=(float)N;
    }

    fprintf(f,"Image mx:  ");
    for (int j=0; j<M[0]; j++)
    {
        fprintf(f,"%f(%f)   ",CF_nm[j],CF_m[j]);
    }
    fprintf(f,"\n");

    fclose(f);


    f=fopen("KLU_1/_KLU1_ideal.txt","wt");
    for (int i=0; i<M[0]; i++) fprintf(f,"%f\n",CF_m[i]);
    fclose(f);
}

//Считывает из текстового файла все записанные числа
void MainWindow::ReadF(char* filename, int &fn, float*&f)
{
    unsigned int str_len;
    char* str=LoadFileToStr(filename,str_len);
    unsigned int cur_pos=0;

    f=new float[14];
    fn=0;

    while (!ReadAmount(str,f[fn],cur_pos))
    {
        fn++;
    }

    delete[] str;
}

//Считывает характеристики изображений
void MainWindow::ReadVectors()
{
    ReadF(ideal_img.f_name,ideal_img.fr_n,ideal_img.f);
    for (int i=0; i<img_n; i++)    
        ReadF(test_img[i].f_name,test_img[i].fr_n,test_img[i].f);
}

//Читает оценки эксперта
void MainWindow::ReadExpertMarks()
{
    unsigned int str_len;
    char* str=LoadFileToStr("KLU_1/KLU1_manual.txt",str_len);
    unsigned int cur_pos=0;
    for (int i=0; i<img_n; i++)
    {
        ReadAmount(str,expert_marks[i][0],cur_pos);
        expert_marks[i][1]=expert_marks[i][0];
    }
    delete[] str;  
}

//Читает диапазон оценок
void MainWindow::ReadExpertMarksRange()
{
    unsigned int str_len;
    char* str=LoadFileToStr("KLU_1/KLU1_manual_range.txt",str_len);
    unsigned int cur_pos=0;
    for (int i=0; i<img_n; i++)
    {
        ReadAmount(str,expert_marks[i][0],cur_pos);
        ReadAmount(str,expert_marks[i][1],cur_pos);
    }
    delete[] str;
}

int MainWindow::CountImagesInFolder(char *name, bool recursive, QStringList & files)
{
    if(!recursive)
    {
        QDir d(name);
        QStringList t;
        t.append("*.png");
        t.append("*.jpg");
        t.append("*.jpeg");
        QStringList files_list = d.entryList(t, QDir::Files);
        return files_list.length();
    }
    else
    {
        QDir d(name);
        QStringList dirs_list = d.entryList(QDir::Dirs);
        if(dirs_list.length() <= 2)
            return 0;
        else
        {
            int total = 0;
            QString root = QString(name);
            for(int i = 2; i < dirs_list.length(); i++)
            {
                QString fold = root + "\\" + dirs_list.at(i);
                char *new_fold = new char[fold.length() + 1];
                new_fold[fold.length()] = '\0';
                strcpy(new_fold, fold.toStdString().c_str());
                QStringList p;
                total += CountImagesInFolder(new_fold, false, p);
            }
            return total;
        }
    }
}

//Вычисляет характеристики изображений
void MainWindow::ComputeFreq()
{
    cFreqAnalysis fa;
    //fa.EnableAutoSizeMore();
    for (int i=0; i<img_n; i++)
    {
        int iw,ih;
        float* img=LoadImage1f(test_img[i].img_name,iw,ih);
        StretchIntensity(img,iw,ih,2.0,max_scale,mask); //Предварительно растягивем контраст

        fa.compute(img,iw,ih,mask,mw,mh);
        int fn=fa.getFAM()-1;
        float* v=fa.getFreqPtr();

        delete[] img;

        FILE* f=fopen(test_img[i].f_name,"wt");
        for (int i=0; i<fn; i++) fprintf(f,"%f\n",v[i]);
        fclose(f);
    }
}

//Сравниваем характеристики всех изображений с эталонной на основе заданных смещений и весов
void MainWindow::AssignMarks(float bias, weights_data& W)
{
    float* &b=ideal_img.f;
    for (int i=0; i<img_n; i++)
    {
        int &n=test_img[i].fr_n;
        float* &a=test_img[i].f;

        float mark=0.0;        

        for (int j=0; j<n; j++)
            mark+=W.get_w(j)*fabs(b[j]-a[j]);

        test_img[i].mark=bias-mark;
        if (test_img[i].mark>1.0) test_img[i].mark=1.0;
        if (test_img[i].mark<0.0) test_img[i].mark=0.0;
    }
}
//Вычисляем общую ошибку по всем изображениям
float MainWindow::ComputeMarksError()
{
    float er=0.0;
    float dif;
    float max_dif=0.0;
    for (int i=0; i<img_n; i++)
    {
        float& m=test_img[i].mark;
        vec2 &em=expert_marks[i];

        dif=(m>=em[0] && m<=em[1])?0.0:(m<em[0]?em[0]-m:m-em[1]);
        if (dif>max_dif) max_dif=dif;        
        er+=dif*dif;
    }
    return er;
    //return max_dif;
}

//Считаем минимальную ошибку на сетке размера bn*w0n*w1n*w2n в заданном диапазоне
float MainWindow::ComputeMarks(float b0, float b1, float w00, float w01, float w10, float w11, float w20, float w21,
                  int bn, int w0n, int w1n, int w2n, float& best_bias, weights_data& best_W)
{
    weights_data W;
    float bias;
    float min_er=1E30f;

    for (int bi=0; bi<bn; bi++)
    {
        bias=((float)bi+0.5f)/(float)bn*(b1-b0)+b0;
        for (int w0i=0; w0i<w0n; w0i++)
        {
            W.w[0]=((float)w0i+0.5f)/(float)w0n*(w01-w00)+w00;
            for (int w1i=0; w1i<w1n; w1i++)
            {
                W.w[1]=((float)w1i+0.5f)/(float)w1n*(w11-w10)+w10;
                for (int w2i=0; w2i<w2n; w2i++)
                {
                    W.w[2]=((float)w2i+0.5f)/(float)w2n*(w21-w20)+w20;

                    AssignMarks(bias,W);
                    float er=ComputeMarksError();
                    if (er<min_er)
                    {
                        min_er=er;
                        best_bias=bias;
                        best_W=W;
                    }
                }
            }
        }
    }
    return min_er;
}

//Выводим информацию об ошибках по каждому изображению в текстовый файл
void MainWindow::PrintMarkInfo(float bias, weights_data& W)
{
    AssignMarks(bias,W);
    FILE* f=fopen("KLU_1/auto_marks.txt","wt");

    float sum=0.0;
    for (int i=0; i<img_n; i++)
    {
        float& m=test_img[i].mark;
        vec2 &em=expert_marks[i];
        float dif=(m>=em[0] && m<=em[1])?0.0:(m<em[0]?em[0]-m:m-em[1]);
        sum+=dif;
        fprintf(f,"Expert: [%f; %f], auto: %f, er=%f\n",em[0],em[1],m,dif);
    }
    sum/=(float)img_n;
    fprintf(f,"Mean error: %f",sum);
    fclose(f);
}

//Поиск лучших значений смещения и весов
void MainWindow::SearchMinimum(float b0, float b1, float w00, float w01, float w10, float w11,
                   float w20,float w21)
{
    const int n=10; //Столько случайных точек используется для начала поиска
    //Чтобы исключить схождение к локальному минимуму, проводим n поисков из разных точек диапазона

    weights_data W;
    float bias;

    float best_bias;
    weights_data best_W;
    float min_er=1E30f;


    float biasR=1.5;
    float wR=20.0f;

    for (int i=0; i<n; i++)
    {
        //Определяем i-ю случайную начальную точку
        bias=(b1-b0)*(float)rand()/(float)RAND_MAX+b0;
        W.w[0]=(w01-w00)*(float)rand()/(float)RAND_MAX+w00;
        W.w[1]=(w11-w10)*(float)rand()/(float)RAND_MAX+w10;
        W.w[2]=(w21-w20)*(float)rand()/(float)RAND_MAX+w20;

        qDebug("%d/%d point (%f %f %f %f)",i+1,n,bias,W.w[0],W.w[1],W.w[2]);

        //Ищем минимум, начиная с найденной точки
        float er=SearchFromPoint(bias,W,biasR,wR);

        qDebug("    reached (%f %f %f %f), er=%f",bias,W.w[0],W.w[1],W.w[2],er);

        if (er<min_er)
        {
            best_bias=bias;
            best_W=W;
            min_er=er;
        }
    }
    qDebug("Min error: %f",min_er);
    qDebug("bias=%f, W=(%f %f %f)",best_bias,best_W.w[0],best_W.w[1],best_W.w[2]);
    PrintMarkInfo(best_bias,W);
}

//Рекурсивная функция поиска
float MainWindow::SearchFromPoint(float &bias, weights_data& W, float biasR, float wR)
{
    const int bn=10; //Количество узлов сетки по смещению
    const int wn=10; //Количество узлов сетки по каждому весу
    float bias_bondary=biasR*2.0f/(float)bn; //Толщина крайней полосы диапазона смещений
    float w_bondary=wR*2.0f/(float)bn; //Толщина крайней полосы диапазона весов

    float last_er=1E30f;
    bool not_stop;
    //Итеративно смещаем окно поиска в сторону минимума, пока не найдем его
    do
    {
        //Сохраняем центр, чтобы потом проверить корректность экстремума
        float bc=bias;
        float w0c=W.w[0];
        float w1c=W.w[1];
        float w2c=W.w[2];

        float prev_er=last_er;
        last_er=ComputeMarks(bias-biasR,bias+biasR,W.w[0]-wR,W.w[0]+wR,W.w[1]-wR,W.w[1]+wR,
                W.w[2]-wR,W.w[2]+wR,bn,wn,wn,wn,bias,W);
        if (prev_er-last_er<0.00001f) break; //Идём в неверном направлении.

        //Если найденный минимум попал на краевые полосы диапазона - необходимо продолжить поиск,
        //в другом диапазоне
        not_stop=(bias<bc-biasR+bias_bondary || bias>bc+biasR-bias_bondary ||
                  W.w[0]<w0c-wR+w_bondary || W.w[0]>w0c+wR-w_bondary ||
                  W.w[1]<w1c-wR+w_bondary || W.w[1]>w1c+wR-w_bondary ||
                  W.w[2]<w2c-wR+w_bondary || W.w[2]>w2c+wR-w_bondary);
    }
    while (not_stop);

    //Минимум найден, уменьшаем диапазон, чтобы уточнить его
    wR*=0.5f;
    biasR*=0.5f;
    if (wR>0.1f) return SearchFromPoint(bias,W,biasR,wR); //Повторяем функцию для уменьшенного диапазона возле минимума
    else return last_er;

}


MainWindow::~MainWindow()
{
    delete ui;
    delete[] mask;
}

void MainWindow::on_pushButton_clicked()
{
    /*img_n = 25;
    expert_marks = new vec2[img_n];
    test_img = new image_data[img_n];
    ComputeNames();
    ComputeIdeals(); //Это перестраивает файл идеальной х-ки (если файл уже есть, можно не вызывать)
    ComputeFreq(); //Это перестраивает файл характеристик изображений, если эти файлы есть, можно не вызывать
    ReadVectors(); //Читает идеальную характеристику и характеристики остальных изображений

    ReadExpertMarksRange();

    float b0 = ui->b0_input->value();
    float b1 = ui->b1_input->value();
    float w00 = ui->w00_input->value();
    float w01 = ui->w01_input->value();
    float w10 = ui->w10_input->value();
    float w11 = ui->w11_input->value();
    float w20 = ui->w20_input->value();
    float w21 = ui->w21_input->value();

    //Ищем минимум смещения и весов групп частот с начальными приближениями в диапазоне, заданном в параметрах
    SearchMinimum(b0, b1, w00, w01, w10, w11, w20, w21);*/
}

void MainWindow::on_folder_button_clicked()
{
    QString folder_name_s = QFileDialog::getExistingDirectory(this, tr("Open folder"), "C:\\", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(folder_name)
        delete[] folder_name;
    folder_name = new char[folder_name_s.length() + 1];
    folder_name[folder_name_s.length()] = '\0';
    strcpy(folder_name, folder_name_s.toStdString().c_str());
    qDebug("Folder name changed on: %s", folder_name);
    ui->folder_open->setText(folder_name_s);
    QStringList files;
    qDebug("Images in folder %d", CountImagesInFolder(folder_name, true, files));
}
