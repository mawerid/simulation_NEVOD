# TODO create makefile for the project
# run, build with/without visualization in docker or locally
# start devcontainer
# run tests
# Variables
DOCKER_COMPOSE_FILE=docker/docker-compose.yml
SERVICE_NAME=simulation

# Targets
# .PHONY: all build run run-viz start-devcontainer test clean

all: build

build:
    docker-compose -f $(DOCKER_COMPOSE_FILE) build

run:
    docker-compose -f $(DOCKER_COMPOSE_FILE) up $(SERVICE_NAME)

run-viz:
    docker-compose -f $(DOCKER_COMPOSE_FILE) up $(SERVICE_NAME) -d --build
    docker exec -it $(SERVICE_NAME) /bin/bash -c "export DISPLAY=:0 && /workspace/build/nevod"

start-devcontainer:
    docker-compose -f $(DOCKER_COMPOSE_FILE) up -d

test:
    docker-compose -f $(DOCKER_COMPOSE_FILE) run --rm $(SERVICE_NAME) /workspace/build/nevod --test

clean:
    docker-compose -f $(DOCKER_COMPOSE_FILE) down --rmi all --volumes --remove-orphans