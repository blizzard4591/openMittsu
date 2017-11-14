#include "src/encoding/Base32.h"

#include "src/exceptions/InvalidInputException.h"
#include <cstdint>

namespace openmittsu {
	namespace encoding {

		bool Base32::isCharacterOfBase32Encoding(QChar const& inputChar) {
			// Valid are 2 ... 7 and A ... Z
			ushort const inputCharUnicodeValue = inputChar.unicode();
			if (((inputCharUnicodeValue >= '2') && (inputCharUnicodeValue <= '7')) || ((inputCharUnicodeValue >= 'A') && (inputCharUnicodeValue <= 'Z'))) {
				return true;
			}
			return false;
		}

		char Base32::convert5BitValueToChar(char inputChar) {
			if (inputChar > 31) {
				throw openmittsu::exceptions::InvalidInputException() << "Illegal input for the convert5BitValueToChar function: \"" << inputChar << "\".";
			} else if (inputChar > 25) {
				return '2' + (inputChar - 26);
			} else {
				return 'A' + inputChar; // 0 == A, ...
			}
		}

		char Base32::convertCharTo5BitValue(QChar const& inputChar) {
			if (!isCharacterOfBase32Encoding(inputChar)) {
				return 0;
			}

			ushort const inputCharUnicodeValue = inputChar.unicode();
			if (inputCharUnicodeValue >= 'A') {
				return inputCharUnicodeValue - 'A'; // *inputChar - 65 -> A == 0, B == 1
			} else {
				return (26 + inputCharUnicodeValue) - '2'; // *inputChar - 50 + 26
			}
		}

		// this function assumes that the amount of decoded Characters is always a multiple of 8 Base32 Characters (so that the output forms a full number of Bytes)
		QByteArray Base32::decodeBase32Sequence(QString const& source) {
			QByteArray result;
			size_t usedBits = 0;
			char value = 0;
			uint_fast32_t cache = 0;
			const int sourceLength = source.size();
			for (int i = 0; i < sourceLength; ++i) {
				if (!isCharacterOfBase32Encoding(source.at(i))) {
					continue;
				}

				value = convertCharTo5BitValue(source.at(i));
				cache = (cache << 5) | value;
				usedBits += 5;

				while (usedBits >= 8) {
					result.append((cache >> (usedBits - 8)) & 0xFF);
					usedBits -= 8;
				}
			}
			return result;
		}

		QString Base32::encodeBase32Sequence(QByteArray const& source, bool formatString) {
			QString result;
			size_t usedBits = 0;
			uint32_t cache = 0;
			unsigned char value = 0;

			const int sourceLength = source.size();
			for (int i = 0; i < sourceLength; ++i) {
				char c = source.at(i);
				value = *reinterpret_cast<unsigned char*>(&c);
				cache = (cache << 8) | value;
				usedBits += 8;

				while (usedBits >= 5) {
					result.append(convert5BitValueToChar((cache >> (usedBits - 5)) & 0x1F));
					usedBits -= 5;
				}
			}

			// Last iteration to take the last bits
			if (usedBits > 0) {
				result.append(convert5BitValueToChar((cache << (5 - usedBits)) & 0x1F));
			}

			if (formatString) {
				QString formatedResult;
				for (int i = 0, length = result.size(); i < length; ++i) {
					if ((i % 4 == 0) && (i > 0)) {
						formatedResult.append('-');
					}
					formatedResult.append(result.at(i));
				}
				return formatedResult;
			}

			return result;
		}

	}
}
