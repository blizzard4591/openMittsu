#ifndef OPENMITTSU_WIDGETS_GIFPLAYER_H_
#define OPENMITTSU_WIDGETS_GIFPLAYER_H_

#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QLabel>
#include <QMovie>
#include <QObject>

class QMouseEvent;

namespace openmittsu {
	namespace widgets {

		class GifPlayer : public QLabel {
			Q_OBJECT
		public:
			explicit GifPlayer(QWidget* parent = nullptr);
			virtual ~GifPlayer();

			void updateData(QByteArray const& gifData, QByteArray const& thumbnailData);
			void deactivateGifMode();
		signals:
			void clicked();
			void doubleClickSelectedText(QString const& selectedText);
		public slots:
			void onMovieStateChanges(QMovie::MovieState state);
		protected:
			virtual void mousePressEvent(QMouseEvent* event) override;
			virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

			virtual void hoverEnter(QHoverEvent* event);
			virtual void hoverLeave(QHoverEvent* event);
			virtual void hoverMove(QHoverEvent* event);
			virtual bool event(QEvent* event) override;

			void paintEvent(QPaintEvent* event) override;
		private:
			QMovie m_movie;
			QImage m_play;
			QImage m_pause;
			QByteArray m_gifData;
			QBuffer m_gifBuffer;
			QPixmap m_playPixmap;
			QPixmap m_pausePixmap;
			QPixmap m_thumbnailPixmap;
			bool m_isMouseOver;
			bool m_gifMode;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_GIFPLAYER_H_