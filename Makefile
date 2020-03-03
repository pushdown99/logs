PY   = python

TARGET=logs-config

all: $(TARGET)

$(TARGET): logs-config.py
	$(PY) logs-config.py

install:
	cp $(TARGET) ../../bin
