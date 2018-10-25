#include "waittingdlg.h"
#include <QLabel>
#include <QMovie>
#include <QBoxLayout>

CWaittingDlg::CWaittingDlg(QWidget *parent)
    : QDialog(parent, Qt::ToolTip), m_pLabMovie(NULL), m_pLabTips(NULL)
{
    _initCtrls();
    m_pMovie->start();
}

CWaittingDlg::~CWaittingDlg()
{
    m_pMovie->stop();
    delete m_pMovie;
    delete m_pLabMovie;
    delete m_pLabTips;
}

void CWaittingDlg::_initCtrls()
{
    m_pLabMovie = new QLabel(this);
    m_pLabMovie->setAlignment(Qt::AlignCenter);
    m_pLabTips = new QLabel("请等待", this);
    m_pLabTips->setAlignment(Qt::AlignCenter);
    m_pMovie = new QMovie(":/new/prefix1/res/wait.gif");
    m_pLabMovie->setMovie(m_pMovie);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pLabMovie,7);
    mainLayout->addWidget(m_pLabTips,3);
    setLayout(mainLayout);
}

void CWaittingDlg::on_show_tips(const QString &tips)
{
    m_pLabTips->setText(tips);
}
