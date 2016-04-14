#ifndef OPENMITTSU_UTILITY_HEXCHAR_H_
#define OPENMITTSU_UTILITY_HEXCHAR_H_

#include <iomanip>
#include <string>
#include <iostream>

class HexChar {
public:
	HexChar(unsigned char uc);
	HexChar(char sc);

	unsigned char getChar() const;
private:
	unsigned char c;
};

std::ostream& operator<<(std::ostream& o, HexChar const& h);

#endif // OPENMITTSU_UTILITY_HEXCHAR_H_
