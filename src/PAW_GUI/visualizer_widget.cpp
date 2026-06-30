#include "visualizer_widget.h"
#include <QPainter>
#include <QColor>
#include <algorithm>

VisualizerWidget::VisualizerWidget(QWidget *parent)
    : QOpenGLWidget(parent){
}

void VisualizerWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}