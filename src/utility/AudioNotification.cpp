#include "src/utility/AudioNotification.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace utility {

		AudioNotification::AudioNotification() : QObject() {
			// Load audio resources
			QAudioFormat format;
			format.setSampleRate(44100);
			format.setChannelCount(2);
			format.setSampleSize(16);
			format.setCodec("audio/pcm");
			format.setByteOrder(QAudioFormat::LittleEndian);
			format.setSampleType(QAudioFormat::SignedInt);
			audioOutput = std::make_unique<QAudioOutput>(format, this);

			OPENMITTSU_CONNECT(audioOutput.get(), stateChanged(QAudio::State), this, audioOutputOnStateChanged(QAudio::State));

			receivedMessageAudioFile.setFileName(":/audio/ReceivedMessage.wav");
			if (!receivedMessageAudioFile.open(QFile::ReadOnly)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not load audio file ReceivedMessage.wav in the Client.";
			}
		}

		AudioNotification::~AudioNotification() {
			audioOutput->stop();
			if (receivedMessageAudioFile.isOpen()) {
				receivedMessageAudioFile.close();
			}
		}

		void AudioNotification::playNotification() {
			// Do not restart audio, let it play.
			if (audioOutput->state() == QAudio::State::ActiveState) {
				LOGGER_DEBUG("Wanted to play audio for new message, but it was already active.");
				return;
			}

			// Play audio for incoming and unseen message.
			receivedMessageAudioFile.reset();
			audioOutput->start(&receivedMessageAudioFile);
		}

		void AudioNotification::audioOutputOnStateChanged(QAudio::State state) {
			switch (state) {
				case QAudio::IdleState:
					// Finished playing (no more data)
					audioOutput->stop();
					receivedMessageAudioFile.reset();
					break;
				case QAudio::StoppedState:
					// Stopped for other reasons
					receivedMessageAudioFile.reset();
					if (audioOutput->error() != QAudio::NoError) {
						LOGGER()->warn("Error while playing sound: {}", static_cast<int>(audioOutput->error()));
					}
					break;
				default:
					break;
			}
		}

	}
}
