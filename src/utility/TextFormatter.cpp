#include "TextFormatter.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStringList>

#include "exceptions/InternalErrorException.h"

QString TextFormatter::format(QString const& input) {
	// Qt 5.4 and later support the QRegularExpression::OptimizeOnFirstUsageOption option.
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
	static const QRegularExpression::PatternOptions patternOptions = QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::OptimizeOnFirstUsageOption;
#else
	static const QRegularExpression::PatternOptions patternOptions = QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption;
#endif
	static QRegularExpression regExpLinks(QStringLiteral("\\b((https?|ftp|file)://[-A-Z0-9+&@#/%?=~_|$!:,.;]*[A-Z0-9+&@#/%=~_|$])"), patternOptions);

	QStringList linkList;
	QStringList remainingParts;

	int start = 0;
	int position = 0;
	
	QRegularExpressionMatchIterator matchIterator = regExpLinks.globalMatch(input);
	while (true) {
		if (!matchIterator.hasNext()) {
			remainingParts.append(input.mid(start));
			break;
		} else {
			QRegularExpressionMatch match = matchIterator.next();
			position = match.capturedStart(1);

			remainingParts.append(input.mid(start, position - start));
			QString const matchedLink = match.captured(1);
			int const matchedLinkLength = matchedLink.length();

			linkList.append(matchedLink);

			start = position + matchedLinkLength;
		}
	}

	int const textPartCount = remainingParts.size();
	int const linkCount = linkList.size();

	if (textPartCount != (linkCount + 1)) {
		throw InternalErrorException() << "Error on text- vs. link counts, we have " << remainingParts.size() << " text parts and " << linkList.size() << " links.";
	}

	static QRegularExpression regExpBold(QStringLiteral("\\B\\*([^\\r\\n]+?)\\*\\B"), patternOptions);
	static QRegularExpression regExpItalic(QStringLiteral("\\b_([^\\r\\n]+?)_\\b"), patternOptions);
	static QRegularExpression regExpStrikethrough(QStringLiteral("\\B~([^\\r\\n]+?)~\\B"), patternOptions);
	static QRegularExpression regExpNewline(QStringLiteral("\\R"), patternOptions);

	QString output;
	for (int i = 0; i <= linkCount; ++i) {
		QString textPart = remainingParts.at(i);
		textPart.replace(regExpBold, QStringLiteral("<span style=\"font-weight: bold;\">\\1</span>"));
		textPart.replace(regExpItalic, QStringLiteral("<span style=\"font-style: italic;\">\\1</span>"));
		textPart.replace(regExpStrikethrough, QStringLiteral("<span style=\"text-decoration: line-through;\">\\1</span>"));
		textPart.replace(regExpNewline, QStringLiteral("<br>"));

		output.append(textPart);
		if (i < linkCount) {
			QString linkPart = QStringLiteral("<a href=\"%1\">%1</a>").arg(linkList.at(i));
			output.append(linkPart);
		}
	}

	return output;
}
