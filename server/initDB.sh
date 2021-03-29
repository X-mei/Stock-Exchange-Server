#!/bin/bash
set -e
export PGUSER=postgres
psql  <<-EOSQL
    CREATE DATABASE exchangedb;
    GRANT ALL PRIVILEGES ON DATABASE exchangedb TO postgres;
EOSQL