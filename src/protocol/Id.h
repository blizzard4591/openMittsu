#ifndef OPENMITTSU_PROTOCOL_ID_H_
#define OPENMITTSU_PROTOCOL_ID_H_

#include <string>
#include <QString>

class Id {
public:
	virtual ~Id();

	virtual std::string toString() const = 0;
	virtual QString toQString() const = 0;
protected:
	Id();
	Id(Id const& other);
};

#endif // OPENMITTSU_PROTOCOL_ID_H_
