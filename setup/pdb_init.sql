CREATE TABLE images (
    image_id INTEGER PRIMARY KEY AUTOINCREMENT,
    original_id INTEGER, -- NULL for originals, references image_id for processed versions
    file_name TEXT NOT NULL,
    file_impath TEXT NOT NULL,
    file_size INTEGER NOT NULL,
    mime_type TEXT NOT NULL, -- e.g., 'image/jpeg', 'image/png'
    width INTEGER NOT NULL,
    height INTEGER NOT NULL,
    -- md5_hash TEXT NOT NULL, -- For duplicate detection
    processing_params TEXT, -- JSON string of processing parameters if processed
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    last_accessed TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    access_count INTEGER DEFAULT 0 NOT NULL,
    is_active BOOLEAN DEFAULT TRUE NOT NULL,
    FOREIGN KEY (original_id) REFERENCES images(image_id) ON DELETE CASCADE
);

-- Indexes for optimal performance
CREATE INDEX idx_images_original_id ON images(original_id);
CREATE INDEX idx_images_file_type ON images(file_type);
CREATE INDEX idx_images_md5_hash ON images(md5_hash);
CREATE INDEX idx_images_last_accessed ON images(last_accessed);
