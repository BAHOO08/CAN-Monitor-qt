#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "CanTreeModel.h"
#include "CanAdapter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onCustomContextMenu(const QPoint &point);
    void on_pushButton_clicked();

    void on_actionAdd_Group_triggered();
    void tickTimerTimeout();

private:
    Ui::MainWindow *ui;
    CanTreeModel *model;

    struct{
        QModelIndex index;
    }m_contextMenuContext;

    CanAdapter * m_canAdapter;
    QTimer m_tickTimer;

};

#endif // MAINWINDOW_H