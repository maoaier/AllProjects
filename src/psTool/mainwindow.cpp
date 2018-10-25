#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "../../../device/trunk/src/ps1000/storages/statdatatool.h"
#include "fftcalculate.h"

FFT_ONCE_DATA s_dat;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _initStatPlot();
    _initWavePlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_fileopen_triggered()
{
    QFileDialog *fileDlg = new QFileDialog(this);
    if (ui->tabWidget->currentWidget() == ui->tab_wave) {
        QStringList files  = fileDlg->getOpenFileNames(this, "打开文件", "", "数据文件(*.csv)");
        if (!files.isEmpty()) {
            m_waveFile = files.at(0);
            _analyseWaveDat(m_waveFile);
        }
    }
    else if (ui->tabWidget->currentWidget() == ui->tab_stat) {
        QStringList files  = fileDlg->getOpenFileNames(this, "打开文件", "", "数据文件(*.dat)");
        if (!files.isEmpty()) {
            m_fileName = files.at(0);
            _analyseStaDat(m_fileName, 0);
        }
    }
    delete fileDlg;
}

void MainWindow::_initStatPlot()
{
    ui->plot_stat->legend->setVisible(true);
    ui->plot_stat->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->plot_stat->xAxis->setTickLabelRotation(-10);
    ui->plot_stat->xAxis->setDateTimeFormat("yyyy-MM-dd hh:mm:ss");
    ui->plot_stat->axisRect()->setupFullAxesBox();
    ui->plot_stat->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    connect(ui->plot_stat, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(on_plot_stat_mouseWheel(QWheelEvent*)));
}

void MainWindow::_initWavePlot()
{
    ui->plot_wave->axisRect()->setupFullAxesBox();
    ui->plot_wave->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_bar->axisRect()->setupFullAxesBox();
    ui->plot_bar->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    connect(ui->plot_wave, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(on_plot_wave_mouseWheel(QWheelEvent*)));
    connect(ui->plot_bar, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(on_plot_bar_mouseWheel(QWheelEvent*)));
    connect(ui->plot_bar, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(on_plot_bar_mouseMove(QMouseEvent*)));
}

void MainWindow::_analyseStaDat(QString fileName, int type)
{
    int idx = fileName.lastIndexOf('/');
    int idx_dot = fileName.lastIndexOf('.');
    QString infoFilePath = fileName.left(idx + 1) + "stat.info";
    QString itemName = fileName.mid(idx + 1, idx_dot - idx - 1);
    CStatDataTool tool;
    StatRecordInfo info;
    tool.ReadInfoFromFile(infoFilePath.toStdString(), info);
    QMap<QString, int> fileDatMap;
    fileDatMap["frequency"] = 1;
    fileDatMap["pos_value"] = 1;
    fileDatMap["pos_ang"] = 1;
    fileDatMap["neg_value"] = 1;
    fileDatMap["neg_ang"] = 1;
    fileDatMap["zero_value"] = 1;
    fileDatMap["zero_ang"] = 1;
    fileDatMap["neg_unbalance"] = 1;
    fileDatMap["zero_unbalance"] = 1;
    fileDatMap["active_power"] = 5;
    fileDatMap["reactive_power"] = 5;
    fileDatMap["apparent_power"] = 5;
    fileDatMap["power_factor"] = 5;
    fileDatMap["rms"] = 4;
    fileDatMap["ang"] = 4;
    fileDatMap["pos_peak"] = 4;
    fileDatMap["neg_peak"] = 4;
    fileDatMap["harm_dc_rms"] = 4;
    fileDatMap["harm_dc_per"] = 4;
    fileDatMap["harm_thd_rms"] = 4;
    fileDatMap["harm_thd_per"] = 4;
    fileDatMap["harm_power"] = 4;
    fileDatMap["harm_rms"] = 4;
    fileDatMap["harm_per"] = 4;
    fileDatMap["harm_ang"] = 4;
    fileDatMap["inter_harm_rms"] = 4;
    fileDatMap["inter_harm_per"] = 4;
    fileDatMap["high_harm_rms"] = 4;
    fileDatMap["high_harm_per"] = 4;

    for (QMap<QString, int>::iterator ite = fileDatMap.begin();
         ite != fileDatMap.end(); ++ite) {
        if (!fileName.contains(ite.key()))
            continue;
        StatDatas dats;
        tool.InitReadFile(fileName.toStdString(), ite.value());
        tool.ReadDataFromFile(0, info.statTotalCnt - 1, dats);
        QVector<double> x;
        QVector< QVector<double> > ys;
        ui->plot_stat->clearGraphs();
        for (uint i = 0; i != dats.size(); i++) {
            UtcTime curTime = info.startTime.toUtcTime();
            curTime.tv_sec += i * info.statCycle;
            DateTime curDate = curTime.toDateTime();
            QDateTime tmp(QDate(curDate.year, curDate.month, curDate.day),
                          QTime(curDate.hour, curDate.minute, curDate.second, curDate.ms));
            x.push_back(tmp.toTime_t());

            std::vector<float> *p;
            if (type == 0) p = &dats.at(i).max_val;
            else if (type == 1) p = &dats.at(i).min_val;
            else if (type == 2) p = &dats.at(i).avg_val;
            else if (type == 3) p = &dats.at(i).cp95_val;
            if (i == 0) {
                for (uint j = 0; j != p->size(); j++) {
                    QPen pen;
                    QString name;
                    name.sprintf("%s %d", itemName.toStdString().c_str(), j + 1);
                    pen.setColor(QColor(j*50 + 50, (j%2)*50 + 50, (j%3)*50 + 50));
                    ui->plot_stat->addGraph();
                    ui->plot_stat->graph(j)->setPen(pen);
                    ui->plot_stat->graph(j)->setName(name);
                    ui->plot_stat->graph(j)->setLineStyle(QCPGraph::lsLine);
                    ui->plot_stat->graph(j)->setScatterStyle(QCPScatterStyle::ssDisc);
                    ui->plot_stat->graph(j)->setSelectable(false);
                    QVector<double> y;
                    ys.push_back(y);
                }
            }

            for (uint j = 0; j != p->size(); j++)
                ys[j].push_back(p->at(j));
        }

        for (int i = 0; i != ys.size(); i++) {
            ui->plot_stat->graph(i)->setData(x, ys.at(i));
            ui->plot_stat->rescaleAxes();
        }
        ui->plot_stat->replot();
        break;
    }
}

void MainWindow::_analyseWaveDat(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    // 读波形数据
    char lineBuf[50];
    int i = 0;
    QVector<double> x, y;
    while (file.readLine(lineBuf, 50) != -1) {
        double val;
        x.push_back(i++);
        sscanf(lineBuf, "%lf", &val);
        y.push_back(val);
    }
    file.close();
    // 画波形图
    ui->plot_wave->clearGraphs();
    ui->plot_wave->addGraph();
    ui->plot_wave->graph(0)->setPen(QPen(Qt::blue));
    ui->plot_wave->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_wave->graph(0)->setData(x, y);
    ui->plot_wave->rescaleAxes();
    ui->plot_wave->replot();
    // 画柱状图
    if (ui->plot_bar->plottableCount() != 0)
        ui->plot_bar->clearPlottables();
    QCPBars *bar = new QCPBars(ui->plot_bar->xAxis, ui->plot_bar->yAxis);
    ui->plot_bar->addPlottable(bar);
    bar->setPen(Qt::NoPen);
    bar->setBrush(Qt::blue);
    CFftCalculate fft;
    fft.CalculateFFT(y, 128, 10, s_dat);
    QVector<double> y1;
    for (int i = 0; i != s_dat.size(); i++) {
        FFT_DATA fftdat = s_dat.at(i);
        y1.push_back(fftdat.rms);
    }
    bar->setData(x, y1);
    ui->plot_bar->rescaleAxes();
    ui->plot_bar->replot();
}

void MainWindow::on_action_max_triggered()
{
    if (!ui->action_max->isChecked())
        return;
    else {
        ui->action_max->setChecked(true);
        ui->action_min->setChecked(false);
        ui->action_avg->setChecked(false);
        ui->action_cp95->setChecked(false);
        _analyseStaDat(m_fileName, 0);
    }
}

void MainWindow::on_action_min_triggered()
{
    if (!ui->action_min->isChecked())
        return;
    else {
        ui->action_min->setChecked(true);
        ui->action_max->setChecked(false);
        ui->action_avg->setChecked(false);
        ui->action_cp95->setChecked(false);
        _analyseStaDat(m_fileName, 1);
    }
}

void MainWindow::on_action_avg_triggered()
{
    if (!ui->action_avg->isChecked())
        return;
    else {
        ui->action_avg->setChecked(true);
        ui->action_max->setChecked(false);
        ui->action_min->setChecked(false);
        ui->action_cp95->setChecked(false);
        _analyseStaDat(m_fileName, 2);
    }
}

void MainWindow::on_action_cp95_triggered()
{
    if (!ui->action_cp95->isChecked())
        return;
    else {
        ui->action_cp95->setChecked(true);
        ui->action_max->setChecked(false);
        ui->action_min->setChecked(false);
        ui->action_avg->setChecked(false);
        _analyseStaDat(m_fileName, 3);
    }
}

void MainWindow::on_plot_stat_mouseWheel(QWheelEvent *event)
{
    _on_mouseWheel(ui->plot_stat, event);
}

void MainWindow::on_plot_wave_mouseWheel(QWheelEvent *event)
{
    _on_mouseWheel(ui->plot_wave, event);
}

void MainWindow::on_plot_bar_mouseWheel(QWheelEvent *event)
{
    _on_mouseWheel(ui->plot_bar, event);
}

void MainWindow::_on_mouseWheel(QCustomPlot *plot, QWheelEvent *event)
{
    QPoint pos = event->pos();
    QRect rect_axis = plot->xAxis->axisRect()->rect();
    if (rect_axis.contains(pos)) {
        plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
        return;
    }

    if (pos.x() < rect_axis.left())
        plot->axisRect()->setRangeZoom(plot->yAxis->orientation());
    else if (pos.y() > rect_axis.bottom())
        plot->axisRect()->setRangeZoom(plot->xAxis->orientation());
}

void MainWindow::on_plot_bar_mouseMove(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QRect rect_axis = ui->plot_bar->xAxis->axisRect()->rect();
    if (!rect_axis.contains(pos)) {
        return;
    }
    double val_x = ui->plot_bar->xAxis->pixelToCoord(pos.x());
    int x = val_x;
    if (val_x - x > 0.5) x++;
    if (x >= s_dat.size() || x < 0)
        return;
    QString valStr;
    valStr.sprintf("%s: harm %d \trms %.3lf \tang %.3lf", m_waveFile.toStdString().c_str(), x, s_dat.at(x).rms, s_dat.at(x).ang);
    statusBar()->showMessage(valStr);
}
