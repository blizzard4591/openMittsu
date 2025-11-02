// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef QMEDIAPLAYLIST_P_H
#define QMEDIAPLAYLIST_P_H

#include <QtGlobal>
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "src/widgets/player/QMediaPlaylist.h"
#include "src/widgets/player/QPlaylistFileParser.h"

#include <QUrl>

#include <QDebug>

#ifdef Q_MOC_RUN
#    pragma Q_MOC_EXPAND_MACROS
#endif

QT_BEGIN_NAMESPACE

class QMediaPlaylistControl;

class QMediaPlaylistPrivate
{
    Q_DECLARE_PUBLIC(QMediaPlaylist)
public:
    QMediaPlaylistPrivate();

    virtual ~QMediaPlaylistPrivate();

    void loadFailed(QMediaPlaylist::Error error, const QString &errorString);

    void loadFinished();

    bool checkFormat(const char *format) const;

    void ensureParser();

    int nextPosition(int steps) const;
    int prevPosition(int steps) const;

    QList<QUrl> playlist;

    int currentPos = -1;
    QMediaPlaylist::PlaybackMode playbackMode = QMediaPlaylist::Sequential;

    QPlaylistFileParser *parser = nullptr;
    mutable QMediaPlaylist::Error error;
    mutable QString errorString;

    QMediaPlaylist *q_ptr = nullptr;
};

QT_END_NAMESPACE
#endif

#endif // QMEDIAPLAYLIST_P_H
