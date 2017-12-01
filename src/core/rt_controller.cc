/* The MIT License (MIT)

   Copyright (c) 2016 Xenofon Foukas

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <thread>
#include <iostream>

#include <pthread.h>
#include <sched.h>
#include <linux/sched.h>

#include <boost/program_options.hpp>

#include "rt_wrapper.h"

#include "async_xface.h"
#include "flexran.pb.h"
#include "rib_updater.h"
#include "rib.h"
#include "task_manager.h"
#include "stats_manager.h"
#include "remote_scheduler.h"
#include "remote_scheduler_delegation.h"
#include "remote_scheduler_eicic.h"
#include "flexible_scheduler.h"
#include "rrc_triggering.h"
#include "delegation_manager.h"
#include "requests_manager.h"
#include "neo4j_client.h"
// Fort RESTful northbound API
#ifdef REST_NORTHBOUND

#include "call_manager.h"
#include "flexible_sched_calls.h"
#include "rrc_triggering_calls.h"
#include "stats_manager_calls.h"
#include "enb_sched_policy_calls.h"

#endif

#include <pistache/endpoint.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "flexran_log.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {

  int cport = 2210;
  int north_port = 9999;
  
  bool debug = false;

  // Find the root directory
  std::string path = "";

  if(const char* env_p = std::getenv("FLEXRAN_RTC_HOME")) {
    path = env_p;
  } else {
    path = "../";
  }

  try {
    po::options_description desc("Help");
    desc.add_options()
      ("port,p", po::value<int>()->default_value(2210),
       "Port for incoming agent connections")
      ("nport,n", po::value<int>()->default_value(9999),
       "Port for northbound API calls")
      ("help,h", "Prints this help message")
      ("debug,d", "Enables debugging messages to be displayed and logged");
    
    po::variables_map opts;
    po::store(po::parse_command_line(argc, argv, desc), opts);

    if ( opts.count("help")  ) { 
      std::cout << "FlexRAN real-time controller" << std::endl 
                << desc << std::endl;
      return 0; 
    } 

    if (opts.count("debug")) {
      std::cout << "Debugging enabled" << std::endl;
      debug = true;
    }
    
    try {
      po::notify(opts);
    } catch (po::error& e) {
      std::cerr << "Error: Unrecognized parameter\n";
      return 1;
    }
    
    cport = opts["port"].as<int>(); 
    north_port = opts["nport"].as<int>();
    
  } catch(std::exception& e) {
    std::cerr << "Error: Unrecognized parameter\n";
    return 2;
  } 

  if (!debug) {
    // Initialize the logger with default properties
    flexran_log::PropertyConfigurator::configure(path + "/log_config/basic_log");
  } else {
    // Initialize the logger with debugging properties
    flexran_log::PropertyConfigurator::configure(path + "/log_config/debug_log");
  }
    
  std::shared_ptr<flexran::app::scheduler::flexible_scheduler> flex_sched_app;

  LOG4CXX_INFO(flog::core, "Listening on port " << cport << " for incoming agent connections");
  flexran::network::async_xface net_xface(cport);
  
  // Create the rib
  flexran::rib::Rib rib;

  // Create the rib update manager
  flexran::rib::rib_updater r_updater(net_xface, rib);

  // Create the task manager
  flexran::core::task_manager tm(r_updater);

  // Create the requests manager
  flexran::core::requests_manager rm(net_xface);

  // Register any applications that we might want to execute in the controller
  // Stats manager
  std::shared_ptr<flexran::app::component> stats_app(new flexran::app::stats::stats_manager(rib, rm));
  tm.register_app(stats_app);

  // Flexible scheduler
  std::shared_ptr<flexran::app::component> flex_sched(new flexran::app::scheduler::flexible_scheduler(rib, rm));
  tm.register_app(flex_sched);

#ifdef REST_NORTHBOUND
  // SCHED policy app 
  std::shared_ptr<flexran::app::component> sched_policy(new flexran::app::scheduler::enb_scheduler_policy(rib, rm));
  tm.register_app(sched_policy);
#endif

  // RRC measurements
   std::shared_ptr<flexran::app::component> rrc_trigger(new flexran::app::rrc::rrc_triggering(rib, rm));
   tm.register_app(rrc_trigger);

  
  /* More examples of developed applications are available in the commented section.
     WARNING: Some of them might still contain bugs or might be from previous versions of the controller. */
  
  // Remote scheduler
  //std::shared_ptr<flexran::app::component> remote_sched(new flexran::app::scheduler::remote_scheduler(rib, rm));
  //tm.register_app(remote_sched);

  // eICIC remote scheduler
  //std::shared_ptr<flexran::app::component> remote_sched_eicic(new flexran::app::scheduler::remote_scheduler_eicic(rib, rm));
  //tm.register_app(remote_sched_eicic);

  // Remote scheduler with delegation (TEST purposes)
  // std::shared_ptr<flexran::app::component> remote_sched(new flexran::app::scheduler::remote_scheduler_delegation(rib, rm));
  // tm.register_app(remote_sched);
  
  // Delegation manager (TEST purposes)
  //std::shared_ptr<flexran::app::component> delegation_manager(new flexran::app::management::delegation_manager(rib, rm));
  //tm.register_app(delegation_manager);

 // std::shared_ptr<flexran::app::component> n4j_client(new flexran::app::management::neo4j_client(rib, rm));
 // tm.register_app(n4j_client);


  // Start the network thread
  std::thread networkThread(&flexran::network::async_xface::execute_task, &net_xface);

  // Start the task manager thread
  std::thread task_manager_thread(&flexran::core::task_manager::execute_task, &tm);

#ifdef REST_NORTHBOUND
  
  // Initialize the northbound API

  // Set the port and the IP to listen for REST calls and initialize the call manager
  Pistache::Port port(north_port);
  Pistache::Address addr(Pistache::Ipv4::any(), port);
  flexran::north_api::manager::call_manager north_api(addr);

  // Register API calls for the developed applications
  flexran::north_api::enb_sched_policy_calls policy_calls(std::dynamic_pointer_cast<flexran::app::scheduler::enb_scheduler_policy>(sched_policy));

  // REgister Rrc Triggering Application
   flexran::north_api::rrc_triggering_calls rrc_calls(std::dynamic_pointer_cast<flexran::app::rrc::rrc_triggering>(rrc_trigger));  

  // Register API calls for the developed applications
  flexran::north_api::flexible_sched_calls scheduler_calls(std::dynamic_pointer_cast<flexran::app::scheduler::flexible_scheduler>(flex_sched));

  flexran::north_api::stats_manager_calls stats_calls(std::dynamic_pointer_cast<flexran::app::stats::stats_manager>(stats_app));
  
   north_api.register_calls(rrc_calls);
  north_api.register_calls(policy_calls);
  north_api.register_calls(scheduler_calls);
  north_api.register_calls(stats_calls);

  // Start the call manager threaded. Once task_manager_thread and
  // networkThread return, north_api will be shut down too
  north_api.init(1);
  north_api.start();
#endif

  if (task_manager_thread.joinable())
    task_manager_thread.join();
  
  if (networkThread.joinable())
    networkThread.join();

#ifdef REST_NORTHBOUND
  north_api.shutdown();
#endif

  return 0;
}
