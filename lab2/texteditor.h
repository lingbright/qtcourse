#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFile>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QTimer>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include "AboutDialog.h"
#include "SearchDialog.h"
#include "ReplaceDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TextEditor; }
QT_END_NAMESPACE

class TextEditor : public QMainWindow {
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void cutText();
    void copyText();
    void pasteText();
    void undoAction();
    void redoAction();
    void findText();
    void replaceText();
    void changeFont();
    void changeFontColor();
    void changeBackgroundColor();
    void toggleToolBar();
    void toggleStatusBar();
    void autoSave();
    void updateWindowTitle();
    void showAbout();
    void toggleLineWrap();
    void clearHighlights();
    void updateCursorPosition();
    void changeeditcolor();
    void exitApp();
private:
    void saveToFile(const QString &fileName);
    bool confirmDiscardChanges();
    Ui::TextEditor *ui;
    QTextEdit *textEdit;
    QString currentFile;
    bool isModified;
    QString lastSearchText;
    QSettings settings;
    bool searchDirectionDown;
};

#endif // TEXTEDITOR_H
