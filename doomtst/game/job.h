#include "../util/topo_sort.h"
#include "../util/type_index.h"
#include "../util/dynamicarray.h"
#include <functional>
#include <thread>
#include <mutex>
#include "../util/queue.h"
#include "../util/dependency.h"
#include "../util/thread_pool.h"
#pragma once
struct Job {
virtual void run();

};

using Job_cmd = std::function<void>();
struct Job_run {
	
	std::mutex lock;
	Dag<Job_cmd> job_graph;
	stn::queue<size_t> finished;
	Job_run(Dag<Job_cmd> jobGraph):job_graph(jobGraph),finished(),lock() {
	
		if (!is_acyclic(jobGraph))
		{
			throw std::logic_error("Graph may not be acyclic");
		}
	}
	void operator()() {

		thread::thread_pool pool=thread::thread_pool(1);
		stn::array<size_t> indegree(job_graph.length());
		
		stn::queue<size_t> can_run;
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
				can_run.push(job_graph[i].index);
			}
			
		}

		while (!can_run.empty()&&pool.has_work())
		{
			lock.lock();
			while (!can_run.empty())
			{
				
					DagNode<Job_cmd> job = job_graph[can_run.pop()];
					pool.push([job, this] {this->run_cmd(job); });
					
			}

			while (!finished.empty())
			{
				auto done = finished.pop();
				for (size_t suc_ind : job_graph[done].successors)
				{
					indegree[suc_ind]--;
					if (indegree[suc_ind] == 0)
					{
						can_run.push(suc_ind);

					}
				}
			}
			lock.unlock();


		}



	}
		void run_cmd(DagNode<Job_cmd> cmd){
			cmd.value();
			lock.lock();
			finished.push(cmd.index);
			lock.unlock();
			
			
		}

};

struct JobManager {
	stn::array<Job_cmd> cmd_list;
	Depends::DependencySystem job_list;

	template<typename T>
	void push(T& job_system) {
		job_list.push(job_system);
		if (!job_list.indexer().contains<T>())
		{
			cmds.push(job_system);
		}
		throw std::logic_error("two jobs of the same type cannot be added yet");
	}
	void run() {
		Dag<Job_cmd> run_dag;
		Dag<size_t> filtered = job_list.filtered();
		for (size_t i = 0; i < filtered.length(); i++)
		{
			run_dag.addNode(cmd_list[i]);
			run_dag.nodes[ i].successors = filtered[i].successors;

		}
		Job_run(run_dag)();
	}
	JobManager():cmd_list(),job_list(){

	}
};