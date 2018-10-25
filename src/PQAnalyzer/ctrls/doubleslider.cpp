#include "doubleslider.h"
#include <math.h>
#include <QPainter>
#include <QMouseEvent>

DoubleSlider::DoubleSlider(QWidget* parent)
    : QWidget(parent)
    , m_min(0.0f)
    , m_max(99.0f)
    , m_singleStep(0.1f)
    , m_minValue(0.0f)
    , m_maxValue(99.0f)
    , m_state(None)
{
    setFocusPolicy(Qt::StrongFocus);
}

void DoubleSlider::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    paintValueLabel(&painter);
}

void DoubleSlider::paintColoredRect(QRect rect, QColor color ,QPainter* painter)
{
    painter->fillRect(rect,QBrush(color));
}

void DoubleSlider::paintValueLabel(QPainter* painter)
{
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial",12));

    int minPos = (m_minValue - m_min) * width() / (m_max - m_min);
    int maxPos = (m_maxValue - m_min) * width() /  (m_max - m_min);

    if(minPos <= 4){
        minPos = 4;
    }else if(minPos >= width() - 10){
        minPos = width() - 10;
    }else{}

    if(maxPos <= 4){
        maxPos = 4;
    }else if(maxPos >= width() -10){
        maxPos = width() - 10;
    }else{}


    //----- paint groove
    paintColoredRect(QRect(4,height()/2 - 2,width() - 8,4),Qt::gray,painter);
    paintColoredRect(QRect(minPos + 4,height()/2 - 2,maxPos - minPos,4),QColor(51,153,155),painter);

    //----- handle

    minHandleRegion = QRect(minPos,height()/2 - 10,10,20);
    maxHandleRegion = QRect(maxPos,height()/2 - 10,10,20);

    //-----paint Handle
    QColor minColor  = (m_state == MinHandle) ? QColor(51,153,155) : Qt::darkGray;
    QColor maxColor  = (m_state == MaxHandle) ? QColor(51,153,155) : Qt::darkGray;
    paintColoredRect(minHandleRegion,minColor,painter);
    paintColoredRect(maxHandleRegion,maxColor,painter);
}

inline float getValidValue(float val, float min, float max)
{
    float tmp = std::max(val, min);
    return std::min(tmp, max);
}


void DoubleSlider::setRange(float min, float max)
{
    m_min = min;
    m_max = max;

    if(minValue() < m_min)
        setMinValue(m_min);

    if(maxValue() > m_max){
        setMaxValue(m_max);
    }
}

void DoubleSlider::setSingleStep(float step)
{
    m_singleStep = step;
}

float DoubleSlider::minValue() const
{
    return m_minValue;
}
void DoubleSlider::setMinValue(float val)
{
    if(fabs( m_minValue - val ) > 0.0001 ){
        m_minValue = val;
        emit minValueChanged(val);
    }
}

float DoubleSlider::maxValue() const
{
    return m_maxValue;
}
void DoubleSlider::setMaxValue(float val)
{
    if(fabs(m_maxValue - val) > 0.0001){
        m_maxValue = val;
        emit maxValueChanged(val);
    }
}

void DoubleSlider::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    m_state = None;
    update();
}

float DoubleSlider::minRange() const
{
    return m_min;
}
float DoubleSlider::maxRange() const
{
    return m_max;
}


void DoubleSlider::mousePressEvent(QMouseEvent* event)
{
    if(minHandleRegion.contains(event->pos())){
        m_state = MinHandle;
    }else if(maxHandleRegion.contains(event->pos())){
        m_state = MaxHandle;
    }else{
        m_state = None;
    }

    if(fabs(minValue() - maxValue()) < 0.001){
        if(fabs(minValue() - m_min) < 0.001){
            m_state = MaxHandle;
        }else if(fabs(minValue() - m_max) < 0.001){
            m_state = MinHandle;
        }else{}
    }

    update();
}

void DoubleSlider::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() & Qt::LeftButton){
        float move = event->x() * (m_max - m_min) * 1.0/ width() + m_min;

        switch(m_state){
            case MinHandle:{
                float val = getValidValue( move, m_min,maxValue());
                setMinValue(val);
                break;
            }

            case MaxHandle:{
                    float val = getValidValue(move, minValue(), m_max);
                    setMaxValue(val);
                    break;
            }
            case None:default: break;
        }
    }
    update();
}


void DoubleSlider::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);

    if(event->key() == Qt::Key_Left){
        if(m_state == MinHandle){
            float val = getValidValue(minValue()- m_singleStep, m_min ,maxValue());
            setMinValue(val);
        }else if(m_state == MaxHandle) {
            float val = getValidValue(maxValue()- m_singleStep, minValue() ,m_max);
            setMaxValue(val);
        }else{}
    }else if(event->key() == Qt::Key_Right){
        if(m_state == MinHandle){
            float val = getValidValue(minValue() + m_singleStep, m_min ,maxValue());
            setMinValue(val);
        }else if(m_state == MaxHandle) {
            float val = getValidValue(maxValue() + m_singleStep, minValue() ,m_max);
            setMaxValue(val);
        }else{}
    }

    update();

}
