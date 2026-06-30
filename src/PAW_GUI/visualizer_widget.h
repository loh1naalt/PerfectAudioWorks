#pragma once 

#include <QOpenGLWidget>
#include <QOpenGLFunctions> // 1. Include QOpenGLFunctions

// 2. Inherit from QOpenGLFunctions
class VisualizerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    // 'explicit' is good practice to prevent accidental implicit conversions
    explicit VisualizerWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
};