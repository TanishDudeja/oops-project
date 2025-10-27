#ifndef TEACHERWIDGET_H
#define TEACHERWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QListWidget>
#include "assignment.h"

class TeacherWidget : public QWidget {
    Q_OBJECT
public:
    explicit TeacherWidget(const QString &teacherName, QWidget *parent = nullptr);

signals:
    void createAssignmentRequested(const Assignment &a);

private:
    QString teacher;
    QLineEdit *titleEdit;
    QTextEdit *descEdit;
    QDateTimeEdit *deadlineEdit;
    QPushButton *createBtn;
    QListWidget *listWidget;

    void refreshLocalList(const QVector<Assignment> &assignments);
};

#endif // TEACHERWIDGET_H
