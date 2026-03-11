#include "VideoRenderer.h"
#include <QPainter>

VideoRenderer::VideoRenderer(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setFillColor(Qt::black);
}

void VideoRenderer::paint(QPainter *painter)
{
    if (!m_hasFrame || m_currentFrame.isNull()) {
        // Draw black background
        painter->fillRect(boundingRect(), Qt::black);
        return;
    }
    
    // Scale frame to fit while maintaining aspect ratio
    QRectF target = boundingRect();
    QSizeF frameSize = m_currentFrame.size();
    
    double aspectRatio = frameSize.width() / frameSize.height();
    double targetAspect = target.width() / target.height();
    
    QRectF drawRect;
    
    if (aspectRatio > targetAspect) {
        // Frame is wider - fit to width
        double height = target.width() / aspectRatio;
        drawRect = QRectF(
            target.x(),
            target.y() + (target.height() - height) / 2,
            target.width(),
            height
        );
    } else {
        // Frame is taller - fit to height
        double width = target.height() * aspectRatio;
        drawRect = QRectF(
            target.x() + (target.width() - width) / 2,
            target.y(),
            width,
            target.height()
        );
    }
    
    // Draw black bars
    painter->fillRect(target, Qt::black);
    
    // Draw scaled frame
    painter->drawImage(drawRect, m_currentFrame);
}

void VideoRenderer::updateFrame(const QImage &frame)
{
    m_currentFrame = frame;
    m_hasFrame = true;
    update();
}

void VideoRenderer::clear()
{
    m_currentFrame = QImage();
    m_hasFrame = false;
    update();
}
