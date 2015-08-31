CREATE TABLE groups(
    id              INTEGER PRIMARY KEY NOT NULL,
    parent_group_id INTEGER,
    name            TEXT,
    sequence        INTEGER             NOT NULL,
    FOREIGN KEY(parent_group_id) REFERENCES groups(id)
);

CREATE TABLE clips(
    id              INTEGER PRIMARY KEY NOT NULL,
    time            DATETIME,
    name            TEXT                NOT NULL,
    owner_group_id  INTEGER,
    sequence        INTEGER             NOT NULL,
    FOREIGN KEY(owner_group_id) REFERENCES groups(id)
);

CREATE TABLE clip_data(
    id              INTEGER PRIMARY KEY NOT NULL,
    clip_id         INTEGER             NOT NULL,
    mimetype        TEXT                NOT NULL,
    text_contents   TEXT,
    binary_contents BLOB,
    FOREIGN KEY(clip_id) REFERENCES clips(id)
);
CREATE UNIQUE INDEX clip_data_mime_index
ON clip_data(clip_id,mimetype);
CREATE INDEX clip_data_contents_index
ON clip_data(text_contents);

CREATE TABLE mru_clips(
    id              INTEGER PRIMARY KEY NOT NULL,
    clip_id         INTEGER             NOT NULL UNIQUE,
    FOREIGN KEY(clip_id) REFERENCES clips(id)
);

PRAGMA foreign_keys = ON;
