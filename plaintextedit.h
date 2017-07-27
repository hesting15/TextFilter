#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H
#include <QPlainTextEdit>

class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit PlainTextEdit(QWidget *parent = Q_NULLPTR);

protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // PLAINTEXTEDIT_H
