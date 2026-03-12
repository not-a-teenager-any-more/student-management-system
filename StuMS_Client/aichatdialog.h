#ifndef AICHATDIALOG_H
#define AICHATDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QAudioSource>
#include <QBuffer>
#include <QFile>
#include <QMediaDevices>
#include <QAudioDevice>
// 🔥 播放器支持
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTemporaryFile>

class AIChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIChatDialog(QWidget *parent = nullptr);
    ~AIChatDialog();

    void setContextData(const QJsonArray &data);
    void setDeepSeekApiKey(const QString &key);
    void setSttApiKey(const QString &key);

signals:
    void functionCallRequested(const QString &functionName, const QJsonObject &args);

private slots:
    void onSend();
    void onNetworkReply(QNetworkReply *reply);
    void onClearHistory();

    // STT (听)
    void onVoicePressed();
    void onVoiceReleased();
    void onSttReply(QNetworkReply *reply);

    // TTS (说)
    void onTtsReply(QNetworkReply *reply);

private:
    void setupUi();
    void appendMessage(const QString &role, const QString &text);
    void initTools();
    void initConversation();
    void speak(const QString &text); // 说话函数
    QByteArray addWavHeader(const QByteArray &pcmData, const QAudioFormat &format);

    QTextBrowser *m_chatDisplay;
    QLineEdit *m_inputEdit;
    QPushButton *m_btnSend;
    QPushButton *m_btnVoice;
    QPushButton *m_btnClear;

    QNetworkAccessManager *m_netManager;
    QJsonArray m_studentData;
    QString m_deepSeekKey;
    QString m_sttKey;

    QJsonArray m_tools;
    QJsonArray m_history;

    QAudioSource *m_audioSource = nullptr;
    QBuffer m_audioBuffer;

    // 播放器
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
};

#endif // AICHATDIALOG_H
