CREATE TABLE `media` (
	`uid`		TEXT,
	`type`		INTEGER DEFAULT 1,
	`size`		INTEGER NOT NULL,
	`checksum`	INTEGER NOT NULL,
	`nonce`		TEXT NOT NULL,
	`key`		TEXT NOT NULL,
	PRIMARY KEY(`uid`,`type`)
);