FROM ubuntu:18.04

RUN bash -c 'echo "nameserver 1.1.1.1" >> /etc/resolv.conf'
RUN apt-get update
RUN apt-get install -y build-essential cmake libffi-dev libgtk-3-dev git
python3-dev python3.7-dev clang libavcodec-dev libc++abi libc++

#WORKDIR /app
#RUN git clone https://github.com/CedricGuillemet/Imogen.git
#RUN ls -lah
#WORKDIR /app/Imogen/build
#RUN cmake ..
#RUN make