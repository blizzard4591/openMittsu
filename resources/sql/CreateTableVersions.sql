CREATE TABLE `table_versions` (
	`table_name`	TEXT NOT NULL UNIQUE,
	`version`	INTEGER NOT NULL,
	PRIMARY KEY(table_name)
);