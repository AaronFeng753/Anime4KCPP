#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "communicator.h"
#include "Anime4KCPP.h"

#include <QMainWindow>
#include <QTranslator>
#include <QCloseEvent>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>
#include <QClipboard>
#include <QSettings>
#include <QDesktopServices>
#include <QInputDialog>

#include <opencv2/opencv.hpp>

#define ANIME4KCPP_GUI_VERSION "1.11.0"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum Language
{
    en = 0, zh_cn = 1
};

enum ErrorType
{
    PROCESSING_LIST_EMPTY,
    FILE_NOT_EXIST, DIR_NOT_EXIST,
    TYPE_NOT_IMAGE, TYPE_NOT_ADD,
    URL_INVALID, ERROR_IMAGE_FORMAT
};

enum FileType
{
    IMAGE = 0, VIDEO = 1, GIF = 2, ERROR_TYPE = 3
};

enum GPUMode
{
    GPUMODE_INITIALZED = 0, GPUMODE_UNINITIALZED = 1, GPUMODE_UNSUPPORT = 3
};

enum GPUCNNMode
{
    GPUCNNMODE_INITIALZED = 0, GPUCNNMODE_UNINITIALZED = 1, GPUCNNMODE_UNSUPPORT = 3
};

enum PauseFlag
{
    NORMAL, PAUSE, PAUSED, CONTINUE
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private:
    void readConfig(const QSettings* conf);
    void writeConfig(QSettings* conf);
    Language getLanguage(const QString& lang);
    QString getLanguage(const Language lang);
    void errorHandler(const ErrorType err);
    void errorHandler(const QString& err);
    void initTextBrowser();
    bool checkFFmpeg();
    QString formatSuffixList(const QString&& type, QString str);
    void initAnime4K(Anime4KCPP::Anime4K*& anime4K);
    void releaseAnime4K(Anime4KCPP::Anime4K*& anime4K);
    FileType fileType(const QFileInfo& file);
    QString getOutputPrefix();
    Anime4KCPP::CODEC getCodec(const QString& codec);
    bool checkGIF(const QString& file);
    bool mergeAudio2Video(const QString& dstFile, const QString& srcFile, const QString& tmpFile);
    bool video2GIF(const QString& srcFile, const QString& dstFile);

private slots:
    void solt_done_renewState(int row, double pro, quint64 time);
    void solt_error_renewState(int row, QString err);
    void solt_allDone_remindUser();
    void solt_showInfo_renewTextBrowser(std::string info);
    void solt_updateProgress_updateCurrentTaskProgress(double v, double elpsed, double remaining);

private slots:
    void on_actionQuit_triggered();

    void on_pushButtonPickFiles_clicked();

    void on_pushButtonOutputPathPick_clicked();

    void on_pushButtonWebVideo_clicked();

    void on_pushButtonClear_clicked();

    void on_pushButtonDelete_clicked();

    void on_radioButtonFast_clicked();

    void on_radioButtonBalance_clicked();

    void on_radioButtonQuality_clicked();

    void on_checkBoxEnablePreprocessing_stateChanged(int arg1);

    void on_checkBoxEnablePostprocessing_stateChanged(int arg1);

    void on_pushButtonPreview_clicked();

    void on_pushButtonPreviewPick_clicked();

    void on_pushButtonStart_clicked();

    void on_actionAbout_triggered();

    void on_tabWidgetMain_tabBarClicked(int index);

    void on_actionChinese_triggered();

    void on_actionEnglish_triggered();

    void on_actionSet_FFmpeg_path_triggered();

    void on_pushButtonClearText_clicked();

    void on_spinBoxFontSize_valueChanged(int arg1);

    void on_fontComboBox_currentFontChanged(const QFont& f);

    void on_pushButtonCopyText_clicked();

    void on_pushButtonPreviewOrgin_clicked();

    void on_pushButtonPreviewOnlyResize_clicked();

    void on_pushButtonPickFolder_clicked();

    void on_checkBoxGPUMode_stateChanged(int arg1);

    void on_actionList_GPUs_triggered();

    void on_pushButtonListGPUs_clicked();

    void on_spinBoxPlatformID_valueChanged(int arg1);

    void on_pushButtonOutputPathOpen_clicked();

    void on_pushButtonReleaseGPU_clicked();

    void on_checkBoxACNet_stateChanged(int arg1);

    void on_checkBoxACNetGPU_stateChanged(int arg1);

    void on_pushButtonForceStop_clicked();

    void on_pushButtonPause_clicked();

    void on_pushButtonContinue_clicked();

private:
    Ui::MainWindow* ui;
    QTranslator* translator;
    QStandardItemModel* tableModel;
    QSettings* config;
    quint64 totalTime;
    int imageCount;
    int videoCount;
    bool ffmpeg;
    QString ffmpegPath;
    unsigned int totalTaskCount;
    Language currLanguage;

    GPUMode GPU;
    GPUCNNMode GPUCNN;
    Anime4KCPP::Anime4KCreator anime4KCreator;

    std::vector<int> devices;
    int platforms;

    QHash<QString, Language> languageSelector;
    QHash<QString, Anime4KCPP::CODEC> codecSelector;

    std::atomic<bool> stop;
    std::atomic<PauseFlag> pause;
};
#endif // MAINWINDOW_H
