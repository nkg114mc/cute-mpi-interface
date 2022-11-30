# cute-mpi-interface

**cute-mpi-interface** is an interface program to run curechess-cli on the cluster system with MPI. [cutechess-cli](https://github.com/cutechess/cutechess) is a command-line tool that can run that computer chess engine-to-engine tournaments. Testing the strength of a chess engine usually requires a tournament of a large number of games. With help of a cluster system, this procedure would become much faster. cute-mpi-interface is developed for doing this.

## Prerequisite

cute-mpi-interface is only tested under Unix/Linux environment. To run a MPI application, a MPI implementation should be install first. In this project, I use MPICH3. To install MPICH3, you can either download the MPICH official website to install from source code, or if you are under Ubuntu, simply run the command below:
```
$ sudo apt-get install libcr-dev mpich mpich-doc
```
## Compiling

To compile from the source, `cd` to project root directory, then run `make` to compile and generate the executable file:
```
$ cd cute-mpi-interface
$ make
```

## Usage

### Quick Start

Generally, the format of command to run cute-mpi-inferface is as follow:
```
mpiexec -np <Number-of-Processes> <Cute-mpi-inferface-Exefile-Path> -cuteexec <Cutechess-cli-Exefile-Path> <Cutechess-cli-Arguments>
```
where `Number-of-Processes` is just the number of processes (hosts) you want to do this job on the cluster system, while `Cutechess-cli-Exefile-Path` and `Cutechess-cli-Arguments` is the cutechess-cli executable file and the original cutechess-cli arguments.

### Example

<!-- We will start this How-To by an example. -->
Assuming that we want to run a tournament between *Fruit* and *Stockfish* with cutechess-cli, 
the original command for running cutechess-cli is
```
./cutechess-cli -rounds 40 -repeat -tournament gauntlet -pgnout fruit-vs-stockfish.pgn -openings file=8moves_GM.pgn format=pgn order=random plies=16 -engine name=fruit cmd=./fruit dir=./../engines/fruit21/ proto=uci restart=on -engine name=stockfish cmd=./stockfish dir=./../engines/stockfish6 proto=uci restart=on -each tc=40/240
```
Then the corresponding command you need for cute-mpi-inferface using 10 nodes will be
```
mpiexec -np 10 ./cute-mpi-interface -cuteexec ./cutechess-cli -rounds 40 -repeat -tournament gauntlet -pgnout fruit-vs-stockfish.pgn -openings file=8moves_GM.pgn format=pgn order=random plies=16 -engine name=fruit cmd=./fruit dir=./../engines/fruit21/ proto=uci restart=on -engine name=stockfish cmd=./stockfish dir=./../engines/stockfish6 proto=uci restart=on -each tc=40/240
```

### Options

#### *cute-mpi-inference* only options:

* `-np <N>`: MPICH3 option, launch `N` MPI workers in the cluster system.
* `-cuteexec <path>`: Set the executable file path of cutechess-cli to `<path>`.
* `-shuffleopening`: Shuffle the opening pgn file or not before doing splitting. Default value is false is this option is omitted.

#### Shared options with *cutechess-cli* but have slightly different usages:

* `-tournament <type>`: In cutechess-cli, 4 types of tournaments are supported, but cute-mpi-interfance only supports `round-robin` and `gauntlet`.
* `-openings format=pgn`: In cutechess-cli, 2 opening file formats are supported, but cute-mpi-interfance only supports `pgn`.

All other options of cutechess-cli can also be used in cute-mpi-interface.

## More Details

When cute-mpi-inferface is running, it will do the following tasks in ordering:

1. Parse argument to recognize the opening file and number of rounds
1. Create a temporary folder with a name `tmp-xxxxxxxx`, where `xxxxxxxx` is a 64-bit random integer.
1. Segment the openning PGN file into N sub-files, where N = _Number-of-Processes_, and store them into the temporary folder. This is just to ensure that each sub-task must have different opennings. 
1. Segment the number of rounds in the original argument, and generate cmds for each sub-task.
Here strategy of cute-mpi-interface of doing the splitting is to split the number of rounds. For example, if a task of three engines with 1000 rounds will be split to 50 sub-tasks, then each sub-task is a tournament of that three engines with 20 rounds.
1. Run each sub-task on the clusters, and dump the game result pgn into the temporary folder.
1. Merge all result pgn files into one, and delete the temporary folder.

Note that if any sub-task failed, or the entire MPI job failed due to some reason, you can still find the finished games in the temporary folder.

<br>
Chao Ma,<br>
Sept. 2013.
