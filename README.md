cute-mpi-interface
==================

**cute-mpi-interface** is an interface program to run curechess-cli on the cluster system with MPI. [cutechess-cli](https://github.com/cutechess/cutechess) is a command-line tool that can run that computer chess engine-to-engine tournaments. Testing the strength of a chess engine usually requires a tournament of a large number of games. With help of a cluster system, this procedure would become much faster. cute-mpi-interface is just designed for doing this.

## Prerequisite

cute-mpi-interface is only developed and tested under Linux environment. To run a MPI application, you need to install a MPI implementation. In this project, I use MPICH3. To install MPICH3, you either download the MPICH official website to install from source code, or if you are under Ubuntu, simply run the command below:

	$ sudo apt-get install libcr-dev mpich mpich-doc

## Compiling

To compile the program, cd to project root directory, then use make to get the executable file.
	$ make

## Run

We will start this How-To by an example. Assume that we want to run a tournament between Fruit and Stockfish with cutechess-cli.
The original command you used to run cutechess-cli is

	./cutechess-cli -rounds 40 -repeat -tournament gauntlet -pgnout fruit-vs-stockfish.pgn -openings file=8moves_GM.pgn format=pgn order=random plies=16 -engine name=fruit cmd=./fruit dir=./../engines/fruit21/ proto=uci restart=on -engine name=stockfish cmd=./stockfish dir=./../engines/stockfish6 proto=uci restart=on -each tc=40/240

Then the corresponding command you need for cute-mpi-inferface is

	mpiexec -np <Number-of-Processes> ./cute-mpi-interface -cuteexec ./cutechess-cli -rounds 40 -repeat -tournament gauntlet -pgnout fruit-vs-stockfish.pgn -openings file=8moves_GM.pgn format=pgn order=random plies=16 -engine name=fruit cmd=./fruit dir=./../engines/fruit21/ proto=uci restart=on -engine name=stockfish cmd=./stockfish dir=./../engines/stockfish6 proto=uci restart=on -each tc=40/240

More generally, the format of command to run cute-mpi-inferface is as follow:

	mpiexec -np <Number-of-Processes> <Cute-mpi-inferface-Exefile-Path> -cuteexec <Cutechess-cli-Exefile-Path> <Cutechess-cli-Arguments>

where _Number-of-Processes_ is just the number of processes (hosts) you want to do this job on the cluster system, while _Cutechess-cli-Exefile-Path_ and _Cutechess-cli-Arguments_ is the cutechess-cli executable file and the original cutechess-cli arguments.

## More Details

When cute-mpi-inferface is running, it will do the following tasks in ordering:

1. Parse argument to recognize the opening file and number of rounds
2. Create a temporary folder with a name "tmp-xxxxxxxx", where xxxxxxxx is just a random integer.
3. Segment the openning PGN file into N sub-files, where N = _Number-of-Processes_, and store them into the temporary folder. This is just to ensure that each sub-task will have different opennings. 
4. Segment the number of rounds in the original argument, and generate cmds for each sub-task.
Here strategy of cute-mpi-interface of doing the splitting is to split the number of rounds. For example, if a task of three engines with 1000 rounds will be split to 50 sub-tasks, then each sub-task is a tournament of that three engines with 20 rounds.
5. Run each sub-task on the clusters, and dump the game result pgn into the temporary folder.
6. Merge all result pgn files into one, and delete the temporary folder.

Note that if any sub-task failed, or the entire MPI job failed due to some reason, you can still find the finished games in the temporary folder.


Chao Ma,
Sept. 2013.
