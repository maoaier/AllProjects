#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initParameter();//初始化参数
    initInterface();//初始化接口
}

//初始化类中的参数
void MainWindow::initParameter()
{
    m_fAmplitude=STANDARD_AMPLITUDE;
    m_fPhase=STANDARD_PHASE;
    m_iSamplingPoint=SAMPLING_POINT;
    m_iWaveNumber=SHOW_WAVE_NUMBER;
    m_bHarmonicState=false;
    ui->lineEd_amplitude->setText(QString::number(m_fAmplitude));//把参数显示到界面中
    ui->lineEd_phase->setText(QString::number(m_fPhase));
    ui->lineEd_point->setText(QString::number(m_iSamplingPoint));
    ui->lineEd_harmonicAmplitude->setText(QString::number(0));
    ui->lineEd_harmonicNumber->setText(QString::number(0));
    ui->lineEd_harmonicPhase->setText(QString::number(0));
    ui->lineEd_WaveNumber->setText(QString::number(m_iWaveNumber));
}

//初始化接口
void MainWindow::initInterface()
{
    QCustomPlot *customPlot=ui->customPlot;
    customPlot->addGraph();//增加一条线
    m_pWaveLine=customPlot->graph(0);
    m_pWaveLine->setPen(QPen(Qt::blue));//设置线的颜色为蓝色
    //m_pWaveLine->setBrush(QBrush(QColor(0,0,255,20)));//设置蓝色为填充色
    m_pWaveLine->setLineStyle(QCPGraph::lsLine);//设置线为连接的
    m_pWaveLine->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));//设置分散方式为第五种
    //customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot->xAxis->setLabel("采样点");//设置标签
    customPlot->yAxis->setLabel("电压(V)");
    QVector<double> x,y;
    samplingData(x,y);//设置默认的数据处理方式
    m_pWaveLine->addData(x,y);//增加数据
    m_pWaveLine->rescaleAxes(true);//自动适应本线
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);//设置表格的交互作用
}


//采样数据
void MainWindow::samplingData(QVector<double> &x,QVector<double> &y)
{
    qDebug()<<"data processing\n";
    double VoltageRange=m_fAmplitude*sqrt(2);//电压范围
    double WaveRange=m_iWaveNumber*2*M_PI;
    //存在谐波数据生成处理
    if(m_cHarmonicData.size())
    {
        int number=m_cHarmonicData.size();
        for(int i=0;i<m_iSamplingPoint;++i)
        {
            x.append(i);
            double dataSum=0;
            for(int j=0;j<number;j++)
            {
                //增加谐波数据
                const CHarmonic &harm=m_cHarmonicData.at(j);
                dataSum+= harm.m_fRange*qSin(WaveRange*i*harm.m_iNumber/m_iSamplingPoint+harm.m_fPhase/180*M_PI);
            }
            //增加基波数据
             y.append(dataSum+VoltageRange*qSin(WaveRange*i/m_iSamplingPoint+m_fPhase/180*M_PI));
        }
    }
    else//只有基波数据处理
    {
        for(int i=0;i<m_iSamplingPoint;++i)
        {
            x.append(i);
            y.append(VoltageRange*qSin(WaveRange*i/(m_iSamplingPoint-1)+m_fPhase/180*M_PI));
        }
    }
    //波形状态已经改变
    m_bHarmonicState=false;
}


//展示配置参数的数据
void MainWindow::on_pushBtn_show_clicked()
{
    //取出行编辑的数据
    float amplitude=ui->lineEd_amplitude->text().toFloat();
    float phase=ui->lineEd_phase->text().toFloat();
    int point=ui->lineEd_point->text().toInt();

    //处理波形数目
    int waveNumber=ui->lineEd_WaveNumber->text().toInt();
    if((waveNumber>0)&&(waveNumber!=m_iWaveNumber))
    {
        m_iWaveNumber=waveNumber;
        m_bHarmonicState=true;
    }
    //判断数据是否改变，如果没有改变则不做处理
    if((amplitude!=m_fAmplitude)||(phase!=m_fPhase)||(point!=m_iSamplingPoint)||(true==m_bHarmonicState))
    {
        qDebug()<<"get new data\n";
        //判断采样点是否正确
        if(m_iSamplingPoint<0)
        {
            if((amplitude==m_fAmplitude)&&(phase==m_fAmplitude))
            {
                m_pWaveLine->rescaleAxes(false);
                m_pWaveLine->rescaleAxes(true);
                ui->customPlot->replot();
                return ;
            }
        }
        m_iSamplingPoint=point;
        m_fAmplitude=amplitude;
        m_fPhase=phase;
        m_pWaveLine->clearData();
        QVector<double> x,y;
        samplingData(x,y);
        m_pWaveLine->addData(x,y);
    }
    //重新让表格自动适应
    m_pWaveLine->rescaleAxes(false);
    m_pWaveLine->rescaleAxes(true);
    ui->customPlot->replot();
}




MainWindow::~MainWindow()
{
    delete ui;
}


//增加谐波数据
void MainWindow::on_pushBuAdd_clicked()
{
    CHarmonic harmonic;//向保存谐波的结构体赋值
    harmonic.m_iNumber=ui->lineEd_harmonicNumber->text().toInt();
    harmonic.m_fAmplitude=ui->lineEd_harmonicAmplitude->text().toFloat();
    harmonic.m_fPhase=ui->lineEd_harmonicPhase->text().toFloat();
    harmonic.m_fRange=harmonic.m_fAmplitude*sqrt(2);
    if(harmonic.m_iNumber<2)
    {
        return ;
    }
    QString str;
    str+=ui->lineEd_harmonicNumber->text()+"次谐波 ";
    str+=ui->lineEd_harmonicAmplitude->text()+"幅值 ";
    str+=ui->lineEd_harmonicPhase->text()+"相位";
    ui->comboBox_harmonic->addItem(str);
    m_cHarmonicData.append(harmonic);
    m_bHarmonicState=true;
}


void MainWindow::initTest()
{
//    QCustomPlot *customPlot=ui->customPlot;
//    customPlot->addGraph();//增加一条线
//    m_pWaveLine->setPen(QPen(Qt::blue));//设置线的颜色为蓝色
//    m_pWaveLine->setBrush(QBrush(QColor(0,0,255,20)));//设置蓝色为填充色
//    customPlot->addGraph();
//    customPlot->graph(1)->setPen(QPen(Qt::red));
//    QVector<double> x(251),y0(251),y1(251);
//    for(int i=0;i<251;++i)
//    {
//        x[i]=i;
//        y0[i]=qExp(-i/150.0)*qCos(i/10);
//        y1[i]=qExp(-i/150);
//    }
//    //    //设置刻度尺不设置标签
//    //    customPlot->xAxis2->setVisible(true);
//    //    customPlot->xAxis2->setTickLabels(true);
//    //    customPlot->yAxis2->setVisible(true);
//    //    customPlot->yAxis2->setTickLabels(true);
//    // //    左边和下边的轴改变将影响到右边和上边的结果
//    //    connect(customPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),customPlot->xAxis2,SLOT(setRange(QCPRange)));
//    //    connect(customPlot->yAxis,SIGNAL(rangeChanged(QCPRange)),customPlot->yAxis2,SLOT(setRange(QCPRange)));
//        //传递数据点到显示到图形
//    m_pWaveLine->setData(x,y0);
//    customPlot->graph(1)->setData(x,y1);
//    m_pWaveLine->rescaleAxes();
//    customPlot->graph(1)->rescaleAxes(true);
    //    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

//清除谐波数据
void MainWindow::on_pushBuClear_clicked()
{
    if(m_cHarmonicData.size())
    {
        m_cHarmonicData.clear();
        m_bHarmonicState=true;
        ui->comboBox_harmonic->clear();
    }

}
