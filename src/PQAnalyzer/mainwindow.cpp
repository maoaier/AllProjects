#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "basedataview.h"
#include "harmdataview.h"
#include "powerdataview.h"
#include "unbalanceview.h"
#include "transeventview.h"
#include "flickdataview.h"
#include "overlimitview.h"
#include "exportdlg.h"
#include "waittingdlg.h"
#include "execlthread.h"
#include "datawaveformview.h"
//第二次
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/new/prefix1/res/logo.ico"));

    m_pStatTip = new QLabel(this);
    statusBar()->addWidget(m_pStatTip);

    CBaseDataView *pBaseView = new CBaseDataView(this);
    ui->tabWidget->addTab(pBaseView, "基础数据");
    m_funcViews.push_back((IViewBase*)pBaseView);

    CHarmDataView *pHarmView = new CHarmDataView(this);
    ui->tabWidget->addTab(pHarmView, "谐波数据");
    m_funcViews.push_back((IViewBase*)pHarmView);

    CPowerDataView *pPowerView = new CPowerDataView(this);
    ui->tabWidget->addTab(pPowerView, "功率数据");
    m_funcViews.push_back((IViewBase*)pPowerView);

    CUnbalanceView *pUnbView = new CUnbalanceView(this);
    ui->tabWidget->addTab(pUnbView, "不平衡度");
    m_funcViews.push_back((IViewBase*)pUnbView);

    CFlickDataView *pFlickView = new CFlickDataView(this);
    ui->tabWidget->addTab(pFlickView, "闪变数据");
    m_funcViews.push_back((IViewBase*)pFlickView);

    CTransEventView *pTranView = new CTransEventView(this);
    ui->tabWidget->addTab(pTranView, "暂态事件");
    m_funcViews.push_back((IViewBase*)pTranView);

    COverLimitView *pLimitView = new COverLimitView(this);
    ui->tabWidget->addTab(pLimitView, "越限事件");
    m_funcViews.push_back((IViewBase*)pLimitView);

    CDataWaveformView *pWaveView=new CDataWaveformView(this);
    //ui->tabWidget->addTab(pWaveView,"实时波形");
    pWaveView->hide();
    m_funcViews.push_back((IViewBase*)pWaveView);

    m_pExeThread = new CExeclThread(this);
}

MainWindow::~MainWindow()
{

    for (int i = 0; i != m_funcViews.size(); i++)
        delete m_funcViews.at(i);
    delete m_pExeThread;
    delete ui;
}

void MainWindow::_initViewsData()
{
    for (int i = 0; i != m_funcViews.size(); i++)
        m_funcViews.at(i)->setRootPath(m_statRootPath);
}

//打开文件触发效果
void MainWindow::on_action_open_triggered()
{
    QFileDialog *fileDlg = new QFileDialog(this);
    QStringList files  = fileDlg->getOpenFileNames(this, "打开文件", "", "数据文件(*.info)");
    if (files.isEmpty())
        return;

    CWaittingDlg *waitdlg = new CWaittingDlg(this);
    connect(m_pExeThread, SIGNAL(closedlg()), waitdlg, SLOT(close()));
    waitdlg->setAttribute(Qt::WA_DeleteOnClose);

    QString infoFilePath = files.at(0);
    int idx = infoFilePath.lastIndexOf('/');
    QString statFilePath = infoFilePath.left(idx + 1);
    m_statRootPath = statFilePath;
    m_pExeThread->setRootPath(statFilePath);
    m_pExeThread->startFunc(CExeclThread::ExeInit);

    m_pStatTip->setText(infoFilePath);

    waitdlg->exec();
    delete fileDlg;
}

//导出报表触发效果
void MainWindow::on_action_export_triggered()
{
//    CExportDlg *expdlg = new CExportDlg(this);
//    expdlg->setAttribute(Qt::WA_DeleteOnClose);
//    expdlg->exec();
    if (m_statRootPath.isEmpty()) {
        QMessageBox::information(this, "提示", "请先打开文件", QMessageBox::NoButton);
        return;
    }
    QFileDialog *fileDlg = new QFileDialog(this);
    QString filePath = fileDlg->getExistingDirectory(this, "导出位置");
    if (filePath.isEmpty())
        return;

    CWaittingDlg *waitdlg = new CWaittingDlg(this);
    connect(m_pExeThread, SIGNAL(closedlg()), waitdlg, SLOT(close()));
    connect(m_pExeThread, SIGNAL(sendTips(QString)), waitdlg, SLOT(on_show_tips(QString)));
    waitdlg->setAttribute(Qt::WA_DeleteOnClose);

    m_pExeThread->setReportPath(filePath);
    m_pExeThread->startFunc(CExeclThread::ExeReport);

    waitdlg->exec();
    delete fileDlg;
}

//波形界面
void MainWindow::on_actionDataWave_triggered()
{
    int count=ui->tabWidget->count();
    for(int i=0;i<count;i++)
    {
        ui->tabWidget->removeTab(0);
    }
    ui->tabWidget->addTab((CDataWaveformView *)m_funcViews.at(7),"实时波形");
    CDataWaveformView *dataWave=(CDataWaveformView *)m_funcViews.at(7);
    dataWave->run();

}

//初始界面
void MainWindow::on_actionInitInterface_triggered()
{
    int count=ui->tabWidget->count();
    for(int i=0;i<count;i++)
    {
        ui->tabWidget->removeTab(0);
    }
    ui->tabWidget->addTab((CBaseDataView *)m_funcViews.at(0),"基础数据");
    ui->tabWidget->addTab((CHarmDataView *)m_funcViews.at(1),"谐波数据");
    ui->tabWidget->addTab((CPowerDataView *)m_funcViews.at(2),"功率数据");
    ui->tabWidget->addTab((CUnbalanceView *)m_funcViews.at(3),"不平衡度");
    ui->tabWidget->addTab((CFlickDataView *)m_funcViews.at(4),"闪变数据");
    ui->tabWidget->addTab((CTransEventView *)m_funcViews.at(5),"暂态事件");
    ui->tabWidget->addTab((COverLimitView *)m_funcViews.at(6),"越限事件");
}
