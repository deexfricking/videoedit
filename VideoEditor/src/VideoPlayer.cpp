#include "VideoPlayer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QSizePolicy>
#include <QFrame>
#include <QDebug>

VideoPlayer::VideoPlayer(QWidget* parent)
    : QWidget(parent)
{
    m_player = new QMediaPlayer(this);
    m_audio  = new QAudioOutput(this);
    m_player->setAudioOutput(m_audio);

    buildUi();
    m_player->setVideoOutput(m_videoWidget);

    connect(m_player, &QMediaPlayer::positionChanged,
            this,     &VideoPlayer::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged,
            this,     &VideoPlayer::onDurationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this,     &VideoPlayer::onPlaybackStateChanged);
}

void VideoPlayer::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Video surface ─────────────────────────────────────────────────────────
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_videoWidget->setStyleSheet("background:#000;");
    m_videoWidget->setMinimumSize(320, 180);
    root->addWidget(m_videoWidget);

    // ── Seek slider ───────────────────────────────────────────────────────────
    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setRange(0, 1000);
    m_seekSlider->setValue(0);
    m_seekSlider->setStyleSheet(R"(
        QSlider::groove:horizontal { height:4px; background:#333; border-radius:2px; }
        QSlider::handle:horizontal { width:12px; height:12px; background:#4a9eff;
                                     border-radius:6px; margin:-4px 0; }
        QSlider::sub-page:horizontal { background:#4a9eff; border-radius:2px; }
    )");
    connect(m_seekSlider, &QSlider::sliderMoved,  this, &VideoPlayer::onSliderMoved);
    connect(m_seekSlider, &QSlider::sliderPressed, [this]{ m_seeking = true; });
    connect(m_seekSlider, &QSlider::sliderReleased,[this]{
        m_seeking = false;
        if (m_player->duration() > 0) {
            qint64 pos = qint64(m_seekSlider->value()) *
                         m_player->duration() / 1000;
            m_player->setPosition(pos);
        }
    });
    root->addWidget(m_seekSlider);

    // ── Controls row ──────────────────────────────────────────────────────────
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setContentsMargins(6, 4, 6, 4);
    ctrlRow->setSpacing(4);

    auto makeBtn = [&](const QStyle::StandardPixmap sp,
                       const QString& tip) -> QPushButton* {
        auto* b = new QPushButton(this);
        b->setIcon(style()->standardIcon(sp));
        b->setToolTip(tip);
        b->setFixedSize(32, 28);
        b->setFlat(true);
        return b;
    };

    m_btnStepBack = makeBtn(QStyle::SP_MediaSeekBackward, tr("Step Back (←)"));
    m_btnPlay     = makeBtn(QStyle::SP_MediaPlay,         tr("Play / Pause (Space)"));
    m_btnStop     = makeBtn(QStyle::SP_MediaStop,         tr("Stop"));
    m_btnStepFwd  = makeBtn(QStyle::SP_MediaSeekForward,  tr("Step Forward (→)"));

    connect(m_btnPlay,     &QPushButton::clicked, this, &VideoPlayer::togglePlayPause);
    connect(m_btnStop,     &QPushButton::clicked, this, &VideoPlayer::stop);
    connect(m_btnStepBack, &QPushButton::clicked, this, &VideoPlayer::stepBackward);
    connect(m_btnStepFwd,  &QPushButton::clicked, this, &VideoPlayer::stepForward);

    ctrlRow->addWidget(m_btnStepBack);
    ctrlRow->addWidget(m_btnPlay);
    ctrlRow->addWidget(m_btnStop);
    ctrlRow->addWidget(m_btnStepFwd);

    // Time label
    m_timeLabel = new QLabel("00:00:00 / 00:00:00", this);
    m_timeLabel->setStyleSheet("color:#aaa; font-size:11px; font-family:monospace;");
    ctrlRow->addWidget(m_timeLabel);

    ctrlRow->addStretch();

    // Volume
    ctrlRow->addWidget(new QLabel("🔊", this));
    m_volSlider = new QSlider(Qt::Horizontal, this);
    m_volSlider->setRange(0, 100);
    m_volSlider->setValue(80);
    m_volSlider->setFixedWidth(80);
    m_volSlider->setToolTip(tr("Volume"));
    connect(m_volSlider, &QSlider::valueChanged, this, &VideoPlayer::setVolume);
    ctrlRow->addWidget(m_volSlider);
    setVolume(80);

    root->addLayout(ctrlRow);
}

// ─── Public API ───────────────────────────────────────────────────────────────

void VideoPlayer::loadMedia(const QString& filePath)
{
    m_player->setSource(QUrl::fromLocalFile(filePath));
    m_player->pause();
}

void VideoPlayer::play()
{
    m_player->play();
}

void VideoPlayer::pause()
{
    m_player->pause();
}

void VideoPlayer::stop()
{
    m_player->stop();
    m_seekSlider->setValue(0);
    updateTimeLabel();
}

void VideoPlayer::togglePlayPause()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState)
        pause();
    else
        play();
}

void VideoPlayer::seekTo(qint64 ms)
{
    m_player->setPosition(ms);
}

void VideoPlayer::stepForward()
{
    // Approximately one frame at 30fps ≈ 33ms
    seekTo(qMin(m_player->position() + 33, m_player->duration()));
}

void VideoPlayer::stepBackward()
{
    seekTo(qMax(m_player->position() - 33, qint64(0)));
}

void VideoPlayer::setVolume(int pct)
{
    if (m_audio)
        m_audio->setVolume(pct / 100.0f);
}

qint64 VideoPlayer::position()  const { return m_player->position(); }
qint64 VideoPlayer::duration()  const { return m_player->duration(); }
bool   VideoPlayer::isPlaying() const {
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

// ─── Slots ────────────────────────────────────────────────────────────────────

void VideoPlayer::onPositionChanged(qint64 pos)
{
    if (!m_seeking && m_player->duration() > 0) {
        m_seekSlider->setValue(int(pos * 1000 / m_player->duration()));
    }
    updateTimeLabel();
    emit positionChanged(pos);
}

void VideoPlayer::onDurationChanged(qint64 dur)
{
    updateTimeLabel();
    emit durationChanged(dur);
}

void VideoPlayer::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    bool playing = (state == QMediaPlayer::PlayingState);
    m_btnPlay->setIcon(style()->standardIcon(
        playing ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay));
    emit playbackStateChanged(playing);
}

void VideoPlayer::onSliderMoved(int value)
{
    if (m_player->duration() > 0) {
        qint64 pos = qint64(value) * m_player->duration() / 1000;
        updateTimeLabel();
        // Live scrub preview
        m_player->setPosition(pos);
    }
}

void VideoPlayer::updateTimeLabel()
{
    m_timeLabel->setText(formatTime(m_player->position()) +
                         " / " +
                         formatTime(m_player->duration()));
}

QString VideoPlayer::formatTime(qint64 ms) const
{
    qint64 total = ms / 1000;
    qint64 h = total / 3600;
    qint64 m = (total % 3600) / 60;
    qint64 s = total % 60;
    return QString("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));
}
