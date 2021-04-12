To run the server, go into the server directory and type `make`, then step back and type `sudo docker-compose up`.
This will start the exchange server.

To run the client, go into the testing directory and `make`, use `./client testxmls/test#.xml` to test, you need to change the host address to the ip of the machine running the docker program.
