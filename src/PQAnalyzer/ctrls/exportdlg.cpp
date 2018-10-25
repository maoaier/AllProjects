#include "exportdlg.h"
#include "doubleslider.h"
#include <QLabel>
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

CExportDlg::CExportDlg(QWidget *parent)
    : QDialog(parent, Qt::ToolTip)
{
    m_startTime = QDateTime::currentDateTime();
    m_endTime = QDateTime::fromTime_t(m_startTime.toTime_t() + 60);
    _initCtrls();
}

CExportDlg::~CExportDlg()
{
    delete m_pTimeLab;
    delete m_pDoubSlider;
    delete m_pPathEdit;
}

void CExportDlg::_initCtrls()
{
    setWindowTitle("导出报表");
    setWindowFlags(Qt::Dialog);

    QLabel *pLab = new QLabel("导出路径：", this);
    m_pPathEdit = new QLineEdit(this);
    m_pPathEdit->setFixedWidth(300);
    QPushButton *pbtn = new QPushButton("选择", this);

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(pLab, 2);
    layout1->addWidget(m_pPathEdit);
    layout1->addWidget(pbtn, 2);

    m_pTimeLab = new QLabel("导出时间：", this);
    m_pTimeLab->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_pTimeLab->setFixedWidth(360);
    m_pDoubSlider = new DoubleSlider(this);
    m_pDoubSlider->setRange(0, 100);
    m_pDoubSlider->setSingleStep(1);

    connect(m_pDoubSlider, SIGNAL(maxValueChanged(float)), this, SLOT(maxSliderChanged(float)));
    connect(m_pDoubSlider, SIGNAL(minValueChanged(float)), this, SLOT(minSliderChanged(float)));
    connect(pbtn, SIGNAL(pressed()), this, SLOT(btnPress()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addWidget(m_pTimeLab);
    mainLayout->addWidget(m_pDoubSlider);
    setLayout(mainLayout);

    _updateTimeLab();
}

void CExportDlg::_updateTimeLab()
{
    int fullTime = m_endTime.toTime_t() - m_startTime.toTime_t();
    int startTime = m_startTime.toTime_t() + m_pDoubSlider->minValue()/100*fullTime;
    int endTime = m_startTime.toTime_t() + m_pDoubSlider->maxValue()/100*fullTime;

    QDateTime st = QDateTime::fromTime_t(startTime);
    QDateTime et = QDateTime::fromTime_t(endTime);
    QString text;
    text.sprintf("导出时间：  %s  --  %s", st.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str(),
                 et.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());
    m_pTimeLab->setText(text);
}

void CExportDlg::maxSliderChanged(float /*val*/)
{
    _updateTimeLab();
}

void CExportDlg::minSliderChanged(float /*val*/)
{
    _updateTimeLab();
}

void CExportDlg::btnPress()
{
    QFileDialog *fileDlg = new QFileDialog(this);
    QString filePath = fileDlg->getExistingDirectory(this, "导出位置");
    if (filePath.isEmpty())
        return;
    m_pPathEdit->setText(filePath);
}
