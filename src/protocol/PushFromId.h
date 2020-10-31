#ifndef OPENMITTSU_PROTOCOL_PUSHFROMID_H_
#define OPENMITTSU_PROTOCOL_PUSHFROMID_H_

#include <QByteArray>
#include <QString>

#include <string>

namespace openmittsu {
	namespace protocol {
		class ContactId;

		class PushFromId {
		public:
			PushFromId(ContactId const& contactId);
			PushFromId(QString const& selfNickname);
			PushFromId(QByteArray const& pushFromId);
			PushFromId(PushFromId const& other);
			virtual ~PushFromId();

			bool containsStandardContactId() const;
			quint64 getPushFromId() const;
			QByteArray getPushFromIdAsByteArray() const;
			static int getSizeOfPushFromIdInBytes();
			std::string toString() const;
			QString toQString() const;

			bool operator ==(PushFromId const& other) const;
			bool operator !=(PushFromId const& other) const;
		private:
			QByteArray pushFromIdBytes;

			static QByteArray expandFromContactId(ContactId const& contactId);
			static QByteArray expandFromNickname(QString const& nickname);

			// Disable the default constructor
			PushFromId();
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_PUSHFROMID_H_
