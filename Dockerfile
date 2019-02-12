FROM debian:testing
RUN apt-get -qq update
RUN apt-get -qq install -y build-essential pkg-config libjson-glib-dev libmosquitto-dev libgps-dev meson mosquitto valgrind python3-pip python3-pytest git
ADD . /root
