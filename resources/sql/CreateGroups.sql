CREATE TABLE `groups` (
	`id`				TEXT NOT NULL,
	`creator`			TEXT NOT NULL,
	`groupname`			TEXT,
	`created_at`		INTEGER,
	`members`			TEXT NOT NULL,
	`avatar_uuid`		TEXT,
	`is_deleted`		INTEGER NOT NULL DEFAULT 0 CHECK(is_deleted IN (0, 1)),
	`is_awaiting_sync`	INTEGER NOT NULL DEFAULT 0 CHECK(is_awaiting_sync IN (0, 1)),
	PRIMARY KEY(`id`,`creator`)
);