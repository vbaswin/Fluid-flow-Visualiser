#include "vis/VtkAdapter.h"
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>

VtkAdapter::VtkAdapter() {
    renderer = vtkSmartPointer<vtkRenderer>::New();
    polyData = vtkSmartPointer<vtkPolyData>::New();
    renderer->SetBackground(0.1, 0.1, 0.15); // Dark blue-gray background

    // Enable depth peeling for correct transparency rendering
    renderer->SetUseDepthPeeling(true);
    renderer->SetMaximumNumberOfPeels(4);
    renderer->SetOcclusionRatio(0.1);
}

void VtkAdapter::initialize(vtkRenderWindow *window) {
    renderWindowRef = window;
    window->AddRenderer(renderer);

    // 1. Setup Particle Actor
    auto points = vtkSmartPointer<vtkPoints>::New();
    polyData->SetPoints(points);

    auto glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputData(polyData);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyphFilter->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(4);
    actor->GetProperty()->SetColor(0.0, 0.8, 1.0); // Cyan particles
    actor->GetProperty()->SetLighting(false);      // Flat color
    renderer->AddActor(actor);

    // 2. Setup Pipe Reference Visual
    auto tube = vtkSmartPointer<vtkCylinderSource>::New();
    tube->SetRadius(5.0);
    tube->SetHeight(20.0);
    tube->SetResolution(32);
    tube->CappingOff();

    auto tubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    tubeMapper->SetInputConnection(tube->GetOutputPort());

    auto tubeActor = vtkSmartPointer<vtkActor>::New();
    tubeActor->SetMapper(tubeMapper);
    tubeActor->GetProperty()->SetOpacity(0.2); // See-through
    tubeActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
    // Align cylinder with Z axis (VTK cylinder defaults to Y)
    tubeActor->RotateX(90);
    tubeActor->SetPosition(0, 0, 10.0); // Center at z=10
    renderer->AddActor(tubeActor);

    auto camera = renderer->GetActiveCamera();
    camera->SetPosition(0, -80, 0); // Camera on -Y axis, looking at pipe center
    camera->SetFocalPoint(0, 0, 10);
    camera->SetViewUp(1, 0, 0);
    renderer->ResetCameraClippingRange();
}

void VtkAdapter::renderParticles(const std::vector<Particle> &particles) {
    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints(particles.size());

    // Fast update of point coordinates
    for (size_t i = 0; i < particles.size(); ++i) {
        const auto &p = particles[i].pos;
        points->SetPoint(i, p.x, p.y, p.z);
    }

    polyData->SetPoints(points); // Update topology
    polyData->Modified();

    if (renderWindowRef)
        renderWindowRef->Render();
}

void VtkAdapter::startRecording(const std::string &filename) {
    if (isRecording) {
        std::cerr << "Warning: Recording already in progress\n";
        return;
    }

    if (!renderWindowRef) {
        std::cerr << "Error: No render window available for recording\n";
        return;
    }

    renderWindowRef->Render();

    frameGrabber = vtkSmartPointer<vtkWindowToImageFilter>::New();
    frameGrabber->SetInput(renderWindowRef);
    frameGrabber->SetInputBufferTypeToRGB(); // Important for FFMPEG
    frameGrabber->ReadFrontBufferOff();      // Capture off-screen buffer
    //
    frameGrabber->Update();

    videoWriter = vtkSmartPointer<vtkFFMPEGWriter>::New();
    videoWriter->SetInputConnection(frameGrabber->GetOutputPort());
    videoWriter->SetFileName(filename.c_str());
    videoWriter->SetRate(30); // 30 FPS
    videoWriter->SetQuality(2);
    // videoWriter->SetBitRate(5000000);

    videoWriter->Start();
    isRecording = true;
    std::cout << "Recording started: " << filename << "\n";
}

void VtkAdapter::stopRecording() {
    if (isRecording && videoWriter) {
        videoWriter->End();
        isRecording = false;
    }
    isRecording = false;
    videoWriter = nullptr;
    frameGrabber = nullptr;
}

void VtkAdapter::captureFrame() {
    if (isRecording && videoWriter && frameGrabber) {
        frameGrabber->Modified();
        frameGrabber->Update();
        videoWriter->Write();
    }
}
