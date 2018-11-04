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

#ifndef OPENMITTSU_WIDGETS_PLAYER_PLAYER_H_
#define OPENMITTSU_WIDGETS_PLAYER_PLAYER_H_

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <QString>
#include <QTemporaryFile>

QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QVideoProbe;
class QVideoWidget;
class QAudioProbe;
QT_END_NAMESPACE

namespace Ui {
	class Player;
}

namespace openmittsu {
	namespace widgets {

		class PlaylistModel;

		class Player : public QWidget {
			Q_OBJECT

		public:
			explicit Player(bool useVideoWidget, QWidget *parent = nullptr);
			virtual ~Player();

			bool isPlayerAvailable() const;

			void play(QByteArray const& mp4Data);
			void addToPlaylist(const QList<QUrl> &urls);
			void setCustomAudioRole(const QString &role);
		signals:
			void fullScreenChanged(bool fullScreen);
		protected:
			virtual void paintEvent(QPaintEvent* event) override;
		private slots:
			void durationChanged(qint64 duration);
			void positionChanged(qint64 progress);
			void metaDataChanged();

			void previousClicked();

			void seek(int seconds);
			void jump(const QModelIndex &index);
			void playlistPositionChanged(int);

			void statusChanged(QMediaPlayer::MediaStatus status);
			void stateChanged(QMediaPlayer::State state);
			void bufferingProgress(int progress);
			void videoAvailableChanged(bool available);

			void displayErrorMessage();
		private:
			void setTrackInfo(const QString &info);
			void setStatusInfo(const QString &info);
			void handleCursor(QMediaPlayer::MediaStatus status);
			void updateDurationInfo(qint64 currentInfo);

			Ui::Player* m_ui;
			bool m_useVideoWidget;

			QMediaPlayer *m_player = nullptr;
			QMediaPlaylist *m_playlist = nullptr;
			QVideoWidget *m_videoWidget = nullptr;
			QLabel *m_coverLabel = nullptr;
			QSlider *m_slider = nullptr;
			QLabel *m_labelDuration = nullptr;
			QPushButton *m_fullScreenButton = nullptr;

			PlaylistModel *m_playlistModel = nullptr;
			QString m_trackInfo;
			QString m_statusInfo;
			qint64 m_duration;

			QTemporaryFile m_tempFile;
		};
	}
}

#endif // OPENMITTSU_WIDGETS_PLAYER_PLAYER_H_
