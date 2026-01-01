#pragma once
#include "physics/ParticleSys.h"
#include "physics/PipeFlow.h"
#include "vis/VtkAdapter.h"
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkInteractorStyleTrackballCamera.h>

// Custom Interactor to handle clicks in 3D space
class InteractionHandler : public vtkInteractorStyleTrackballCamera {
public:
  static InteractionHandler *New();
  vtkTypeMacro(InteractionHandler, vtkInteractorStyleTrackballCamera);

  void OnLeftButtonDown() override;
  void setPhysics(IVectorField *phys) { physics = phys; }

private:
  IVectorField *physics = nullptr;
};

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  void setupUi();
  void gameLoop(); // Called by timer

  // UI Elements
  QVTKOpenGLNativeWidget *vtkWindow;
  QSlider *speedSlider;
  QPushButton *recordButton;
  QTimer *loopTimer;

  // Logic Modules
  std::unique_ptr<PipeFlow> flowModel;
  std::unique_ptr<ParticleSystem> particleSystem;
  std::unique_ptr<VtkAdapter> visualizer;

  bool isRecording = false;
};
