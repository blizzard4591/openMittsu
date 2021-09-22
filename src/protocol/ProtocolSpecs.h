#ifndef OPENMITTSU_PROTOCOL_PROTOCOLSPECS_H_
#define OPENMITTSU_PROTOCOL_PROTOCOLSPECS_H_

#include <QByteArray>
#include <cstdint>

#include <sodium.h>

constexpr auto PROTO_FINGERPRINT_LENGTH_BYTES                   = (16);
constexpr auto PROTO_KEY_LENGTH_BYTES                           = (32);
constexpr auto PROTO_IDENTITY_LENGTH_BYTES                      = (8);
constexpr auto PROTO_NONCE_LENGTH_BYTES                         = (16);
constexpr auto PROTO_NONCE_PREFIX_LENGTH_BYTES                  = (16);
constexpr auto PROTO_AUTHENTICATION_UNENCRYPTED_LENGTH_BYTES    = (128);
constexpr auto PROTO_AUTHENTICATION_VERSION_BYTES               = (32);
constexpr auto PROTO_AUTHENTICATION_RANDOMNONCE_BYTES           = (24);
constexpr auto PROTO_AUTHENTICATION_REPLY_LENGTH_BYTES          = (16);

constexpr auto PROTO_DATA_HEADER_SIZE_LENGTH_BYTES              = (2);
constexpr auto PROTO_DATA_HEADER_TYPE_LENGTH_BYTES              = (4);
constexpr auto PROTO_ACKNOWLEDGMENT_LENGTH_BYTES                = (20);

constexpr auto PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES             = (4);
constexpr auto PROTO_MESSAGE_FLAGS_LENGTH_BYTES                 = (1);
constexpr auto PROTO_MESSAGE_RESERVED_AFTER_FLAGS_LENGTH_BYTES  = (3);
constexpr auto PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES             = (8);
constexpr auto PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES             = (32);
constexpr auto PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES           = ((PROTO_IDENTITY_LENGTH_BYTES) + (PROTO_IDENTITY_LENGTH_BYTES) + (PROTO_MESSAGE_MESSAGEID_LENGTH_BYTES) + (PROTO_MESSAGE_TIMESTAMP_LENGTH_BYTES) + (PROTO_MESSAGE_FLAGS_LENGTH_BYTES) + (PROTO_MESSAGE_RESERVED_AFTER_FLAGS_LENGTH_BYTES) + (PROTO_MESSAGE_PUSH_FROM_LENGTH_BYTES));

constexpr auto PROTO_MESSAGE_PADDING_MAX_LENGTH_BYTES           = (255);

constexpr auto PROTO_MESSAGE_NONCE_LENGTH_BYTES                 = (crypto_box_NONCEBYTES);

constexpr auto PROTO_MESSAGE_CONTENT_TYPE_LENGTH_BYTES          = (1);

constexpr auto PROTO_FILE_ENCRYPTION_KEY_LENGTH_BYTES           = (crypto_secretbox_KEYBYTES);

constexpr auto PROTO_PACKET_MAX_LENGTH_BYTES                    = (4096);
constexpr auto PROTO_MESSAGE_CONTENT_PAYLOAD_MAX_LENGTH_BYTES   = ((PROTO_PACKET_MAX_LENGTH_BYTES) - (PROTO_DATA_HEADER_TYPE_LENGTH_BYTES) - (crypto_box_MACBYTES) - (crypto_box_MACBYTES) - (PROTO_MESSAGE_CONTENT_TYPE_LENGTH_BYTES) - (PROTO_MESSAGE_NONCE_LENGTH_BYTES) - (PROTO_MESSAGE_HEADER_FULL_LENGTH_BYTES) - (PROTO_MESSAGE_PADDING_MAX_LENGTH_BYTES));

// Packet Types
constexpr auto PROTO_PACKET_SIGNATURE_KEEPALIVE_REQUEST         = ((char)0);                    // 0x00
constexpr auto PROTO_PACKET_SIGNATURE_SENDING_MSG               = ((char)1);                    // 0x01
constexpr auto PROTO_PACKET_SIGNATURE_DELIVERING_MSG            = ((char)2);                    // 0x02
constexpr auto PROTO_PACKET_SIGNATURE_KEEPALIVE_ANSWER          = ((char)-128);                 // 0x80
constexpr auto PROTO_PACKET_SIGNATURE_SERVER_ACK                = ((char)-127);                 // 0x81
constexpr auto PROTO_PACKET_SIGNATURE_CLIENT_ACK                = ((char)-126);                 // 0x82
constexpr auto PROTO_PACKET_SIGNATURE_CONNECTION_ESTABLISHED    = ((char)-48);                  // 0xD0
constexpr auto PROTO_PACKET_SIGNATURE_CONNECTION_DUPLICATE      = ((char)-32);                  // 0xE0
constexpr auto PROTO_PACKET_SIGNATURE_CONNECTION_ALERT          = ((char)-31);                  // 0xE1

// Payload Signature Bytes
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_TEXT             = ((char)1);                    // 0x01
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_PICTURE          = ((char)2);                    // 0x02
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_LOCATION         = ((char)16);                   // 0x10
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_VIDEO            = ((char)19);                   // 0x13
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_AUDIO            = ((char)20);                   // 0x14
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_POLL_CREATION    = ((char)21);                   // 0x15
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_POLL_ANSWER      = ((char)22);                   // 0x16
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_FILE             = ((char)23);                   // 0x17
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_SET_PHOTO        = ((char)24);                   // 0x18
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_DELETE_PHOTO     = ((char)25);                   // 0x19
constexpr auto PROTO_MESSAGE_SIGNATURE_CONTACT_REQUEST_PHOTO    = ((char)26);                   // 0x1A

constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_TEXT               = ((char)65);                   // 0x41
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_LOCATION           = ((char)66);                   // 0x42
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_PICTURE            = ((char)67);                   // 0x43
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_VIDEO              = ((char)68);                   // 0x44
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_AUDIO              = ((char)69);                   // 0x45
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_FILE               = ((char)70);                   // 0x46
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_CREATION           = ((char)74);                   // 0x4A
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_TITLE              = ((char)75);                   // 0x4B
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_LEAVE              = ((char)76);                   // 0x4C
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_ADD_MEMBER         = ((char)77);                   // 0x4D
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_REMOVE_MEMBER      = ((char)78);                   // 0x4E
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_DESTROY            = ((char)79);                   // 0x4F
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_PHOTO              = ((char)80);                   // 0x50
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_SYNC               = ((char)81);                   // 0x51
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_POLL_CREATIION     = ((char)82);                   // 0x52
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_POLL_ANSWER        = ((char)83);                   // 0x53
constexpr auto PROTO_MESSAGE_SIGNATURE_GROUP_DELETE_PHOTO       = ((char)84);                   // 0x54

constexpr auto PROTO_MESSAGE_SIGNATURE_VOIP_CALL_OFFER          = ((char)96);                   // 0x60
constexpr auto PROTO_MESSAGE_SIGNATURE_VOIP_CALL_ANSWER         = ((char)97);                   // 0x61
constexpr auto PROTO_MESSAGE_SIGNATURE_VOIP_ICE_CANDIDATES      = ((char)98);                   // 0x62
constexpr auto PROTO_MESSAGE_SIGNATURE_VOIP_CALL_HANGUP         = ((char)99);                   // 0x63
constexpr auto PROTO_MESSAGE_SIGNATURE_VOIP_CALL_RINGING        = ((char)100);                  // 0x64

constexpr auto PROTO_MESSAGE_SIGNATURE_RECEIPT                  = ((char)-128);                 // 0x80
constexpr auto PROTO_MESSAGE_SIGNATURE_TYPINGNOTIFICATION       = ((char)-112);                 // 0x90

// Receipt Types
constexpr auto PROTO_RECEIPT_TYPE_RECEIVED                      = ((char)1);                    // 0x01
constexpr auto PROTO_RECEIPT_TYPE_SEEN                          = ((char)2);                    // 0x02
constexpr auto PROTO_RECEIPT_TYPE_AGREE                         = ((char)3);                    // 0x03
constexpr auto PROTO_RECEIPT_TYPE_DISAGREE                      = ((char)4);                    // 0x04

// Typing Notification Types
constexpr auto PROTO_TYPINGNOTIFICATION_TYPE_STOPPED            = ((char)0);                    // 0x00
constexpr auto PROTO_TYPINGNOTIFICATION_TYPE_TYPING             = ((char)1);                    // 0x01

constexpr auto PROTO_GROUP_GROUPID_LENGTH_BYTES                 = (8);

constexpr auto PROTO_NONCE_SERVER_LENGTH_BYTES                  = (16);
constexpr auto PROTO_SERVERHELLO_CYPHERTEXT_LENGTH_BYTES        = (64);
constexpr auto PROTO_SERVERHELLO_LENGTH_BYTES                   = (PROTO_NONCE_SERVER_LENGTH_BYTES + PROTO_SERVERHELLO_CYPHERTEXT_LENGTH_BYTES);

constexpr auto PROTO_KEEPALIVE_REQUEST_MAX_LENGTH_BYTES         = (128);

constexpr auto PROTO_IMAGESERVER_ID_LENGTH_BYTES                = (16);
constexpr auto PROTO_IMAGESERVER_KEY_LENGTH_BYTES               = (24);
constexpr auto PROTO_IMAGESERVER_SIZE_LENGTH_BYTES              = (4);

constexpr auto BACKUP_LENGTH_BYTES                              = (256);
constexpr auto BACKUP_SALT_BYTES                                = (8);
constexpr auto BACKUP_IDENTITY_BYTES                            = (PROTO_IDENTITY_LENGTH_BYTES);
constexpr auto BACKUP_HASH_BYTES                                = (2);
constexpr auto BACKUP_DECODED_BYTES                             = (BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES);
constexpr auto BACKUP_ENCRYPTION_KEY_BYTES                      = (32);
constexpr auto BACKUP_KEY_PBKDF_ITERATIONS                      = (100000);

namespace openmittsu {
	namespace protocol {
		//
	}
}

#endif // OPENMITTSU_PROTOCOL_PROTOCOLSPECS_H_
