FROM debian:latest

# Set the workdir in app cause that's what they do on the internet
WORKDIR /app

# Copy the repository into app/ so we can work
COPY . /app

RUN apt-get -y update
RUN apt-get install -y bison build-essential flex gawk git libelf-dev libncurses-dev lzop unzip
