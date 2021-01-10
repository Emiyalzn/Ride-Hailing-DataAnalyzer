#ifndef SELECTGRID_H
#define SELECTGRID_H

#include <QWidget>
#include <QPainter>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QtDebug>
#include <QScreen>

class SelectGrid : public QWidget
{
    Q_OBJECT

public:
    SelectGrid(QWidget *parent = 0);
    ~SelectGrid();
    QPoint m_beginPoint, m_endPoint;

signals:
    void completeSelection();

private:
    void initWindow();
    void loadBackgroundPixmap();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);

private:
    bool m_isMousePress;
    QPixmap m_loadPixmap, m_capturePixmap;
    int m_screenwidth;
    int m_screenheight;
    QPainter m_painter;
    QPoint corner;
};

#endif // SELECTGRID_H
