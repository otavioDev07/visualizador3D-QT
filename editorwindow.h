#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>

class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow() override;
};
#endif // EDITORWINDOW_H
