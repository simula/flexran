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

#ifndef FLEXIBLE_SCHEDULER_H_
#define FLEXIBLE_SCHEDULER_H_

#include "periodic_component.h"
#include "enb_scheduling_info.h"
#include "ue_scheduling_info.h"
#include "rib_common.h"

#include <atomic>

namespace flexran {

  namespace app {

    namespace rrc {

      class rrc_measurements : public periodic_component {

      public:

	rrc_measurements(rib::Rib& rib, const core::requests_manager& rm)
	  : periodic_component(rib, rm) {

	  // central_scheduling.store(false);
	  
	}

	// void run_periodic_task();

	// void push_code(int agent_id, std::string function_name, std::string lib_name);

	void reconfigure_agent(int agent_id);

	void enable_central_scheduling(bool central_sch);
	
	// static int32_t tpc_accumulated;

      private:

	// void run_central_scheduler();
	
	// ::std::shared_ptr<enb_scheduling_info> get_scheduling_info(int agent_id);
	
	// ::std::map<int, ::std::shared_ptr<enb_scheduling_info>> scheduling_info_;
	
	// Set these values internally for now

	// std::atomic<bool> central_scheduling;
	const int schedule_ahead = 0;
	// bool code_pushed_;
	// int prev_val_, current_val;
	
      };
      
    }
    
  }

}


#endif /* RRC_MEASUREMENTS_H_ */