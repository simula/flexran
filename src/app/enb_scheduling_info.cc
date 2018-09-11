/*
 * Copyright 2016-2018 FlexRAN Authors, Eurecom and The University of Edinburgh
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * For more information about Mosaic5G:  contact@mosaic-5g.io
 */

/*! \file    enb_scheduling_info.cc
 *  \brief   information for central scheduling
 *  \authors Xenofon Foukas
 *  \company Eurecom
 *  \email   x.foukas@sms.ed.ac.uk
 */

#include "enb_scheduling_info.h"
#include "remote_scheduler_primitives.h"

std::shared_ptr<flexran::app::scheduler::ue_scheduling_info>
flexran::app::scheduler::enb_scheduling_info::get_ue_scheduling_info(flexran::rib::rnti_t rnti) {
  auto it = scheduling_info_.find(rnti);
  if (it != scheduling_info_.end()) {
    return it->second;
  } else {
    return std::shared_ptr<ue_scheduling_info>(nullptr);
  }
}

void flexran::app::scheduler::enb_scheduling_info::create_ue_scheduling_info(flexran::rib::rnti_t rnti) {
  scheduling_info_.insert(std::pair<flexran::rib::rnti_t,
			  std::shared_ptr<ue_scheduling_info>> (rnti,
								std::shared_ptr<ue_scheduling_info>(new ue_scheduling_info(rnti))));
}


void flexran::app::scheduler::enb_scheduling_info::increase_num_pdcch_symbols(const protocol::flex_cell_config& cell_config,
									     flexran::rib::subframe_t subframe) {
  int cell_id = cell_config.cell_id();
  uint32_t prev_nCCE_max = get_nCCE_max(num_pdcch_symbols_[cell_id],
					cell_config,
					subframe);
  num_pdcch_symbols_[cell_id]++;
  uint32_t nCCE_max = get_nCCE_max(num_pdcch_symbols_[cell_id],
				   cell_config,
				   subframe);
  nCCE_rem_[cell_id] = nCCE_max - (prev_nCCE_max - nCCE_rem_[cell_id]); 
				   
}

void flexran::app::scheduler::enb_scheduling_info::start_new_scheduling_round(flexran::rib::subframe_t subframe,
									     const protocol::flex_cell_config& cell_config) {
  
  int cell_id = cell_config.cell_id();
  
  std::fill(vrb_map_[cell_id], vrb_map_[cell_id] + rib::N_RBG_MAX, 0);
  num_pdcch_symbols_[cell_id] = 0;
  
  nCCE_rem_[cell_id] = get_nCCE_max(num_pdcch_symbols_[cell_id],
				    cell_config,
				    subframe);
  
  int n_rb_dl = cell_config.dl_bandwidth();
  
  // Check if we have other things to schedule as well (OAI specific for the moment)
  if ((subframe == 0) || (subframe == 5)) {
    // We will need at least one symbol for the scheduling
    num_pdcch_symbols_[cell_id] = 3;
    // Set the first 4 RBs for RA
    vrb_map_[cell_id][0] = 1;
    vrb_map_[cell_id][1] = 1;
    vrb_map_[cell_id][2] = 1;
    vrb_map_[cell_id][3] = 1;
  }

  // Check if we have an SI transmission as well
  if (subframe == 5) {
    int index = 0;
    switch(n_rb_dl) {
    case 6:
      index = 0;
      break;
    case 15:
      index = 6;
      break;
    case 25:
      index = 11;
      break;
    case 50:
      index = 23;
      break;
    case 100:
      index = 48;
      break;
    default:
      index = 0;
      break;
    }
    vrb_map_[cell_id][index] = 1;
    vrb_map_[cell_id][index+1] = 1;
    vrb_map_[cell_id][index+2] = 1;
    vrb_map_[cell_id][index+3] = 1;
  }
}
