#ifndef __FLAT_DIALOG__
#define __FLAT_DIALOG__

#include <QDialog>

namespace Ui {
class FlatDialog;
}

class QString;

class FlatDialog : public QDialog
{
    Q_OBJECT

public:
    FlatDialog(QWidget* parent = 0);

    void setText(const QString& text);

protected:
    void keyPressEvent(QKeyEvent* event);

private slots:
    void acceptDialog(void);
    void rejectDialog(void);
    void cancelDialog(void);

private:
    Ui::FlatDialog* _ui;
};

#endif
