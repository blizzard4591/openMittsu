#include "Updater.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include "Endian.h"
#include "utility/Logging.h"
#include "utility/QObjectConnectionMacro.h"
#include "utility/Version.h"

Updater::Updater() : QThread() {
	
}

Updater::~Updater() {
	//
}

void Updater::run() {
	if ((Version::versionMajor == 0) && (Version::versionMinor == 0) && (Version::versionPatch == 0)) {
		LOGGER()->info("This is not an official build of OpenMittsu, can not check for updates.");
		return;
	}


	QString queryUrl(QStringLiteral("https://www.openmittsu.de/update.php?major=%1&amp;&minor=%2&amp;patch=%3&amp;commitsSinceTag=%4&amp;gitHash=%5&amp;os=%6&amp;bits=%7&amp;endianness=%8"));

	QUrlQuery urlQuery;
	urlQuery.addQueryItem("major", QString::number(Version::versionMajor));
	urlQuery.addQueryItem("minor", QString::number(Version::versionMinor));
	urlQuery.addQueryItem("patch", QString::number(Version::versionPatch));
	urlQuery.addQueryItem("commitsSinceTag", QString::number(Version::commitsAhead));
	urlQuery.addQueryItem("gitHash", QString::fromStdString(Version::gitRevisionHash));
	urlQuery.addQueryItem("os", QString::fromStdString(Version::systemName));
	urlQuery.addQueryItem("bits", QString::fromStdString(Version::systemPtrSize));
	urlQuery.addQueryItem("endianness", Endian::getEndiannessDescriptionString());
	urlQuery.addQueryItem("channel", QStringLiteral("nightly"));

	QNetworkAccessManager networkAccessManager;
	QEventLoop eventLoop;

	OPENMITTSU_CONNECT(&networkAccessManager, finished(QNetworkReply*), &eventLoop, quit());

	// the HTTPs request
	QNetworkRequest request;
	QUrl url(QStringLiteral("https://www.openmittsu.de/update.php"));
	url.setQuery(urlQuery);
	request.setUrl(url);
	LOGGER_DEBUG("Update query URL is {}.", url.toString().toStdString());

	QNetworkReply *reply = networkAccessManager.get(request);
	eventLoop.exec(); // blocks until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) {
		// success
		QByteArray answer(reply->readAll());
		delete reply;

		try {
			QJsonDocument const jsonResponse = QJsonDocument::fromJson(answer);
			QJsonObject const jsonObject = jsonResponse.object();
			QJsonValue const jsonValueError = jsonObject.value("error");
			QJsonValue const jsonValueErrorMsg = jsonObject.value("errorMsg");

			if (jsonValueError.toBool(true)) {
				if (!jsonResponse.isNull() && !jsonValueErrorMsg.toString().isEmpty()) {
					LOGGER()->warn("Update check failed! Server-side error message: {}", jsonValueErrorMsg.toString().toStdString());
				} else {
					LOGGER()->warn("Update check failed! The server did not send valid JSON. Error message: {}", QString::fromUtf8(answer).toStdString());
				}
			} else {
				QJsonValue const jsonValueHasNewVersion = jsonObject.value("hasNewVersion");
				bool const hasNewVersion = jsonValueHasNewVersion.toBool(false);

				QJsonValue const jsonValueMajor = jsonObject.value("versionMajor");
				QJsonValue const jsonValueMinor = jsonObject.value("versionMinor");
				QJsonValue const jsonValuePatch = jsonObject.value("versionPatch");
				QJsonValue const jsonValueCommitsSinceTag = jsonObject.value("commitsSinceTag");
				QJsonValue const jsonValueGitHash = jsonObject.value("gitHash");
				QJsonValue const jsonValueType = jsonObject.value("type");
				QJsonValue const jsonValueChannel = jsonObject.value("channel");
				QJsonValue const jsonValueLink = jsonObject.value("link");

				if (hasNewVersion) {
					LOGGER()->info("Found new version: {}.{}.{}+{} @ {} ({}) in channel {}.", jsonValueMajor.toInt(), jsonValueMinor.toInt(), jsonValuePatch.toInt(), jsonValueCommitsSinceTag.toInt(), jsonValueGitHash.toString().toStdString(), jsonValueType.toString().toStdString(), jsonValueChannel.toString().toStdString());

					emit foundNewVersion(jsonValueMajor.toInt(), jsonValueMinor.toInt(), jsonValuePatch.toInt(), jsonValueCommitsSinceTag.toInt(), jsonValueGitHash.toString(), jsonValueChannel.toString(), jsonValueLink.toString());
				} else {
					LOGGER()->info("No new version available. The latest version online is: {}.{}.{}+{} @ {} ({}) in channel {}.", jsonValueMajor.toInt(), jsonValueMinor.toInt(), jsonValuePatch.toInt(), jsonValueCommitsSinceTag.toInt(), jsonValueGitHash.toString().toStdString(), jsonValueType.toString().toStdString(), jsonValueChannel.toString().toStdString());
				}
			}
		} catch (...) {
			LOGGER()->warn("Caught an exception while running the update query.");
		}
	} else {
		// failure
		QString const errorString(reply->errorString());
		delete reply;
		LOGGER()->warn("Update check failed, query error: {}", errorString.toStdString());
	}
}