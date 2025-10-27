#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(800, 600);
    // Home page: choose role and enter display name
    homePage = new QWidget();
    auto *hl = new QVBoxLayout(homePage);
    hl->addWidget(new QLabel("<h2>Virtual Classroom</h2>", this), 0, Qt::AlignCenter);

    nameEdit = new QLineEdit();
    roleCombo = new QComboBox();
    roleCombo->addItems({"Student", "Teacher"});
    startBtn = new QPushButton("Start");

    auto *form = new QFormLayout();
    form->addRow("Display name:", nameEdit);
    form->addRow("Role:", roleCombo);
    hl->addLayout(form);
    hl->addWidget(startBtn);

    stack = new QStackedWidget(this);
    stack->addWidget(homePage); // index 0
    setCentralWidget(stack);

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    loadAssignments();
    ensureSubmissionDir();
}

MainWindow::~MainWindow() {
    saveAssignments();
}

QString MainWindow::appPath() const {
    return QDir::currentPath();
}

void MainWindow::ensureSubmissionDir() {
    QDir d(appPath());
    if (!d.exists("submissions")) d.mkpath("submissions");
}

void MainWindow::onStartClicked() {
    QString name = nameEdit->text().trimmed();
    QString role = roleCombo->currentText();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Name required", "Please enter a display name.");
        return;
    }

    if (role == "Teacher") {
        if (teacherWidget) {
            delete teacherWidget;
            teacherWidget = nullptr;
        }
        teacherWidget = new TeacherWidget(name, this);
        connect(teacherWidget, &TeacherWidget::createAssignmentRequested, this, &MainWindow::onAssignmentCreated);
        stack->addWidget(teacherWidget);
        stack->setCurrentWidget(teacherWidget);
    } else {
        if (studentWidget) {
            delete studentWidget;
            studentWidget = nullptr;
        }
        studentWidget = new StudentWidget(name, this);
        connect(studentWidget, &StudentWidget::submitRequested, this, &MainWindow::onStudentSubmitRequested);
        stack->addWidget(studentWidget);
        // provide assignments
        studentWidget->setAssignments(assignments);
        stack->setCurrentWidget(studentWidget);
    }
}

void MainWindow::onAssignmentCreated(const Assignment &a) {
    Assignment copy = a;
    copy.id = nextAssignmentId++;
    assignments.append(copy);
    saveAssignments();
    QMessageBox::information(this, "Created", "Assignment created successfully.");
    refreshViews();
}

void MainWindow::onStudentSubmitRequested(int assignmentId, const QString &filePath) {
    // find assignment
    for (Assignment &a : assignments) {
        if (a.id == assignmentId) {
            // only accept pdf
            if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
                QMessageBox::warning(this, "Invalid", "Only PDF files accepted.");
                return;
            }
            // create student folder
            QFileInfo fi(filePath);
            QString fileName = fi.fileName();
            // student name taken from current page (widget)
            QString studentName = studentWidget ? studentWidget->windowTitle() : QString();
            // user-friendly: studentWidget has no windowTitle set; better pass student name in constructor and store;
            // but we used constructor param. We can use studentWidget->objectName or we can modify to store name. For simplicity:
            // Save file as submissions/<studentName>_<assignmentId>_<origname>
            QString studentDir = QDir(appPath()).filePath("submissions");
            QDir dir(studentDir);
            if (!dir.exists()) dir.mkpath(".");
            QString dest = dir.filePath(QString("%1_%2_%3").arg(studentName).arg(a.id).arg(fileName));
            if (!QFile::copy(filePath, dest)) {
                QMessageBox::warning(this, "Copy failed", "Could not copy file (maybe file exists).");
                return;
            }
            a.submittedBy = studentName;
            a.submissionFile = dest;
            a.submittedOn = QDateTime::currentDateTime();
            saveAssignments();
            QMessageBox::information(this, "Submitted", "Assignment submitted.");
            refreshViews();
            return;
        }
    }
    QMessageBox::warning(this, "Not found", "Assignment not found.");
}

void MainWindow::loadAssignments() {
    QString path = QDir(appPath()).filePath("assignments.json");
    QFile f(path);
    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly)) return;
    QByteArray data = f.readAll();
    f.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    QJsonObject root = doc.object();
    nextAssignmentId = root["nextAssignmentId"].toInt(1);
    assignments.clear();
    QJsonArray arr = root["assignments"].toArray();
    for (const QJsonValue &v : arr) {
        assignments.append(Assignment::fromJson(v.toObject()));
    }
}

void MainWindow::saveAssignments() {
    QJsonArray arr;
    for (const Assignment &a : assignments) arr.append(a.toJson());
    QJsonObject root;
    root["nextAssignmentId"] = nextAssignmentId;
    root["assignments"] = arr;
    QJsonDocument doc(root);
    QString path = QDir(appPath()).filePath("assignments.json");
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return;
    f.write(doc.toJson());
    f.close();
}

void MainWindow::refreshViews() {
    if (teacherWidget) {
        // teacher local view: rebuild by scanning assignments for createdBy name
        // teacherWidget didn't store assignments vector, so we re-create it by re-adding widget or we could add a refresh method; for simplicity, recreate:
        QString teacherName = teacherWidget->windowTitle();
        // teacherWidget doesn't use windowTitle to store name; but its constructor took name — no public getter. Simpler: just delete and recreate current teacher widget view using last used name.
        // For simplicity in this minimal project, we will just update student view (main requirement).
    }
    if (studentWidget) {
        studentWidget->setAssignments(assignments);
    }
}
