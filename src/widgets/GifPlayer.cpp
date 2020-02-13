#include "src/widgets/GifPlayer.h"

#include <QMouseEvent>
#include <QPainter>

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace widgets {

		GifPlayer::GifPlayer(QWidget* parent) : QLabel(parent), m_movie(), m_play(":/icons/icon_play.png"), m_pause(":/icons/icon_pause.png"), m_playPixmap(":/icons/icon_play.png"), m_pausePixmap(":/icons/icon_pause.png"), m_isMouseOver(false) {
			//
			this->setMinimumSize(50, 50);
			this->setAttribute(Qt::WA_Hover);
			OPENMITTSU_CONNECT(&m_movie, stateChanged(QMovie::MovieState), this, onMovieStateChanges(QMovie::MovieState));
			this->setMovie(&m_movie);
		}

		GifPlayer::~GifPlayer() {
			//
		}

		void GifPlayer::mousePressEvent(QMouseEvent* event) {
			if (event->buttons() == Qt::LeftButton) {

				QMovie::MovieState state = m_movie.state();
				if (state == QMovie::MovieState::NotRunning) {
					m_movie.start();
				} else if (state == QMovie::MovieState::Paused) {
					m_movie.setPaused(false);
				} else if (state == QMovie::MovieState::Running) {
					m_movie.setPaused(true);
				}

				emit clicked();
			}

			QLabel::mousePressEvent(event);
		}

		void GifPlayer::mouseDoubleClickEvent(QMouseEvent *event) {
			QLabel::mouseDoubleClickEvent(event);

			if (this->hasSelectedText()) {
				emit doubleClickSelectedText(this->selectedText());
			}
		}

		bool GifPlayer::event(QEvent* event) {
			switch (event->type()) {
				case QEvent::HoverEnter:
					hoverEnter(static_cast<QHoverEvent*>(event));
					return true;
					break;
				case QEvent::HoverLeave:
					hoverLeave(static_cast<QHoverEvent*>(event));
					return true;
					break;
				case QEvent::HoverMove:
					hoverMove(static_cast<QHoverEvent*>(event));
					return true;
					break;
				default:
					break;
			}
			return QWidget::event(event);
		}

		void GifPlayer::hoverEnter(QHoverEvent* event) {
			m_isMouseOver = true;
		}
		
		void GifPlayer::hoverLeave(QHoverEvent* event) {
			m_isMouseOver = false;
		}

		void GifPlayer::hoverMove(QHoverEvent* event) {

		}

		void GifPlayer::paintEvent(QPaintEvent* event) {
			QLabel::paintEvent(event);

			QMovie::MovieState const state = m_movie.state();
			if (m_isMouseOver || (state != QMovie::MovieState::Running)) {
				QPainter painter(this);
				painter.setOpacity(0.75);

				QSize const movieSize = m_thumbnailPixmap.size();
				int const outerWidth = (movieSize.width() > 10) ? movieSize.width() : this->width();
				int const outerHeight = (movieSize.height() > 10) ? movieSize.height() : this->height();

				int const iconWidth = std::min(std::max(outerWidth / 2, 50), outerWidth);
				int const iconHeight = std::min(std::max(outerHeight / 2, 50), outerHeight);
				int const iconSize = std::min(iconWidth, iconHeight); // To keep the aspect ratio this has to be the same (the icons are quadratic)
				int const startX = (outerWidth - iconSize) / 2;
				int const startY = (outerHeight - iconSize) / 2;

				if (state == QMovie::MovieState::NotRunning) {
					painter.drawPixmap(startX, startY, iconSize, iconSize, m_playPixmap);
				} else if (state == QMovie::MovieState::Paused) {
					painter.drawPixmap(startX, startY, iconSize, iconSize, m_playPixmap);
				} else if (state == QMovie::MovieState::Running) {
					painter.drawPixmap(startX, startY, iconSize, iconSize, m_pausePixmap);
				}
			}
		}

		void GifPlayer::updateData(QByteArray const& gifData, QByteArray const& thumbnailData) {
			m_gifBuffer.close();

			m_gifData = gifData;
			if (!m_thumbnailPixmap.loadFromData(thumbnailData)) {
				LOGGER()->error("Failed to load thumbnail from data stream!");
			}

			m_gifBuffer.setData(m_gifData);
			m_gifBuffer.open(QBuffer::ReadOnly);
			m_movie.setDevice(&m_gifBuffer);
			m_movie.start();
		}

		void GifPlayer::onMovieStateChanges(QMovie::MovieState state) {
			if (state == QMovie::MovieState::NotRunning) {
				LOGGER_DEBUG("State: Not Running.");
			} else if(state == QMovie::MovieState::Paused) {
				LOGGER_DEBUG("State: Paused.");
			} else if (state == QMovie::MovieState::Running) {
				LOGGER_DEBUG("State: Running.");
			}
			this->update();
		}
	}
}
