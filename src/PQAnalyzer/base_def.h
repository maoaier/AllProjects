#ifndef BASE_DEF
#define BASE_DEF

#include <QColor>
#include <QVector>

typedef struct _StatOnePoint
{
    double x;
    QVector<double> y;
}StatOnePoint;
typedef QList<StatOnePoint> StatDataPoints;

typedef struct _StatGraphInfo
{
    QString name;   // 图例的名称
    QString unit;   // 图例的单位
    QColor col;     // 图例的颜色
    bool visiable;  // 是否显示
}StatGraphInfo;
typedef QList<StatGraphInfo> StatGraphInfos;

typedef struct _StatItemInfo
{
    _StatItemInfo() {}
    _StatItemInfo(QString ch, QString ut) { chinese = ch; unit = ut; }
    QString chinese;    // 数据项中文
    QString unit;       // 数据项单位
}StatItemInfo;

#endif // BASE_DEF

