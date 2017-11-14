CREATE TABLE `feature_levels` (
	`identity`	TEXT NOT NULL UNIQUE,
	`feature_level`	INTEGER NOT NULL DEFAULT 0,
	PRIMARY KEY(identity)
);