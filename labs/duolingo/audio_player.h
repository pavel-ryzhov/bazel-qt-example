#pragma once

#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>
#include <cstdint>

class AudioPlayer {
   public:
    static AudioPlayer& GetInstance() {
        static AudioPlayer player{};
        return player;
    }

    enum Sound : uint8_t { Start, Success, Error };

    void PlaySound(Sound sound) {
        player_.setSource(QUrl::fromLocalFile(GetPath(sound)));
        player_.play();
    }

   private:
    QMediaPlayer player_;

    AudioPlayer() {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        auto* output = new QAudioOutput();
        output->setVolume(100);
        player_.setAudioOutput(output);
        QObject::connect(&player_, &QMediaPlayer::errorOccurred, [](auto error) {
            qDebug() << "Playback error:" << error;
        });
    }

    [[nodiscard]] static QString GetPath(Sound sound) {
        return QString("labs/duolingo/data/") + [sound] {
            switch (sound) {
                case Start:
                    return "start";
                case Success:
                    return "success";
                case Error:
                    return "error";
            }
            return "";
        }() + ".wav";
    }
};