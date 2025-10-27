#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVector>
#include "assignment.h"
#include "teacherwidget.h"
#include "studentwidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartClicked();
    void onAssignmentCreated(const Assignment &a);
    void onStudentSubmitRequested(int assignmentId, const QString &filePath);

private:
    QWidget *homePage;
    QLineEdit *nameEdit;
    QComboBox *roleCombo;
    QPushButton *startBtn;

    QStackedWidget *stack;

    TeacherWidget *teacherWidget = nullptr;
    StudentWidget *studentWidget = nullptr;

    QVector<Assignment> assignments;
    int nextAssignmentId = 1;

    void loadAssignments();
    void saveAssignments();
    QString appPath() const;
    void refreshViews();
    void ensureSubmissionDir();
};

#endif // MAINWINDOW_H
