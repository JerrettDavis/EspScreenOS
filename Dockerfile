FROM espressif/idf:v5.4.2
WORKDIR /workspace
COPY . /workspace
RUN python tools/espscreen.py validate --all && python tools/espscreen.py generate --all --check
CMD ["bash"]
