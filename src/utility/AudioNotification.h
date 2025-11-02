#ifndef OPENMITTSU_UTILITY_AUDIONOTIFICATION_H_
#define OPENMITTSU_UTILITY_AUDIONOTIFICATION_H_

#include <QAudioOutput>
#include <QFile>
#include <QObject>
#include <QtVersionChecks>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioSink>
#endif

#include <memory>

namespace openmittsu {
	namespace utility {

		class AudioNotification : public QObject {
			Q_OBJECT
		public:
			AudioNotification();
			virtual ~AudioNotification();

		public slots:
			void playNotification();
		private slots:
			void audioOutputOnStateChanged(QAudio::State state);
		private:
			// Audio playing
			QFile receivedMessageAudioFile;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			std::unique_ptr<QAudioSink> audioSink;
#else
			std::unique_ptr<QAudioOutput> audioOutput;
#endif
		};

	}
}

#endif // OPENMITTSU_UTILITY_AUDIONOTIFICATION_H_
