
#include <scorep/plugin/plugin.hpp>


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>






using namespace scorep::plugin::policy;

using scorep::plugin::logging;

class cpuidle_plugin
  : public scorep::plugin::base<cpuidle_plugin, async, post_mortem, scorep_clock, once>{
	
	static bool is_pinned()
	{
		cpu_set_t cpumask;
		sched_getaffinity(0, sizeof(cpu_set_t), &cpumask);

		return CPU_COUNT(&cpumask) == 1;
	}

	static int get_current_cpu()
    	{
		int res = sched_getcpu();

		if (res < 0)
		{
			scorep::exception::raise("Failed to get current cpu for current thread");
		}

		return res;
	}
	void setMeasure(uint32_t cpu, int flag){
		FILE *ft;
		char name[55];
		int ch;	
		snprintf(name, sizeof(name), "/sys/devices/system/cpu/cpu%u/cpuidle/measure/measure",cpu);
	
		ft=fopen(name, "w");
		if(ft==NULL){
			printf("Error opening file! %s\n", name);
			exit(1);	
		}
		fprintf(ft, "%u\n", flag);
		fclose(ft);
	}

public:


	std::vector<scorep::plugin::metric_property> 
	get_metric_properties(const std::string& cpu)
	{
		cpus_.push_back(std::stoi(cpu));
		std::stringstream name;
		char hostname[128];
		gethostname(hostname, sizeof hostname);
		name << hostname << "/cpu" << cpu << "/cstate";
		return { scorep::plugin::metric_property(name.str(), "Cstate Measurements", "#")
			     .absolute_next()
			     .value_uint() };
	}
	
	int32_t 
	add_metric(const std::string& metric_name)
	{
		int cpu;
		std::cmatch cm;
		std::regex e (".*cpu(\\d*)/.*");
		std::regex_match ( metric_name.c_str(), cm, e);
		cpu = 0;
		if(cm.size()>0){		
			cpu = std::stoi(cm[1]);
		}else{
			scorep::plugin::log::logging::warn() <<
			"Could not identify CPU for data collection.";
		}
		if (!is_pinned())
		{
			scorep::plugin::log::logging::warn() <<
			"Thread is not pinned to one specific CPU. You're swimming in new waters.";

			cpu = 0;
		}


		return static_cast<int>(cpu);
	}
	
	void start()
	{
        	begin = scorep::chrono::measurement_clock::now();
		//set measure flag for cpus under observation
		for (auto cpu : cpus_){
			setMeasure(cpu, 1);
		}
		
	}

	void stop()
	{
		//unset measure flag
		for (auto cpu : cpus_){
			setMeasure(cpu, 0);
		}
	        end = scorep::chrono::measurement_clock::now();
	}

	template <typename C>
	void get_all_values(int32_t cpu, C& c)
	{
		scorep::plugin::log::logging::debug() << "Get values called on CPU #" << cpu;

		//init cstate always 0
		c.write(begin, (std::uint64_t) 0);
		
	       	std::stringstream path;
		path << "/sys/devices/system/cpu/cpu" << std::to_string(cpu) << "/cpuidle/measure/result";
		std::ifstream infile(path.str());
		std::string line;
		while (std::getline(infile, line))
		{
		
			std::stringstream ss(line);
			std::string tok;
			std::vector<std::string> temp;
			while(std::getline(ss,tok,',')){
				
				temp.push_back(tok);
			}
			if(temp.size()!=3){ 
				//occurs at end of file	
				break; 
			}
			if(stoi(temp[2])!=-1 && stoul(temp[1],nullptr,0)!=0){
				
				std::uint64_t start, dur, cstate;
				
				start = stoul(temp[0],nullptr,0);
				dur = stoul(temp[1],nullptr,0);
				cstate = stoul(temp[2],nullptr,0);
				if(start>begin.count()){

			    		c.write(scorep::chrono::ticks(start), cstate);
			    		c.write((scorep::chrono::ticks(start) + scorep::chrono::ticks(dur)), (std::uint64_t) 0);
	            		}
			}
		
		}
	       	
		
	       
	}

private:
    scorep::chrono::ticks begin, end;
    std::vector<unsigned int> cpus_;
	
};



SCOREP_METRIC_PLUGIN_CLASS(cpuidle_plugin, "cpuidle")
