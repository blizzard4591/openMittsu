#include "gtest/gtest.h"

#include <QMap>
#include <QString>
#include <iostream>

#include "src/utility/TextFormatter.h"

#ifndef qUtf8Printable
#define qUtf8Printable(string) QString(string).toUtf8().constData()
#endif

QT_BEGIN_NAMESPACE
inline void PrintTo(const QString& str, ::std::ostream* os) {
	*os << qUtf8Printable(str);  // whatever needed to print bar to os
}
QT_END_NAMESPACE

TEST(TextFormatterTest, BoldTest) {
	QMap<QString, QString> testData;
	testData.insert("", "");
	testData.insert("*", "*");
	testData.insert("**", "**");
	testData.insert("* *", "* *");
	testData.insert("*  *", "*  *");
	testData.insert("* test*", "* test*");
	testData.insert("*test *", "*test *");
	testData.insert("* test *", "* test *");
	testData.insert("*test*", "<span style=\"font-weight: bold;\">test</span>");
	
	testData.insert("Some text *", "Some text *");
	testData.insert("Some text **", "Some text **");
	testData.insert("Some text * *", "Some text * *");
	testData.insert("Some text *  *", "Some text *  *");
	testData.insert("Some text * test*", "Some text * test*");
	testData.insert("Some text *test *", "Some text *test *");
	testData.insert("Some text * test *", "Some text * test *");
	testData.insert("Some text *test*", "Some text <span style=\"font-weight: bold;\">test</span>");

	QMap<QString, QString>::const_iterator i = testData.constBegin();
	while (i != testData.constEnd()) {
		QString const result = openmittsu::utility::TextFormatter::format(i.key());

		if (result != i.value()) {
			std::cout << "Error: Input is " << i.value().toStdString() << std::endl << "Output is " << result.toStdString() << std::endl;
		}

		ASSERT_EQ(result, i.value());
		++i;
	}
}

TEST(TextFormatterTest, BugReportMathEquation) {
	QString const input = QStringLiteral("119 * 88 * 99 = 1036728");
	QString const result = openmittsu::utility::TextFormatter::format(input);

	ASSERT_EQ(result, input);
}
