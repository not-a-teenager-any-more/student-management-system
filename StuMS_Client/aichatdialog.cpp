#include "aichatdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>
#include <QHttpMultiPart>
#include <QSslConfiguration>
#include <QNetworkProxy>
#include <QLabel>

AIChatDialog::AIChatDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();

    // 1. 网络环境强力修复 (SSL + Proxy绕过)
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    QSslConfiguration::setDefaultConfiguration(config);

    m_netManager = new QNetworkAccessManager(this);
    m_netManager->setProxy(QNetworkProxy::NoProxy); // 🔥 解决 ProxyIE 拦截

    connect(m_netManager, &QNetworkAccessManager::finished, this, &AIChatDialog::onNetworkReply);

    // 2. 初始化播放器
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(1.0); // 音量拉满

    initTools();
    initConversation();

    appendMessage("system", "你好！我是全能语音助手。<br>"
                            "<b>按住按钮</b>说话，我会听、会想、还会说。<br>"
                            "例如：<i>“查询张三”</i> -> <i>“把他删掉”</i>。");
}

AIChatDialog::~AIChatDialog() {
    if(m_audioSource) {
        m_audioSource->stop();
        delete m_audioSource;
    }
}

void AIChatDialog::setupUi() {
    // 1. 窗口整体设置
    setWindowTitle("DeepSeek 智能语音助手");
    resize(500, 750); // 调整为更像手机/聊天窗口的比例

    // 设置全局背景色 (淡雅灰)
    this->setStyleSheet(R"(
        QDialog {
            background-color: #f5f7fa;
            font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;
        }
        /* 滚动条美化 */
        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 6px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #cbd5e0;
            min-height: 20px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical:hover {
            background: #a0aec0;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 去除边缘，让 header 顶格
    mainLayout->setSpacing(0);

    // ================== 1. 顶部 Header (白色 + 阴影) ==================
    QFrame *headerFrame = new QFrame(this);
    headerFrame->setFixedHeight(60);
    headerFrame->setStyleSheet(R"(
        QFrame {
            background-color: white;
            border-bottom: 1px solid #e2e8f0;
        }
    )");

    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 0, 20, 0);

    QLabel *iconLabel = new QLabel("🤖", this);
    iconLabel->setStyleSheet("font-size: 24px;");

    QLabel *titleLabel = new QLabel("DeepSeek AI", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: 800; color: #2d3748; margin-left: 8px;");

    QLabel *statusLabel = new QLabel("Online", this); // 装饰用
    statusLabel->setStyleSheet("font-size: 12px; color: #48bb78; font-weight: bold; margin-left: 5px; margin-top: 4px;");

    // 清除按钮 (扁平化图标风格)
    m_btnClear = new QPushButton("🗑️", this);
    m_btnClear->setFixedSize(36, 36);
    m_btnClear->setCursor(Qt::PointingHandCursor);
    m_btnClear->setToolTip("清空对话历史");
    m_btnClear->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border-radius: 6px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #fff5f5;
            border: 1px solid #fed7d7;
        }
        QPushButton:pressed {
            background-color: #feb2b2;
        }
    )");

    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(statusLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_btnClear);

    // ================== 2. 聊天内容区 ==================
    m_chatDisplay = new QTextBrowser(this);
    m_chatDisplay->setOpenExternalLinks(false);
    // 关键：设置背景透明，显示父窗口的灰色，且去除边框
    m_chatDisplay->setStyleSheet("QTextBrowser { background-color: transparent; border: none; padding: 20px; }");
    m_chatDisplay->setFrameShape(QFrame::NoFrame);

    // ================== 3. 底部输入区 (悬浮卡片风格) ==================
    QWidget *bottomContainer = new QWidget(this);
    bottomContainer->setStyleSheet("QWidget { background-color: transparent; }");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomContainer);
    bottomLayout->setContentsMargins(20, 10, 20, 20); // 留出边缘

    // 白色圆角背景容器
    QFrame *inputFrame = new QFrame(this);
    inputFrame->setStyleSheet(R"(
        QFrame {
            background-color: white;
            border-radius: 24px; /* 胶囊形状 */
            border: 1px solid #e2e8f0;
        }
    )");
    // 给容器加阴影 (注意：Qt 阴影通常需要 QGraphicsEffect，这里用 border 模拟简洁效果)

    QHBoxLayout *inputInnerLayout = new QHBoxLayout(inputFrame);
    inputInnerLayout->setContentsMargins(10, 5, 10, 5);
    inputInnerLayout->setSpacing(10);

    // 输入框
    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setPlaceholderText("聊点什么...");
    m_inputEdit->setStyleSheet(R"(
        QLineEdit {
            border: none;
            background: transparent;
            font-size: 14px;
            color: #2d3748;
            padding: 5px;
        }
    )");

    // 语音按钮 (圆形)
    m_btnVoice = new QPushButton("🎙️", this);
    m_btnVoice->setFixedSize(40, 40);
    m_btnVoice->setCursor(Qt::PointingHandCursor);
    m_btnVoice->setToolTip("按住 说话");
    m_btnVoice->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);
            color: white;
            border-radius: 20px;
            border: none;
            font-size: 18px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #5a67d8, stop:1 #6b46c1);
            margin-top: -2px; /* 悬浮微动 */
        }
        QPushButton:pressed {
            background: #4c51bf;
            margin-top: 1px;
        }
    )");

    // 发送按钮 (圆形)
    m_btnSend = new QPushButton("发送", this);    // 默认显示文字
    m_btnSend->setFixedSize(90, 40);              // 宽度改为 90 以容纳“思考中...”
    m_btnSend->setCursor(Qt::PointingHandCursor);
    m_btnSend->setToolTip("发送消息");

    // 优化样式表：蓝色胶囊按钮
    m_btnSend->setStyleSheet(R"(
        QPushButton {
            background-color: #3182ce;    /* 蓝色背景 */
            color: white;                 /* 白色文字 */
            border-radius: 20px;          /* 圆角保持高度的一半 (40/2=20)，形成胶囊形状 */
            border: none;
            font-size: 14px;              /* 字体大小适配中文 */
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2b6cb0;    /* 悬浮变深蓝 */
        }
        QPushButton:pressed {
            background-color: #2c5282;    /* 按下更深 */
            padding-top: 2px;             /* 按下时文字微动 */
        }
        QPushButton:disabled {
            background-color: #e2e8f0;    /* 禁用时变灰 */
            color: #a0aec0;
        }
    )");
    inputInnerLayout->addWidget(m_inputEdit);
    inputInnerLayout->addWidget(m_btnVoice);
    inputInnerLayout->addWidget(m_btnSend);

    bottomLayout->addWidget(inputFrame);

    // 组装主界面
    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(m_chatDisplay, 1); // 1 伸缩因子，占据中间
    mainLayout->addWidget(bottomContainer);

    // ================== 连接信号 ==================
    connect(m_btnSend, &QPushButton::clicked, this, &AIChatDialog::onSend);
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &AIChatDialog::onSend);
    connect(m_btnVoice, &QPushButton::pressed, this, &AIChatDialog::onVoicePressed);
    connect(m_btnVoice, &QPushButton::released, this, &AIChatDialog::onVoiceReleased);
    connect(m_btnClear, &QPushButton::clicked, this, &AIChatDialog::onClearHistory);
}
void AIChatDialog::initTools() {
    QJsonObject queryTool; queryTool["type"] = "function"; queryTool["function"] = QJsonObject{{"name", "query_students"}, {"description", "查询学生"}, {"parameters", QJsonObject{{"type", "object"}, {"properties", QJsonObject{{"keyword", QJsonObject{{"type", "string"}, {"description", "关键词"}}}}}, {"required", QJsonArray{"keyword"}}}}};
    QJsonObject delTool; delTool["type"] = "function"; delTool["function"] = QJsonObject{{"name", "delete_student"}, {"description", "删除学生。如果用户只提供了姓名，直接填入 name 字段，禁止先查询！"}, {"parameters", QJsonObject{{"type", "object"}, {"properties", QJsonObject{{"student_id", QJsonObject{{"type", "string"}}}, {"name", QJsonObject{{"type", "string"}}}}}, {"required", QJsonArray{}}}}};
    QJsonObject addTool; addTool["type"] = "function"; addTool["function"] = QJsonObject{{"name", "add_student"}, {"description", "添加新学生"}, {"parameters", QJsonObject{{"type", "object"}, {"properties", QJsonObject{{"name", QJsonObject{{"type", "string"}}}, {"student_id", QJsonObject{{"type", "string"}}}, {"gender", QJsonObject{{"type", "string"}}}, {"age", QJsonObject{{"type", "integer"}}}, {"department", QJsonObject{{"type", "string"}}}}}, {"required", QJsonArray{"name"}}}}};
    m_tools.append(queryTool); m_tools.append(delTool); m_tools.append(addTool);
}

void AIChatDialog::initConversation() {
    m_history = QJsonArray();
    QJsonDocument doc(m_studentData);
    QString jsonCtx = doc.toJson(QJsonDocument::Compact);
    if(jsonCtx.length() > 50000) jsonCtx = jsonCtx.left(50000);
    QJsonObject sysMsg; sysMsg["role"] = "system";
    sysMsg["content"] = "你是一个学生管理系统AI。表格数据如下：\n" + jsonCtx + "\n\n规则：1.用户说“删除XXX”且只给名字，直接调 delete_student(name='XXX')。";
    m_history.append(sysMsg);
}

void AIChatDialog::setContextData(const QJsonArray &data) { m_studentData = data; initConversation(); }
void AIChatDialog::setDeepSeekApiKey(const QString &key) { m_deepSeekKey = key; }
void AIChatDialog::setSttApiKey(const QString &key) { m_sttKey = key; }
void AIChatDialog::onClearHistory() { initConversation(); m_chatDisplay->clear(); appendMessage("system", "记忆已重置"); }

void AIChatDialog::appendMessage(const QString &role, const QString &text) {
    QString color = (role == "user") ? "#007bff" : "#28a745";
    QString name = (role == "user") ? "你" : "DeepSeek";
    m_chatDisplay->append(QString("<div style='margin-bottom:12px'><b style='color:%1'>%2:</b><br>%3</div>").arg(color, name, text));
    m_chatDisplay->verticalScrollBar()->setValue(m_chatDisplay->verticalScrollBar()->maximum());
}

// ==================== 1. 录音逻辑 (Intel 修复 + 静音检测) ====================

void AIChatDialog::onVoicePressed() {
    if (m_sttKey.isEmpty()) {
        QMessageBox::warning(this, "配置错误", "请先设置硅基流动 API Key。");
        return;
    }

     m_inputEdit->setPlaceholderText("正在聆听... (请松开按钮结束)");



    // 🔥 视觉反馈：变为红色，表示正在录音 🔥
    m_btnVoice->setStyleSheet(R"(
        QPushButton {
            background-color: #f56565; /* 红色 */
            color: white;
            border-radius: 20px;
            border: 4px solid #fed7d7; /* 外发光效果 */
            font-size: 18px;
        }
    )");


    // 1. 暴力查找 Intel 麦克风
    QAudioDevice targetDevice = QMediaDevices::defaultAudioInput();
    for (const QAudioDevice &device : QMediaDevices::audioInputs()) {
        QString name = device.description();
        // 跳过立体声混音
        if (name.contains("Stereo Mix") || name.contains("立体声混音")) continue;

        // 优先锁定 Intel 麦克风
        if (name.contains("Microphone") || name.contains("麦克风") || name.contains("阵列")) {
            targetDevice = device;
            qDebug() << ">>> 锁定麦克风设备:" << name;
            break;
        }
    }

    if (targetDevice.isNull()) {
        QMessageBox::warning(this, "错误", "未找到麦克风！");
        return;
    }

    // 2. 🔥 Intel 专用配置：强制 48000Hz + Int16 🔥
    QAudioFormat format;
    format.setSampleRate(48000); // Intel 智音通常必须用 48000
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16); // 强制 Int16 避免噪音

    // 如果不支持强制配置，尝试自动适配
    if (!targetDevice.isFormatSupported(format)) {
        qDebug() << "设备不支持 48k Int16，尝试自动适配...";
        format = targetDevice.preferredFormat();
        // 再次强制改为 Int16 (关键!)
        format.setSampleFormat(QAudioFormat::Int16);
    }

    qDebug() << "最终录音格式: Rate=" << format.sampleRate()
             << " Format=" << format.sampleFormat();

    m_audioBuffer.close();
    m_audioBuffer.setData(QByteArray());
    m_audioBuffer.open(QIODevice::WriteOnly);

    m_audioSource = new QAudioSource(targetDevice, format, this);
    m_audioSource->setVolume(1.0); // 软件层面拉满音量
    m_audioSource->start(&m_audioBuffer);
}
void AIChatDialog::onVoiceReleased() {

    // 🔥 恢复视觉状态：变回渐变紫 🔥
    m_btnVoice->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);
            color: white;
            border-radius: 20px;
            border: none;
            font-size: 18px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #5a67d8, stop:1 #6b46c1);
            margin-top: -2px;
        }
    )");
    m_inputEdit->setPlaceholderText("聊点什么...");
    if (!m_audioSource) return;

    m_audioSource->stop();
    QAudioFormat actualFormat = m_audioSource->format();
    delete m_audioSource;
    m_audioSource = nullptr;
    m_audioBuffer.close();

    QByteArray pcmData = m_audioBuffer.data();
    int dataSize = pcmData.size();

    // --- 🔥 调试核心：计算录音振幅 🔥 ---
    long long totalAmp = 0;
    int sampleCount = dataSize / 2; // Int16 是 2 字节一个样本
    const qint16 *samples = reinterpret_cast<const qint16*>(pcmData.constData());

    for (int i = 0; i < sampleCount; i++) {
        totalAmp += std::abs(samples[i]);
    }
    double avgAmp = (sampleCount > 0) ? (totalAmp / (double)sampleCount) : 0;

    qDebug() << ">>> 录音结束，数据大小:" << dataSize << " 平均振幅:" << avgAmp;

    // 阈值判断 (Intel 麦克风正常说话通常 > 500)
    if (avgAmp < 100) {
        // 振幅太小，说明全是静音
        appendMessage("system", QString("检测到静音 (振幅 %.1f)。\n请在 Windows 设置里调大麦克风音量！").arg(avgAmp));
        // 这里不 return，强行发出去试试，但大概率没用
    }
    // ------------------------------------

    if (dataSize < 4000) {
        appendMessage("system", "说话时间太短，请按住久一点。");
        return;
    }

    appendMessage("system", "正在识别...");

    // 生成 WAV 头 (务必传入 actualFormat)
    QByteArray wavData = addWavHeader(pcmData, actualFormat);

    // 发送请求 (保持不变)
    QNetworkRequest request(QUrl("https://api.siliconflow.cn/v1/audio/transcriptions"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_sttKey).toUtf8());

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart modelPart;
    modelPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"model\""));
    modelPart.setBody("FunAudioLLM/SenseVoiceSmall");
    multiPart->append(modelPart);

    QHttpPart langPart;
    langPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"language\""));
    langPart.setBody("zh");
    multiPart->append(langPart);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"voice.wav\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("audio/wav"));
    filePart.setBody(wavData);
    multiPart->append(filePart);

    disconnect(m_netManager, &QNetworkAccessManager::finished, this, &AIChatDialog::onNetworkReply);
    QNetworkReply *reply = m_netManager->post(request, multiPart);
    multiPart->setParent(reply);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        this->onSttReply(reply);
        connect(m_netManager, &QNetworkAccessManager::finished, this, &AIChatDialog::onNetworkReply);
    });
}

void AIChatDialog::onSttReply(QNetworkReply *reply) {
    if (reply->error()) { appendMessage("system", "网络错误: " + reply->errorString()); return; }
    QJsonObject resp = QJsonDocument::fromJson(reply->readAll()).object();
    reply->deleteLater();
    if (resp.contains("text")) {
        QString text = resp["text"].toString().trimmed();
        if(text.isEmpty() || text == "." || text == "Yeah.") { appendMessage("system", "未听清"); return; }
        m_inputEdit->setText(text);
        onSend();
    }
}

QByteArray AIChatDialog::addWavHeader(const QByteArray &pcmData, const QAudioFormat &format) {
    QByteArray header; int r=format.sampleRate(), c=format.channelCount(), b=16, d=pcmData.size();
    header.append("RIFF"); int t=d+36; header.append((char*)&t, 4); header.append("WAVEfmt ");
    int s=16; header.append((char*)&s, 4); short f=1; header.append((char*)&f, 2);
    header.append((char*)&c, 2); header.append((char*)&r, 4); int br=r*c*b/8; header.append((char*)&br, 4);
    short a=c*b/8; header.append((char*)&a, 2); short bs=b; header.append((char*)&bs, 2);
    header.append("data"); header.append((char*)&d, 4); return header + pcmData;
}

// ==================== 2. TTS 说话逻辑 ====================

void AIChatDialog::speak(const QString &text) {
    if (m_sttKey.isEmpty()) return;
    QString cleanText = text; cleanText.remove("**"); cleanText.remove("#");

    QNetworkRequest request(QUrl("https://api.siliconflow.cn/v1/audio/speech"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_sttKey).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload; payload["model"] = "fishaudio/fish-speech-1.5";
    payload["input"] = cleanText; payload["voice"] = "fishaudio/fish-speech-1.5:alex"; payload["response_format"] = "mp3";

    disconnect(m_netManager, &QNetworkAccessManager::finished, this, &AIChatDialog::onNetworkReply);
    QNetworkReply *reply = m_netManager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply](){ this->onTtsReply(reply); connect(m_netManager, &QNetworkAccessManager::finished, this, &AIChatDialog::onNetworkReply); });
}

void AIChatDialog::onTtsReply(QNetworkReply *reply) {
    if (!reply->error()) {
        QByteArray data = reply->readAll();
        if(!data.isEmpty()) {
            QTemporaryFile *temp = new QTemporaryFile(this);
            temp->setFileTemplate("tts_XXXXXX.mp3");
            if (temp->open()) {
                temp->write(data); temp->flush(); temp->close();
                m_player->setSource(QUrl::fromLocalFile(temp->fileName()));
                m_player->play();
            }
        }
    }
    reply->deleteLater();
}

// ==================== 3. DeepSeek 对话逻辑 (含多轮对话修复) ====================

void AIChatDialog::onSend() {
    QString question = m_inputEdit->text().trimmed();
    if (question.isEmpty()) return;
    appendMessage("user", question); m_inputEdit->clear(); m_inputEdit->setEnabled(false); m_btnSend->setText("思考中...");

    // 🔥 修复多轮对话链条：如果是工具调用后续，必须补一个 tool 结果 🔥
    if (!m_history.isEmpty()) {
        QJsonObject last = m_history.last().toObject();
        if (last["role"] == "assistant" && last.contains("tool_calls")) {
            QJsonArray calls = last["tool_calls"].toArray();
            for (auto v : calls) {
                QJsonObject resp; resp["role"]="tool"; resp["tool_call_id"]=v.toObject()["id"]; resp["content"]="success";
                m_history.append(resp);
            }
        }
    }

    QJsonObject userMsg; userMsg["role"] = "user"; userMsg["content"] = question;
    m_history.append(userMsg);

    QJsonObject payload; payload["model"] = "deepseek-chat"; payload["messages"] = m_history; payload["tools"] = m_tools; payload["tool_choice"] = "auto";
    QNetworkRequest request(QUrl("https://api.deepseek.com/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_deepSeekKey).toUtf8());
    m_netManager->post(request, QJsonDocument(payload).toJson());
}

void AIChatDialog::onNetworkReply(QNetworkReply *reply) {
    m_inputEdit->setEnabled(true); m_btnSend->setText("发送"); m_inputEdit->setFocus();
    if (reply->error()) {
        appendMessage("system", "Error: " + reply->errorString());
        if(!m_history.isEmpty() && m_history.last().toObject()["role"] == "user") m_history.removeAt(m_history.size()-1);
        reply->deleteLater(); return;
    }

    QJsonObject resp = QJsonDocument::fromJson(reply->readAll()).object();
    reply->deleteLater();

    if (resp.contains("choices")) {
        QJsonArray choices = resp["choices"].toArray();
        if (!choices.isEmpty()) {
            QJsonObject msg = choices[0].toObject()["message"].toObject();
            m_history.append(msg);

            if (msg.contains("tool_calls")) {
                speak("正在执行操作。"); // 播报
                QJsonArray toolCalls = msg["tool_calls"].toArray();
                for (const auto &val : toolCalls) {
                    QJsonObject func = val.toObject()["function"].toObject();
                    QString funcName = func["name"].toString();
                    QJsonObject args = QJsonDocument::fromJson(func["arguments"].toString().toUtf8()).object();
                    appendMessage("system", "正在执行: " + funcName);
                    emit functionCallRequested(funcName, args);
                }
            } else if (msg.contains("content")) {
                QString content = msg["content"].toString();
                if(!content.isEmpty()) {
                    content.replace("\n", "<br>");
                    appendMessage("assistant", content);
                    speak(msg["content"].toString()); // 播报回答
                }
            }
        }
    }
}
