# models.py
from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy()

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    ch4 = db.Column(db.Integer)
    co = db.Column(db.Integer)
    o2 = db.Column(db.Float)
    timestamp = db.Column(db.DateTime, server_default=db.func.now())