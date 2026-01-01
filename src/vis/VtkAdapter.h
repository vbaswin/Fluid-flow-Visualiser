#pragma once
#include "physics/ParticleSys.h"
#include <vtkFFMPEGWriter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkWindowToImageFilter.h>

class VtkAdapter {
public:
  VtkAdapter();

  // Connects internal VTK renderer to the Qt Widget's window
  void initialize(vtkGenericOpenGLRenderWindow *window);

  // Updates visual representation of particles
  void renderParticles(const std::vector<Particle> &particles);

  // Handling Recording
  void startRecording(const std::string &filename);
  void stopRecording();
  void captureFrame();

  vtkRenderer *getRenderer() { return renderer; }

private:
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkPolyData> polyData; // Geometry container

  // Recording pipeline
  bool isRecording = false;
  vtkSmartPointer<vtkFFMPEGWriter> videoWriter;
  vtkSmartPointer<vtkWindowToImageFilter> frameGrabber;
  vtkGenericOpenGLRenderWindow *renderWindowRef = nullptr;
};
