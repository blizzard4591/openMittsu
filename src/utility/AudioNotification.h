#ifndef OPENMITTSU_UTILITY_AUDIONOTIFICATION_H_
#define OPENMITTSU_UTILITY_AUDIONOTIFICATION_H_

#include <QAudioOutput>
#include <QFile>
#include <QObject>

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
			std::unique_ptr<QAudioOutput> audioOutput;
		};

	}
}

#endif // OPENMITTSU_UTILITY_AUDIONOTIFICATION_H_
