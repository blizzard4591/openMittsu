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

#include "src/widgets/player/Player.h"

#include "src/widgets/player/PlayerControls.h"
#include "src/widgets/player/PlaylistModel.h"
#include "src/widgets/player/VideoWidget.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>

#include <QDir>

#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "ui_Player.h"

namespace openmittsu {
	namespace widgets {

		Player::Player(bool useVideoWidget, QWidget *parent) : QWidget(parent), m_ui(std::make_unique<Ui::Player>()), m_useVideoWidget(useVideoWidget), m_tempFile(QDir::tempPath().append(QStringLiteral("/openmittsu_player_temp_XXXXXX.mp4"))) {
			m_ui->setupUi(this);
			m_player = new QMediaPlayer(this);
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
			if (m_useVideoWidget) {
				m_player->setAudioRole(QAudio::VideoRole);
			} else {
				m_player->setAudioRole(QAudio::MusicRole);
			}
#endif

			// owned by PlaylistModel
			m_playlist = new QMediaPlaylist();
			m_player->setPlaylist(m_playlist);

			OPENMITTSU_CONNECT(m_player, durationChanged(qint64), this, durationChanged(qint64));
			OPENMITTSU_CONNECT(m_player, positionChanged(qint64), this, positionChanged(qint64));
			OPENMITTSU_CONNECT(m_player, metaDataChanged(), this, metaDataChanged());
			OPENMITTSU_CONNECT(m_playlist, currentIndexChanged(int), this, playlistPositionChanged(int));
			OPENMITTSU_CONNECT(m_player, mediaStatusChanged(QMediaPlayer::MediaStatus), this, statusChanged(QMediaPlayer::MediaStatus));
			OPENMITTSU_CONNECT(m_player, bufferStatusChanged(int), this, bufferingProgress(int));
			OPENMITTSU_CONNECT(m_player, videoAvailableChanged(bool), this, videoAvailableChanged(bool));
			OPENMITTSU_CONNECT(m_player, error(QMediaPlayer::Error), this, displayErrorMessage());
			OPENMITTSU_CONNECT(m_player, stateChanged(QMediaPlayer::State), this, stateChanged(QMediaPlayer::State));

			if (m_useVideoWidget) {
				m_videoWidget = m_ui->videoWidget;
				m_player->setVideoOutput(m_videoWidget);
			}

			m_playlistModel = new PlaylistModel(this);
			m_playlistModel->setPlaylist(m_playlist);

			m_slider = m_ui->slider;
			m_slider->setRange(0, m_player->duration() / 1000);

			m_labelDuration = m_ui->lblDuration;

			// Pre-set duration
			int minutes = m_player->duration() / 1000 / 60;
			int seconds = (m_player->duration() / 1000) - (minutes * 60);
			m_labelDuration->setText(QStringLiteral("0:00 / %1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));

			OPENMITTSU_CONNECT(m_slider, sliderMoved(int), this, seek(int));

			PlayerControls *controls = m_ui->playerControls;
			controls->setState(m_player->state());
			controls->setVolume(m_player->volume());
			controls->setMuted(controls->isMuted());

			OPENMITTSU_CONNECT(controls, play(), m_player, play());
			OPENMITTSU_CONNECT(controls, pause(), m_player, pause());
			OPENMITTSU_CONNECT(controls, stop(), m_player, stop());
			OPENMITTSU_CONNECT(controls, changeVolume(int), m_player, setVolume(int));
			OPENMITTSU_CONNECT(controls, changeMuting(bool), m_player, setMuted(bool));
			if (m_useVideoWidget) {
				OPENMITTSU_CONNECT(controls, stop(), m_videoWidget, update());
			}

			OPENMITTSU_CONNECT(m_player, stateChanged(QMediaPlayer::State), controls, setState(QMediaPlayer::State));
			OPENMITTSU_CONNECT(m_player, volumeChanged(int), controls, setVolume(int));
			OPENMITTSU_CONNECT(m_player, mutedChanged(bool), controls, setMuted(bool));

			if (m_useVideoWidget) {
				//m_fullScreenButton = new QPushButton(tr("Fullscreen"), this);
				//m_fullScreenButton->setCheckable(true);
			}

			if (!m_useVideoWidget) {
				m_ui->videoWidget->hide();
			}

			QBoxLayout *controlLayout = m_ui->controlLayout;
			if (m_useVideoWidget) {
				controlLayout->addStretch(1);
				//controlLayout->addWidget(m_fullScreenButton);
			}

			if (!isPlayerAvailable()) {
				QMessageBox::warning(this, tr("Service not available"),
									 tr("The QMediaPlayer object does not have a valid service.\n"\
										"Please check the media service plugins are installed."));

				controls->setEnabled(false);
				m_fullScreenButton->setEnabled(false);
			}

			metaDataChanged();
		}

		Player::~Player() {
			//
		}

		void Player::paintEvent(QPaintEvent* event) {
			QStyleOption opt;
			opt.init(this);
			QPainter p(this);
			style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

			QWidget::paintEvent(event);
		}

		bool Player::isPlayerAvailable() const {
			return m_player->isAvailable();
		}

		void Player::addToPlaylist(const QList<QUrl> &urls) {
			for (auto &url : urls) {
				m_playlist->addMedia(url);
			}
		}

		void Player::setCustomAudioRole(const QString &role) {
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
			m_player->setCustomAudioRole(role);
#endif
		}

		void Player::durationChanged(qint64 duration) {
			m_duration = duration / 1000;
			m_slider->setMaximum(m_duration);
		}

		void Player::positionChanged(qint64 progress) {
			if (!m_slider->isSliderDown())
				m_slider->setValue(progress / 1000);

			updateDurationInfo(progress / 1000);
		}

		void Player::metaDataChanged() {
			if (m_player->isMetaDataAvailable()) {
				setTrackInfo(QString("%1 - %2")
							 .arg(m_player->metaData(QMediaMetaData::AlbumArtist).toString())
							 .arg(m_player->metaData(QMediaMetaData::Title).toString()));

				if (m_coverLabel) {
					QUrl url = m_player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

					m_coverLabel->setPixmap(!url.isEmpty()
											? QPixmap(url.toString())
											: QPixmap());
				}
			}
		}

		void Player::previousClicked() {
			// Go to previous track if we are within the first 5 seconds of playback
			// Otherwise, seek to the beginning.
			if (m_player->position() <= 5000)
				m_playlist->previous();
			else
				m_player->setPosition(0);
		}

		void Player::jump(const QModelIndex &index) {
			if (index.isValid()) {
				m_playlist->setCurrentIndex(index.row());
				m_player->play();
			}
		}

		void Player::playlistPositionChanged(int currentItem) {
			//
		}

		void Player::seek(int seconds) {
			m_player->setPosition(seconds * 1000);
		}

		void Player::statusChanged(QMediaPlayer::MediaStatus status) {
			handleCursor(status);

			// handle status message
			switch (status) {
				case QMediaPlayer::UnknownMediaStatus:
				case QMediaPlayer::NoMedia:
				case QMediaPlayer::LoadedMedia:
					setStatusInfo(QString());
					break;
				case QMediaPlayer::LoadingMedia:
					setStatusInfo(tr("Loading..."));
					break;
				case QMediaPlayer::BufferingMedia:
				case QMediaPlayer::BufferedMedia:
					setStatusInfo(tr("Buffering %1%").arg(m_player->bufferStatus()));
					break;
				case QMediaPlayer::StalledMedia:
					setStatusInfo(tr("Stalled %1%").arg(m_player->bufferStatus()));
					break;
				case QMediaPlayer::EndOfMedia:
					QApplication::alert(this);
					break;
				case QMediaPlayer::InvalidMedia:
					displayErrorMessage();
					break;
			}
		}

		void Player::stateChanged(QMediaPlayer::State state) {
			if (state == QMediaPlayer::StoppedState) {

			}
			//
		}

		void Player::play(QByteArray const& mp4Data) {
			if (m_tempFile.open()) {
				{
					LOGGER_DEBUG("Player: Temporary filename for audio/video data is {}", m_tempFile.fileName().toStdString());
					QFile file(m_tempFile.fileName());
					if (!file.open(QFile::WriteOnly)) {
						throw;
					}
					file.write(mp4Data);
					file.close();
				}

				QList<QUrl> urls;
				QUrl url = QUrl::fromLocalFile(m_tempFile.fileName());
				urls.append(url);

				this->addToPlaylist(urls);
			}
		}

		void Player::handleCursor(QMediaPlayer::MediaStatus status) {
#ifndef QT_NO_CURSOR
			if (status == QMediaPlayer::LoadingMedia ||
				status == QMediaPlayer::BufferingMedia ||
				status == QMediaPlayer::StalledMedia)
				setCursor(QCursor(Qt::BusyCursor));
			else
				unsetCursor();
#endif
		}

		void Player::bufferingProgress(int progress) {
			if (m_player->mediaStatus() == QMediaPlayer::StalledMedia)
				setStatusInfo(tr("Stalled %1%").arg(progress));
			else
				setStatusInfo(tr("Buffering %1%").arg(progress));
		}

		void Player::videoAvailableChanged(bool available) {
			if (!m_useVideoWidget) {
				return;
			}
			/*
			if (!available) {
				OPENMITTSU_DISCONNECT(m_fullScreenButton, clicked(bool), m_videoWidget, setFullScreen(bool));
				OPENMITTSU_DISCONNECT(m_videoWidget, fullScreenChanged(bool), m_fullScreenButton, setChecked(bool));
				m_videoWidget->setFullScreen(false);
			} else {
				OPENMITTSU_CONNECT(m_fullScreenButton, clicked(bool), m_videoWidget, setFullScreen(bool));
				OPENMITTSU_CONNECT(m_videoWidget, fullScreenChanged(bool), m_fullScreenButton, setChecked(bool));

				if (m_fullScreenButton->isChecked()) {
					m_videoWidget->setFullScreen(true);
				}
			}
			*/
			m_videoWidget->setFullScreen(true);
		}

		void Player::setTrackInfo(const QString &info) {
			m_trackInfo = info;

			if (!m_statusInfo.isEmpty())
				setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
			else
				setWindowTitle(m_trackInfo);
		}

		void Player::setStatusInfo(const QString &info) {
			m_statusInfo = info;

			if (!m_statusInfo.isEmpty())
				setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
			else
				setWindowTitle(m_trackInfo);
		}

		void Player::displayErrorMessage() {
			setStatusInfo(m_player->errorString());
		}

		void Player::updateDurationInfo(qint64 currentInfo) {
			QString tStr;
			if (currentInfo || m_duration) {
				QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
								  currentInfo % 60, (currentInfo * 1000) % 1000);
				QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
								m_duration % 60, (m_duration * 1000) % 1000);
				QString format = "mm:ss";
				if (m_duration > 3600)
					format = "hh:mm:ss";
				tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
			}
			m_labelDuration->setText(tStr);
		}

	}
}
