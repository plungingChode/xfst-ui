#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QKeyEvent>
#include "highlighter.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void xfst();
    void save();

private slots:
    void on_btnNew_clicked();

    void on_btnLoad_clicked();

    void on_btnSaveAs_clicked();

    void on_cbQuiet_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;
    QString scriptPath;
};
#endif // MAINWINDOW_HPP
