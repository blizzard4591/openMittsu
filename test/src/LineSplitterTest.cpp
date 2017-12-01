#include "gtest/gtest.h"

#include <QByteArray>
#include <QMap>
#include <QString>
#include <iostream>

#include "src/backup/SimpleCsvLineSplitter.h"

#ifndef qUtf8Printable
#define qUtf8Printable(string) QString(string).toUtf8().constData()
#endif

QT_BEGIN_NAMESPACE
inline void PrintTo(const QString& str, ::std::ostream* os) {
	*os << qUtf8Printable(str);  // whatever needed to print bar to os
}
QT_END_NAMESPACE

TEST(LineSplitterTest, BasicTest) {
	QString const oneColumnEmpty("\"\"");
	QString const oneColumnText("\"someText\"");
	QString const oneColumnTextWithQuotMark("\"some \"\"quoted\"\" Text\"");

	openmittsu::backup::SimpleCsvLineSplitter const splitOneColumnEmpty = openmittsu::backup::SimpleCsvLineSplitter::split(1, oneColumnEmpty);
	ASSERT_EQ(1, splitOneColumnEmpty.getNumberOfColumns());
	ASSERT_EQ(QString(""), splitOneColumnEmpty.getColumn(0));

	openmittsu::backup::SimpleCsvLineSplitter const splitOneColumnText = openmittsu::backup::SimpleCsvLineSplitter::split(1, oneColumnText);
	ASSERT_EQ(1, splitOneColumnText.getNumberOfColumns());
	ASSERT_EQ(QString("someText"), splitOneColumnText.getColumn(0));

	openmittsu::backup::SimpleCsvLineSplitter const splitOneColumnTextWithQuotMark = openmittsu::backup::SimpleCsvLineSplitter::split(1, oneColumnTextWithQuotMark);
	ASSERT_EQ(1, splitOneColumnTextWithQuotMark.getNumberOfColumns());
	ASSERT_EQ(QString("some \"quoted\" Text"), splitOneColumnTextWithQuotMark.getColumn(0));
}

