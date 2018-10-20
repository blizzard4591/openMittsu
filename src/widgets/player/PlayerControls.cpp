/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "src/widgets/player/PlayerControls.h"

#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>
#include <QAudio>

#include "src/utility/QObjectConnectionMacro.h"

#if defined(QT_VERSION) && (QT_VERSION < QT_VERSION_CHECK(5, 8, 0))
#include <qmath.h>
#define LOG100 4.60517018599
#endif

namespace openmittsu {
	namespace widgets {

		PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent) {
			m_playButton = new QToolButton(this);
			m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

			OPENMITTSU_CONNECT(m_playButton, clicked(bool), this, playClicked());

			m_stopButton = new QToolButton(this);
			m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
			m_stopButton->setEnabled(false);

			OPENMITTSU_CONNECT(m_stopButton, clicked(bool), this, stopClicked());

			m_muteButton = new QToolButton(this);
			m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

			OPENMITTSU_CONNECT(m_muteButton, clicked(bool), this, muteClicked());

			m_volumeSlider = new QSlider(Qt::Horizontal, this);
			m_volumeSlider->setRange(0, 100);

			OPENMITTSU_CONNECT(m_volumeSlider, valueChanged(int), this, onVolumeSliderValueChanged());

			QBoxLayout *layout = new QHBoxLayout;
			layout->setMargin(0);
			layout->addWidget(m_stopButton);
			layout->addWidget(m_playButton);
			layout->addWidget(m_muteButton);
			layout->addWidget(m_volumeSlider);
			setLayout(layout);
		}

		QMediaPlayer::State PlayerControls::state() const {
			return m_playerState;
		}

		void PlayerControls::setState(QMediaPlayer::State state) {
			if (state != m_playerState) {
				m_playerState = state;

				switch (state) {
					case QMediaPlayer::StoppedState:
						m_stopButton->setEnabled(false);
						m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
						break;
					case QMediaPlayer::PlayingState:
						m_stopButton->setEnabled(true);
						m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
						break;
					case QMediaPlayer::PausedState:
						m_stopButton->setEnabled(true);
						m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
						break;
				}
			}
		}

		int PlayerControls::volume() const {
#if defined(QT_VERSION) && (QT_VERSION < QT_VERSION_CHECK(5, 8, 0))
			qreal linearVolume = convertVolume(m_volumeSlider->value() / qreal(100), LogarithmicVolumeScale, LinearVolumeScale);
#else
			qreal linearVolume = QAudio::convertVolume(m_volumeSlider->value() / qreal(100), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
#endif

			return qRound(linearVolume * 100);
		}

		void PlayerControls::setVolume(int volume) {
#if defined(QT_VERSION) && (QT_VERSION < QT_VERSION_CHECK(5, 8, 0))
			qreal logarithmicVolume = convertVolume(volume / qreal(100), LinearVolumeScale, LogarithmicVolumeScale);
#else
			qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100), QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale);
#endif
			m_volumeSlider->setValue(qRound(logarithmicVolume * 100));
		}

		bool PlayerControls::isMuted() const {
			return m_playerMuted;
		}

		void PlayerControls::setMuted(bool muted) {
			if (muted != m_playerMuted) {
				m_playerMuted = muted;

				m_muteButton->setIcon(style()->standardIcon(muted
															? QStyle::SP_MediaVolumeMuted
															: QStyle::SP_MediaVolume));
			}
		}

		void PlayerControls::playClicked() {
			switch (m_playerState) {
				case QMediaPlayer::StoppedState:
				case QMediaPlayer::PausedState:
					emit play();
					break;
				case QMediaPlayer::PlayingState:
					emit pause();
					break;
			}
		}

		void PlayerControls::stopClicked() {
			emit stop();
		}

		void PlayerControls::muteClicked() {
			emit changeMuting(!m_playerMuted);
		}

		void PlayerControls::onVolumeSliderValueChanged() {
			emit changeVolume(volume());
		}

#if defined(QT_VERSION) && (QT_VERSION < QT_VERSION_CHECK(5, 8, 0))
		qreal PlayerControls::convertVolume(qreal volume, VolumeScale from, VolumeScale to) const {
			switch (from) {
				case LinearVolumeScale:
					volume = qMax(qreal(0), volume);
					switch (to) {
						case LinearVolumeScale:
							return volume;
						case CubicVolumeScale:
							return qPow(volume, qreal(1 / 3.0));
						case LogarithmicVolumeScale:
							return 1 - std::exp(-volume * LOG100);
						case DecibelVolumeScale:
							if (volume < 0.001)
								return qreal(-200);
							else
								return qreal(20.0) * std::log10(volume);
					}
					break;
				case CubicVolumeScale:
					volume = qMax(qreal(0), volume);
					switch (to) {
						case LinearVolumeScale:
							return volume * volume * volume;
						case CubicVolumeScale:
							return volume;
						case LogarithmicVolumeScale:
							return 1 - std::exp(-volume * volume * volume * LOG100);
						case DecibelVolumeScale:
							if (volume < 0.001)
								return qreal(-200);
							else
								return qreal(3.0 * 20.0) * std::log10(volume);
					}
					break;
				case LogarithmicVolumeScale:
					volume = qMax(qreal(0), volume);
					switch (to) {
						case LinearVolumeScale:
							if (volume > 0.99)
								return 1;
							else
								return -std::log(1 - volume) / LOG100;
						case CubicVolumeScale:
							if (volume > 0.99)
								return 1;
							else
								return qPow(-std::log(1 - volume) / LOG100, qreal(1 / 3.0));
						case LogarithmicVolumeScale:
							return volume;
						case DecibelVolumeScale:
							if (volume < 0.001)
								return qreal(-200);
							else if (volume > 0.99)
								return 0;
							else
								return qreal(20.0) * std::log10(-std::log(1 - volume) / LOG100);
					}
					break;
				case DecibelVolumeScale:
					switch (to) {
						case LinearVolumeScale:
							return qPow(qreal(10.0), volume / qreal(20.0));
						case CubicVolumeScale:
							return qPow(qreal(10.0), volume / qreal(3.0 * 20.0));
						case LogarithmicVolumeScale:
							if (qFuzzyIsNull(volume))
								return 1;
							else
								return 1 - std::exp(-qPow(qreal(10.0), volume / qreal(20.0)) * LOG100);
						case DecibelVolumeScale:
							return volume;
					}
					break;
			}
			return volume;
		}

#endif

	}
}