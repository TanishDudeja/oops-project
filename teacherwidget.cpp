#include "teacherwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>

TeacherWidget::TeacherWidget(const QString &teacherName, QWidget *parent)
    : QWidget(parent), teacher(teacherName)
{
    auto *mainLay = new QVBoxLayout(this);

    auto *titleLabel = new QLabel(QString("Teacher: <b>%1</b>").arg(teacher));
    mainLay->addWidget(titleLabel);

    auto *form = new QFormLayout();
    titleEdit = new QLineEdit();
    descEdit = new QTextEdit();
    deadlineEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(7));
    deadlineEdit->setCalendarPopup(true);

    form->addRow("Title:", titleEdit);
    form->addRow("Description:", descEdit);
    form->addRow("Deadline:", deadlineEdit);
    mainLay->addLayout(form);

    createBtn = new QPushButton("Create Assignment");
    mainLay->addWidget(createBtn);

    mainLay->addWidget(new QLabel("Created assignments (view from main list):"));
    listWidget = new QListWidget();
    mainLay->addWidget(listWidget);

    connect(createBtn, &QPushButton::clicked, this, [=]() {
        QString t = titleEdit->text().trimmed();
        QString d = descEdit->toPlainText().trimmed();
        QDateTime ddl = deadlineEdit->dateTime();

        if (t.isEmpty()) {
            QMessageBox::warning(this, "Validation", "Title required");
            return;
        }

        Assignment a;
        a.title = t;
        a.description = d;
        a.deadline = ddl;
        a.createdBy = teacher;
        emit createAssignmentRequested(a);

        titleEdit->clear();
        descEdit->clear();
        deadlineEdit->setDateTime(QDateTime::currentDateTime().addDays(7));
    });
}

void TeacherWidget::refreshLocalList(const QVector<Assignment> &assignments) {
    listWidget->clear();
    for (const Assignment &a : assignments) {
        if (a.createdBy == teacher) {
            QString line = QString("[%1] %2 | Due: %3")
            .arg(a.id).arg(a.title).arg(a.deadline.toString("yyyy-MM-dd HH:mm"));
            listWidget->addItem(line);
        }
    }
}
