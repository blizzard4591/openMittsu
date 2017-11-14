CREATE TABLE `contacts` (
	`identity`					TEXT UNIQUE,
	`publickey`					TEXT,
	`verification`				TEXT,
	`acid`						TEXT,
	`tacid`						TEXT,
	`firstname`					TEXT,
	`lastname`					TEXT,
	`nick_name`					TEXT,
	`color`						INTEGER,
	`status`					INTEGER NOT NULL DEFAULT -1 CHECK(status IN (-1, 0, 1, 2)),
	`status_last_check`			INTEGER NOT NULL DEFAULT -1,
	`feature_level`				INTEGER NOT NULL DEFAULT -2 CHECK(feature_level IN (-2, -1, 0, 1, 2, 3)),
	`feature_level_last_check`	INTEGER NOT NULL DEFAULT -1,
	PRIMARY KEY(`identity`)
);