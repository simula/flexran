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

#ifndef REMOTE_SCHEDULER_HELPER_H_
#define REMOTE_SCHEDULER_HELPER_H_

#include "flexran.pb.h"
#include "rib_common.h"

#include "periodic_component.h"
#include "enb_scheduling_info.h"
#include "ue_scheduling_info.h"

namespace flexran {

  namespace app {

    namespace scheduler {

      class remote_scheduler_helper {

      private:
	
	struct ue_stats {
	  
	  int ue_id;
	  int bytes_in_ccch;
	  int total_bytes_in_buffers;
	  int hol_delay;
	  int cqi;
	  int harq_round;

	ue_stats():ue_id(0),bytes_in_ccch(0),total_bytes_in_buffers(0),hol_delay(0),cqi(0),harq_round(0){}
	  
	};

	static bool compare_stats(const ue_stats& a, const ue_stats& b);
	
      public:
      
	static void run_dlsch_scheduler_preprocessor(const protocol::flex_cell_config& cell_config,
						     const protocol::flex_ue_config_reply& ue_configs,
						     const protocol::flex_lc_config_reply& lc_configs,
						     ::std::shared_ptr<rib::enb_rib_info> agent_config,
						     ::std::shared_ptr<enb_scheduling_info> sched_info,
						     rib::frame_t frame,
						     rib::subframe_t subframe);
      
	static void assign_rbs_required(::std::shared_ptr<ue_scheduling_info> ue_sched_info,
					::std::shared_ptr<rib::ue_mac_rib_info> ue_mac_info,
					const protocol::flex_cell_config& cell_config,
					const protocol::flex_lc_ue_config& lc_ue_config);
	
	static void perform_pre_processor_allocation(const protocol::flex_cell_config& cell_config,
						     std::shared_ptr<enb_scheduling_info> sched_info,
						     std::shared_ptr<ue_scheduling_info> ue_sched_info,
						     int transmission_mode);


	static const std::shared_ptr<std::vector<int>> sort_UEs(const protocol::flex_cell_config& cell_config,
								const protocol::flex_ue_config_reply& ue_configs,
								const ::std::shared_ptr<rib::enb_rib_info> agent_config); 
	  	
      };
    }

  }

}

#endif
