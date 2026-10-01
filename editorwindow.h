#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include "displayfile.h"

class EditorWindow : public QMainWindow
{
    Q_OBJECT

private:
    DisplayFile displayFile;
    void carregarCenaTeste();

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow() override;
};
#endif // EDITORWINDOW_H