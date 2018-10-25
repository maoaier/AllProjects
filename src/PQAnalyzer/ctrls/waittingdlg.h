#ifndef CWAITTINGWIDGET_H
#define CWAITTINGWIDGET_H

#include <QDialog>

class QLabel;
class QMovie;

class CWaittingDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CWaittingDlg(QWidget *parent = 0);
    virtual ~CWaittingDlg();

public slots:
    void on_show_tips(const QString &tips);

protected:
    void _initCtrls();

protected:
    QLabel *m_pLabMovie;
    QLabel *m_pLabTips;
    QMovie *m_pMovie;
};

#endif // CWAITTINGWIDGET_H
