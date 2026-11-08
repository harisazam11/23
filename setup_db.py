# setup_db.py
import sqlite3

conn = sqlite3.connect('sensor_data.db')
c = conn.cursor()

c.execute('''
CREATE TABLE IF NOT EXISTS sensor_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    ch4 REAL,
    co REAL,
    nh3 REAL
)
''')
conn.commit()
conn.close()