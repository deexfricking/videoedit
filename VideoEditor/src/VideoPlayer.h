#pragma once
#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>

// ─── VideoPlayer ──────────────────────────────────────────────────────────────
// Central preview panel — plays the source clip from the media bin, or
// renders the timeline composition at the current playhead position.

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget* parent = nullptr);

    qint64 position()   const;
    qint64 duration()   const;
    bool   isPlaying()  const;

public slots:
    void loadMedia(const QString& filePath);
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    void seekTo(qint64 ms);
    void stepForward();
    void stepBackward();
    void setVolume(int pct);            // 0-100

signals:
    void positionChanged(qint64 ms);
    void durationChanged(qint64 ms);
    void playbackStateChanged(bool playing);

private slots:
    void onPositionChanged(qint64 pos);
    void onDurationChanged(qint64 dur);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onSliderMoved(int value);
    void updateTimeLabel();

private:
    void buildUi();
    QString formatTime(qint64 ms) const;

    QMediaPlayer*  m_player      = nullptr;
    QAudioOutput*  m_audio       = nullptr;
    QVideoWidget*  m_videoWidget = nullptr;
    QSlider*       m_seekSlider  = nullptr;
    QSlider*       m_volSlider   = nullptr;
    QLabel*        m_timeLabel   = nullptr;
    QPushButton*   m_btnPlay     = nullptr;
    QPushButton*   m_btnStop     = nullptr;
    QPushButton*   m_btnStepBack = nullptr;
    QPushButton*   m_btnStepFwd  = nullptr;
    QComboBox*     m_zoomCombo   = nullptr;

    bool m_seeking = false;
};
