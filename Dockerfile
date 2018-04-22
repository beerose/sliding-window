FROM bluebell/sane-dev
COPY . ./workdir
WORKDIR ./workdir
RUN make
CMD ./udp-client $PORT $FILE $SIZE
