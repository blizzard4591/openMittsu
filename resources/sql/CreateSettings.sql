CREATE TABLE `settings` (
	`name`			TEXT UNIQUE,
	`is_internal`	INTEGER NOT NULL DEFAULT 0 CHECK(is_internal IN (0, 1)),
	`value`			TEXT,
	PRIMARY KEY(name)
);