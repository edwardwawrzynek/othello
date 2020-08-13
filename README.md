# Othello

An othello ai, specifically designed to play in the [codekata-othello](https://github.com/henrymwestfall/codekata-othello-flask) competition.

## Building
The project requires a C++-17 compiler, as well as the [nlohmann json](https://github.com/nlohmann/json) library to be installed.

The project uses cmake as its build system:
```
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

## Algorithm
The AI uses a minimax search algorithm.

It evaluates based on:
* Win/Loss
* Maximizing mobility (number of moves available)
* Minimizing frontier during midgame (number of disks with open spaces next to them)
* Taking corners + avoiding squares next to corners

The AI starts the minimax at 1 move deep, then searches 2 moves deep, then 3, etc (iterative deepening). This allows it to search as deep as it can in the given time and always have a move available.

The search algorithm caches all board positions it sees in a transposition table, which allows it to avoid searching the same position twice. Combined with an alpha-beta pruning algorithm and iterative deepening, the transposition table allows the search algorithm to examine moves that scored higher in previous (lower depth) searches first.

## Performance

On my machine (i5-2435), with a 5s search time:
* ~3M board evaluations/second
* ~13 moves deep search during midgame
* Endgame solved ~15-20 moves out
