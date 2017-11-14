CREATE TABLE `control_messages` (
	`identity`					TEXT,
	`apiid`						TEXT,
	`related_message_apiid`		TEXT,
	`uid`						TEXT UNIQUE,
	`is_outbox`					INTEGER NOT NULL DEFAULT 0 CHECK(is_outbox IN (0, 1)),
	`messagestate`				TEXT,
	`created_at`				INTEGER,
	`sent_at`					INTEGER,
	`modified_at`				INTEGER,
	`control_message_type`		TEXT,
	`is_queued`					INTEGER NOT NULL DEFAULT 0 CHECK(is_queued IN (0, 1)),
	`is_sent`					INTEGER NOT NULL DEFAULT 0 CHECK(is_sent IN (0, 1)),
	PRIMARY KEY(`uid`)
);
