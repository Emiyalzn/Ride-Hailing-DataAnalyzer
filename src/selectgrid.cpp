//
//Created on 2020-12-06
//
#include "selectgrid.h"

SelectGrid::SelectGrid(QWidget *parent)
    :QWidget(parent),
      m_isMousePress(false)
{
    initWindow();
    loadBackgroundPixmap();
}

SelectGrid::~SelectGrid()
{

}

//set window mode
void SelectGrid::initWindow()
{
    this->setMouseTracking(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowState(Qt::WindowActive | Qt::WindowFullScreen);
}

void SelectGrid::loadBackgroundPixmap()
{
    QScreen *screen = QApplication::primaryScreen();
    m_loadPixmap = screen->grabWindow(QApplication::desktop()->winId());
    m_screenwidth = m_loadPixmap.width();
    m_screenheight = m_loadPixmap.height();
}

//get start point
void SelectGrid::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isMousePress = true;
        m_beginPoint = event->pos();
    }
    return QWidget::mousePressEvent(event);
}

//update end point
void SelectGrid::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isMousePress)
    {
        m_endPoint = event->pos();
        update();
    }
    return QWidget::mouseMoveEvent(event);
}

//get end point
void SelectGrid::mouseReleaseEvent(QMouseEvent *event)
{
    m_endPoint = event->pos();
    m_isMousePress = false;
    return QWidget::mouseReleaseEvent(event);
}

void SelectGrid::keyPressEvent(QKeyEvent *event)
{
    // Esc to escape
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
    // Enter to fin
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        emit completeSelection();
        close();
    }
}

//repaint selected rect area
void SelectGrid::paintEvent(QPaintEvent *event)
{
    m_painter.begin(this);          //repaint

    QColor shadowColor = QColor(0, 0, 0, 100);
    m_painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::FlatCap));
    m_painter.drawPixmap(0, 0, m_loadPixmap);
    m_painter.fillRect(m_loadPixmap.rect(), shadowColor);

    if (m_isMousePress)
    {
        QRect selectedRect = getRect(m_beginPoint, m_endPoint);
        m_capturePixmap = m_loadPixmap.copy(selectedRect);
        m_painter.drawPixmap(selectedRect.topLeft(), m_capturePixmap);
        m_painter.drawRect(selectedRect);
    }
    m_painter.end();
}

QRect SelectGrid::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
    int x, y, width, height;
    width = qAbs(beginPoint.x() - endPoint.x());
    height = qAbs(beginPoint.y() - endPoint.y());
    x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
    y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

    QRect selectedRect = QRect(x, y, width, height);

    //avoid selecting only one point
    if (selectedRect.width() == 0)
    {
        selectedRect.setWidth(1);
    }
    if (selectedRect.height() == 0)
    {
        selectedRect.setHeight(1);
    }

    return selectedRect;
}
