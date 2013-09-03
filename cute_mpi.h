/*
 * cute_mpi.h
 *
 *  Created on: Sep 2, 2013
 *      Author: mc
 */

#ifndef CUTE_MPI_H_
#define CUTE_MPI_H_

#include "task.h"

int split_task(option_t &opt, int n_process, int &n_task, task_stack_t all_task[]);
void run_task(option_t &opt, task_stack_t &alltask);

// serialize a task to be ready for mpi sending
int serialize_task_stack(match_task_t &task, unsigned char serial_buffer[]);

#endif /* CUTE_MPI_H_ */
