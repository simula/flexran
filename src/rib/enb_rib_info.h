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

#ifndef ENB_RIB_INFO_H_
#define ENB_RIB_INFO_H_

#include <ctime>
#include <map>
#include <memory>

#include "flexran.pb.h"
#include "rib_common.h"
#include "ue_mac_rib_info.h"
#include "cell_mac_rib_info.h"

namespace flexran {

  namespace rib {

    class enb_rib_info {
    public:
      enb_rib_info(int agent_id);
      
      void update_eNB_config(const protocol::flex_enb_config_reply& enb_config_update);
      
      void update_UE_config(const protocol::flex_ue_config_reply& ue_config_update);

      void update_UE_config(const protocol::flex_ue_state_change& ue_state_change);
      
      void update_LC_config(const protocol::flex_lc_config_reply& lc_config_update);

      void update_liveness();

      void update_subframe(const protocol::flex_sf_trigger& sf_trigger);

      void update_mac_stats(const protocol::flex_stats_reply& mac_stats);
  
      bool need_to_query();

      void dump_mac_stats() const;

      std::string dump_mac_stats_to_string() const;

      std::string dump_mac_stats_to_json_string() const;

      void dump_configs() const;

      std::string dump_configs_to_string() const;

      std::string dump_configs_to_json_string() const;

      frame_t get_current_frame() const { return current_frame_; }

      subframe_t get_current_subframe() const { return current_subframe_; }

      protocol::flex_enb_config_reply& get_enb_config() {return eNB_config_;}

      protocol::flex_ue_config_reply& get_ue_configs() {return ue_config_;}

      protocol::flex_lc_config_reply& get_lc_configs() {return lc_config_;}

      std::shared_ptr<ue_mac_rib_info> get_ue_mac_info(rnti_t rnti);

      cell_mac_rib_info& get_cell_mac_rib_info(uint16_t cell_id) {
	return cell_mac_info_[cell_id];
      }
      
    private:
      int agent_id_;

      clock_t last_checked;
      const clock_t time_to_query = 500;

      frame_t current_frame_;
      subframe_t current_subframe_;
      
      // eNB config structure
      protocol::flex_enb_config_reply eNB_config_;
      // UE config structure
      protocol::flex_ue_config_reply ue_config_;
      // LC config structure
      protocol::flex_lc_config_reply lc_config_;
      
      std::map<rnti_t, std::shared_ptr<ue_mac_rib_info>> ue_mac_info_;

      cell_mac_rib_info cell_mac_info_[MAX_NUM_CC];
  
    };

  }

}
    
#endif
