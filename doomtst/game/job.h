/*#include "../util/topo_sort.h"
#include "../util/type_index.h"
#include "../util/dynamicarray.h"
#include <functional>
#include <thread>
#include <mutex>
#include "../util/queue.h"
#pragma once
struct Job {

	

};

struct Job_cmd {
	Job_cmd() {

	}
	size_t id;
	std::function<void()> run;
	Job_cmd(const Job_cmd& oth) {
		run = oth.run;
		id = oth.id;
	}
};
struct Job_run {
	std::mutex lock;
	Dag<Job_cmd> job_graph;
	Cont::queue<Job_cmd> finished;
	void run() {

		size_t threads = 4;
		Cont::queue<Job_cmd> running;
		Cont::array<size_t> indegree(job_graph.length());
		Cont::array<Job_cmd> to_run;
		Cont::queue<Job_cmd> can_run;
		for (size_t i = 0; i < job_graph.length(); i++)
		{
			DagNode<Job_cmd> graph_node = job_graph[i];
			for (size_t indice : graph_node.successors)
			{
				indegree[indice]++;
			}
		}
		for (size_t i = 0; i < job_graph.length(); i++)
		{
			if (indegree[i] == 0)
			{
				can_run.push(job_graph[i].value);
			}
			else
			{
				to_run.push(job_graph[i].value);
			}
		}

		while (to_run.length != 0)
		{
			lock.lock();
			while (running.length() < threads && !can_run.empty())
			{
				
					Job_cmd job = can_run.pop();
					std::thread thread(run_cmd, job);
					thread.detach();
			}
			while (!finished.empty())
			{
				auto done = finished.pop();
				for (size_t suc_ind : job_graph[done.id].successors)
				{
					indegree[suc_ind]--;
					if (indegree[suc_ind] == 0)
					{
						can_run.push(job_graph[suc_ind].value);

					}
				}
			}
			lock.unlock();


		}



	}
		void run_cmd(Job_cmd cmd){
			cmd.run();
			lock.lock();
			finished.push(cmd);
			lock.unlock();
			
			
		}

			//if one job completes
			//subtract 1 from the indegree of the depencies 
			//choose 
};

struct JobManager {
	
	

	void run() {

	}
	Dag<Job_cmd> current;

};*/