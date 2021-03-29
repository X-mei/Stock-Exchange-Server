FROM ubuntu:18.04
RUN mkdir /code
RUN apt-get update && apt-get install -y sudo netcat g++ make libpqxx-dev libpq-dev libxerces-c-dev
RUN apt-get upgrade libstdc++6
WORKDIR /code
ADD . /code