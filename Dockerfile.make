FROM imogen

#RUN apt-get install -y clang libsdl2-dev
RUN ls /usr/include/python3.7
# RUN git pull origin master
COPY . /app/Imogen

WORKDIR /app/Imogen

RUN clang++ -v

ENV CXX=/usr/bin/clang++
ENV CC=/usr/bin/clang

RUN cmake .
RUN make