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
    sequence        INTEGER             NOT NULL
);

CREATE TABLE clips_groups(
    clip_id     INTEGER NOT NULL,
    group_id    INTEGER NOT NULL,
    FOREIGN KEY(clip_id) REFERENCES clips(id),
    FOREIGN KEY(group_id) REFERENCES groups(id),
    PRIMARY KEY(clip_id, group_id)
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
