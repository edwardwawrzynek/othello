# Othello

An othello ai, specifically designed to play in the [codekata-othello](https://github.com/henrymwestfall/codekata-othello-flask) competition.

## Building
The project requires a C++-17 compiler, as well as the [nlohmann json](https://github.com/nlohmann/json) library to be installed.

The project uses cmake as its build system:
```sh
mkdir build
cd build
cmake ..
make othello
```
(results in an othello executable in the `build` folder).

## Usage
`othello URL KEY NAME SEARCH_TIME`

where:
* `URL` is the url of the codekata-othello server
* `KEY` is the key for the codekata-othello server
* `NAME` is the name to send the codekata-othello server
* `SEARCH_TIME` is the time, in seconds, to spend searching for each move
