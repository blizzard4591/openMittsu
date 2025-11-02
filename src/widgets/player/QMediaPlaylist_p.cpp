// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "src/widgets/player/QMediaPlaylist_p.h"

#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

QT_BEGIN_NAMESPACE

QMediaPlaylistPrivate::QMediaPlaylistPrivate() : error(QMediaPlaylist::NoError) { }

QMediaPlaylistPrivate::~QMediaPlaylistPrivate()
{
    delete parser;
}

void QMediaPlaylistPrivate::loadFailed(QMediaPlaylist::Error error, const QString &errorString)
{
    this->error = error;
    this->errorString = errorString;

    emit q_ptr->loadFailed();
}

void QMediaPlaylistPrivate::loadFinished()
{
    q_ptr->addMedia(parser->playlist);

    emit q_ptr->loaded();
}

bool QMediaPlaylistPrivate::checkFormat(const char *format) const
{
    QLatin1String f(format);
    QPlaylistFileParser::FileType type =
            format ? QPlaylistFileParser::UNKNOWN : QPlaylistFileParser::M3U8;
    if (format) {
        if (f == QLatin1String("m3u") || f == QLatin1String("text/uri-list")
            || f == QLatin1String("audio/x-mpegurl") || f == QLatin1String("audio/mpegurl"))
            type = QPlaylistFileParser::M3U;
        else if (f == QLatin1String("m3u8") || f == QLatin1String("application/x-mpegURL")
                 || f == QLatin1String("application/vnd.apple.mpegurl"))
            type = QPlaylistFileParser::M3U8;
    }

    if (type == QPlaylistFileParser::UNKNOWN || type == QPlaylistFileParser::PLS) {
        error = QMediaPlaylist::FormatNotSupportedError;
        errorString = QMediaPlaylist::tr("This file format is not supported.");
        return false;
    }
    return true;
}

void QMediaPlaylistPrivate::ensureParser()
{
    if (parser)
        return;

    parser = new QPlaylistFileParser(q_ptr);
    QObject::connect(parser, &QPlaylistFileParser::finished, q_ptr, [this]() { loadFinished(); });
    QObject::connect(parser, &QPlaylistFileParser::error, q_ptr,
                     [this](QMediaPlaylist::Error err, const QString &errorMsg) {
                         loadFailed(err, errorMsg);
                     });
}

QT_END_NAMESPACE
#endif
