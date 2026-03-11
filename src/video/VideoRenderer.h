#pragma once

#include <QQuickPaintedItem>
#include <QImage>

/**
 * @brief QML-compatible video renderer
 * 
 * Renders decoded video frames in QML interface.
 * This item can be placed in QML and will display video content.
 */
class VideoRenderer : public QQuickPaintedItem
{
    Q_OBJECT
    
public:
    explicit VideoRenderer(QQuickItem *parent = nullptr);
    
    void paint(QPainter *painter) override;
    
public slots:
    void updateFrame(const QImage &frame);
    void clear();

private:
    QImage m_currentFrame;
    bool m_hasFrame = false;
};
