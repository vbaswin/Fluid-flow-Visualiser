#include "MainWindow.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <vtkCellPicker.h>
#include <vtkRenderWindow.h>

vtkStandardNewMacro(InteractionHandler);

void InteractionHandler::OnLeftButtonDown() {
    // 1. Get Mouse Position
    int *pos = this->Interactor->GetEventPosition();

    // 2. Raycast into scene
    auto picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);
    picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

    // 3. Apply perturbation at clicked 3D location
    double *p = picker->GetPickPosition(); // Returns  if miss
    if (physics) {
        // Add a "repulsive" force at click location
        physics->addPerturbation({p[0], p[1], p[2]}, 50.0);
    }

    // Default camera rotation behavior
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();

    // Initialize Logic
    flowModel = std::make_unique<PipeFlow>(5.0); // Radius 5
    particleSystem = std::make_unique<ParticleSystem>(2000, 5.0, 20.0);
    particleSystem->setFlowField(flowModel.get());

    visualizer = std::make_unique<VtkAdapter>();
    visualizer->initialize(vtkWindow->renderWindow());

    // Setup Interaction
    auto style = vtkSmartPointer<InteractionHandler>::New();
    style->SetDefaultRenderer(visualizer->getRenderer());
    style->setPhysics(flowModel.get());
    vtkWindow->renderWindow()->GetInteractor()->SetInteractorStyle(style);

    // Setup Game Loop (60 FPS)
    loopTimer = new QTimer(this);
    connect(loopTimer, &QTimer::timeout, this, &MainWindow::gameLoop);
    loopTimer->start(16);
}

MainWindow::~MainWindow() {
    if (isRecording)
        visualizer->stopRecording();
}

void MainWindow::setupUi() {
    QWidget *central = new QWidget;
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // 1. VTK Viewport
    vtkWindow = new QVTKOpenGLNativeWidget;
    mainLayout->addWidget(vtkWindow, 4); // Takes 80% space

    // 2. Sidebar Controls
    QVBoxLayout *controls = new QVBoxLayout;

    controls->addWidget(new QLabel("Flow Speed:"));
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(0, 200);
    speedSlider->setValue(100);
    controls->addWidget(speedSlider);

    connect(speedSlider, &QSlider::valueChanged, [this](int val) {
        flowModel->setFlowSpeed(val / 50.0); // Map 0-200 to 0.0-4.0
    });

    controls->addStretch();

    recordButton = new QPushButton("Start Recording");
    controls->addWidget(recordButton);
    connect(recordButton, &QPushButton::clicked, [this]() {
        isRecording = !isRecording;
        if (isRecording) {
            visualizer->startRecording("output.avi");
            recordButton->setText("Stop Recording");
        } else {
            visualizer->stopRecording();
            recordButton->setText("Start Recording");
        }
    });

    mainLayout->addLayout(controls, 1); // Takes 20% space
    resize(1000, 600);
    setWindowTitle("Fluid Flow Visualizer");
}

void MainWindow::gameLoop() {
    constexpr double dt = 0.016; // match 16ms timer interval
    // 1. Update Physics
    flowModel->update(dt); // decay pertubations each frame
    particleSystem->update(dt);

    // 2. Update Visuals
    visualizer->renderParticles(particleSystem->getParticles());

    // 3. Record Frame if active
    if (isRecording) {
        visualizer->captureFrame();
    }
}
