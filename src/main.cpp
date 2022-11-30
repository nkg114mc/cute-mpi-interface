#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>
#include <unistd.h>

#include "util.h"
#include "task.h"
#include "open_split.h"
#include "cute_mpi.h"

using namespace std;

#define SERIAL_TASK_BUFFER_SIZE 65536


void parse_arg(int argc, char* argv[], option_t &opt)
{
	int i, j;
	string token;
	string val;
	string other_option = "";

	other_option = "";
	for (i = 0; i < argc; i++) {
		token = string(argv[i]);

		if (token[0] == '-') { // this is an option

			if (token == "-tournament") {
				val = string(argv[i + 1]);
				opt.tournament_type = val;
				cout << "Parsing tournament type: " << val << endl;

			} else if (token == "-pgnout") {
				val = string(argv[i + 1]);
				opt.pgnout = val;

			} else if (token == "-engine") {
				string mul_val = "";
				for (j = (i + 1); j < argc; j++) {
					string tmp_tk = string(argv[j]);
					if (tmp_tk[0] != '-') {
						mul_val = mul_val + " " + tmp_tk;
					} else {
						break;
					}
				}
				opt.engine_infos.push_back(mul_val);

			} else if (token == "-rounds") {
				val = string(argv[i + 1]);
				opt.rounds = str2int(val);

			} else if (token == "-openings") {
				string mul_val = "";
				for (j = (i + 1); j < argc; j++) {
					string tmp_tk = string(argv[j]);
					if (tmp_tk[0] != '-') {
						mul_val = mul_val + " " + tmp_tk;
					} else {
						break;
					}
				}
				opt.openning_info_str = mul_val;
			//} else if (token == "-initfile") {

			} else if (token == "-shuffleopening") {
				opt.shuffle_opening = true;

			} else if (token == "-cuteexec") {
				opt.cute_exe_path = string(argv[i + 1]);

			} else { // all other options
				other_option = other_option + " " + token;
				for (j = (i + 1); j < argc; j++) {
					string tmp_tk = string(argv[j]);
					if (tmp_tk[0] != '-') {
						other_option = other_option + " " + tmp_tk;
					} else {
						i = j - 1;
						break;
					}
				}
				opt.other_option = other_option;
			}
		}
	}

	cout << "==== Tournament Info ====" << endl;
	cout << "cutechess-cli exe path = " << opt.cute_exe_path << endl;
	cout << "tournament_type = " << opt.tournament_type << endl;
	cout << "openning_info_str = " << opt.openning_info_str << endl;
	cout << "pgnout = " << opt.pgnout << endl;
	cout << "rounds = " << opt.rounds << endl;
	cout << "n_engine = " << opt.get_n_engine() << endl;
	for (i = 0; i < opt.get_n_engine(); i++) {
		cout << "engine_info[" << i << "] = " << opt.engine_infos[i] << endl;
	}
	cout << "other_option = " << opt.other_option << endl;
	cout << "=========================" << endl;
}

void clear_arg(option_t &opt)
{
	opt.rounds = 0;
	opt.tournament_type = "";
	opt.engine_infos.clear();
	opt.openning_info_str = "";
	opt.other_option = "";
	opt.pgnout = "";
	opt.shuffle_opening = false;

	opt.cute_exe_path = "cutechess-cli"; // default cutechess-cli exe file name

	// all subpgn
	opt.sub_pgnout.clear();
}

int main(int argc, char* argv[])
{
	option_t main_opt;
	task_stack_t all_task[MAX_WORKERS_N];
	task_stack_t mytask;
	int myid, nprocs, namelen, master, size, rank;
	int ierr, i, j, n_task;
	char processor_name[1024];
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Get_processor_name(processor_name, &namelen);

	const int MASTER_RANK = 0;
	const string start_cmd = "START";
	const string useless_cmd = "ENDUP";
	const string end_cmd = "FINISH";
	char recvbuff[256];

	size = nprocs;
	rank = myid;

	// clear argument
	clear_arg(main_opt);

	cout << "Process[" << rank << "] launched!" << endl;
	usleep(100000);

	// prepare
	if (rank == MASTER_RANK) {
		bool success_split = false;
		n_task = 0;

		cout << "Master process start preprocessing!\n" << endl;
		// Preprocess =======================================

		// 1) parse arguments in option_t
		parse_arg(argc, argv, main_opt);

		// 2) split the big task into small ones
		success_split = split_task(main_opt, size, n_task, all_task);
		if (!success_split) {
			for (j = 1; j < size; j++) {
				ierr = MPI_Send(useless_cmd.c_str(), 5, MPI_CHAR, j, 0, MPI_COMM_WORLD);
			}
			goto endup;
		}
		// End of preprocess ================================

		// all is ready, now begin!
		for (j = 1; j < size; j++) {
			if (j < n_task) {
				ierr = MPI_Send(start_cmd.c_str(), 5, MPI_CHAR, j, 0, MPI_COMM_WORLD);
			} else {
				ierr = MPI_Send(useless_cmd.c_str(), 5, MPI_CHAR, j, 0, MPI_COMM_WORLD);
			}
		}

		sleep(1); // delay for a while

		// task for processor 0
		for (j = 0; j < all_task[0].stack.size(); j++) {
			mytask.stack.push_back(all_task[0].stack[j]);
		}

		// sent all tasks for other processers
		for (j = 1; j < n_task; j++) {
			int stack_size = all_task[j].stack.size();
			// what size of this task_stack?
			ierr = MPI_Send(&stack_size, sizeof(int), MPI_INT, j, 0, MPI_COMM_WORLD);

			unsigned char serial_buffer[SERIAL_TASK_BUFFER_SIZE];
			for (int k = 0; k < stack_size; k++) {
				int byte_size = serialize_task_stack(all_task[j].stack[k], serial_buffer);
				ierr = MPI_Send(serial_buffer, SERIAL_TASK_BUFFER_SIZE, MPI_BYTE, j, 0, MPI_COMM_WORLD);
			}
		}

	} else {

		while (1) {
			ierr = MPI_Recv(recvbuff, 5, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
			if (recvbuff[0] == 'S' &&
					recvbuff[1] == 'T' &&
					recvbuff[2] == 'A' &&
					recvbuff[3] == 'R' &&
					recvbuff[4] == 'T') {
				cout << "Process [" << rank << "] get START message!" << endl;
				break;
			} else if (recvbuff[0] == 'E' &&
					recvbuff[1] == 'N' &&
					recvbuff[2] == 'D' &&
					recvbuff[3] == 'U' &&
					recvbuff[4] == 'P') {
				cout << "Process [" << rank << "] get ENDUP message! Exit now!" << endl;
				goto endup;
				break;
			}
		}

		// reading stack size
		int stack_size = 0;
		ierr = MPI_Recv(&stack_size, sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		cout << "Process [" << rank << "] recieve " << stack_size << " tasks!" << endl;

		char serial_buffer[SERIAL_TASK_BUFFER_SIZE];
		for (int k = 0; k < stack_size; k++) {
			ierr = MPI_Recv(&serial_buffer, SERIAL_TASK_BUFFER_SIZE, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
			match_task_t recieve_task;
			recieve_task.cmd = string(serial_buffer);
			mytask.stack.push_back(recieve_task);
			cout << "Process [" << rank << "] get cmd: " << recieve_task.cmd << endl;
		}

	}

	// ========================
	cout << "Process [" << rank << "] start working!" << endl;

	run_task(rank, mytask);

	cout << "Process [" << rank << "] finish working!" << endl;
	// ========================

	// post process
	if (rank == MASTER_RANK) {
		int finish[512];
		int n_task = size, i;

		for (i = 1; i < size; i++) {
			finish[i] = 0;
		}

		while (1) {
			int msg_flag = 1;
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (msg_flag) {
				int message_id = status.MPI_TAG;
				int msg_source = status.MPI_SOURCE;
				cout << "A message from process[" << msg_source << "]!!!\n";

				ierr = MPI_Recv(recvbuff, 6, MPI_CHAR, msg_source, 0, MPI_COMM_WORLD, &status);
				if (recvbuff[0] == 'F' &&
					recvbuff[1] == 'I' &&
					recvbuff[2] == 'N' &&
					recvbuff[3] == 'I' &&
					recvbuff[4] == 'S' &&
					recvbuff[5] == 'H')
				{
					cout << "Get FINISH message from process [" << msg_source << "]!" << endl;
					finish[msg_source] = 1;
				}
				bool all_finsh = true;
				for (i = 1; i < n_task; i++) {
					if (finish[i] == 0) {
						cout << "Process " << i << " is still working!\n";
						all_finsh = false;
					}
				}
				if (all_finsh) {
					break;
				}
				cout << "Still waiting!\n";
			}
		}

		// all child process finished working
		// POSRPROCESS =================================================
		// 4) merge all result files
		file_merge(main_opt.pgnout, main_opt.sub_pgnout);

		// 5) delete all tmp dirs and files
		//system("");
		//system("rm -f ./notes.txt");
		// =============================================================
		printf("All Done!!!!\n");

	} else {
		// just exit
		printf("Process %d of %d ready to send 'FINISH'!\n", rank, size);
		ierr = MPI_Send(end_cmd.c_str(), 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		printf("Process %d of %d finish working, now exit!\n", rank, size);
	}

endup:
	MPI_Finalize();

	return 0;
}
