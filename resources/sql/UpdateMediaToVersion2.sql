CREATE TABLE `openmittsu_upgrade_table_media` (
	`uid`	TEXT,
	`type`	INTEGER DEFAULT 1,
	`size`	INTEGER NOT NULL,
	`checksum`	INTEGER NOT NULL,
	`nonce`	TEXT NOT NULL,
	`key`	TEXT NOT NULL,
	PRIMARY KEY(`uid`,`type`)
);
__OPENMITTSU_QUERY_SEP__
INSERT INTO openmittsu_upgrade_table_media SELECT `uid`, 1 as `type`,`size`,`checksum`,`nonce`,`key` FROM `media`;
__OPENMITTSU_QUERY_SEP__
PRAGMA defer_foreign_keys = "1";
__OPENMITTSU_QUERY_SEP__
DROP TABLE `media`;
__OPENMITTSU_QUERY_SEP__
ALTER TABLE `openmittsu_upgrade_table_media` RENAME TO `media`;
__OPENMITTSU_QUERY_SEP__
PRAGMA defer_foreign_keys = "0";
