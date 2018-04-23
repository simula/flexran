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

#ifndef RIB_H_
#define RIB_H_

#include <map>
#include <set>

#include "enb_rib_info.h"
#include <memory>
#include <set>

namespace flexran {

  namespace rib {

    class Rib {
    public:

      // Pending agent methods
      void add_pending_agent(int agent_id);
      void remove_pending_agent(int agent_id);
      bool agent_is_pending(int agent_id);
      
      // eNB config management
      void new_eNB_config_entry(int agent_id);
      bool has_eNB_config_entry(int agent_id) const;
      void remove_eNB_config_entry(int agent_id);
      void eNB_config_update(int agent_id,
			     const protocol::flex_enb_config_reply& enb_config_update);
      void ue_config_update(int agent_id,
			    const protocol::flex_ue_config_reply& ue_config_update);
      void ue_config_update(int agent_id,
			    const protocol::flex_ue_state_change& ue_state_change);
      void lc_config_update(int agent_id,
			    const protocol::flex_lc_config_reply& lc_config_update);
      
      void update_liveness(int agent_id);
      
      void set_subframe_updates(int agent_id,
				const protocol::flex_sf_trigger& sf_trigger_msg);
      
      void mac_stats_update(int agent_id,
			    const protocol::flex_stats_reply& mac_stats_update);
      
      std::set<int> get_available_agents() const;
      
      std::shared_ptr<enb_rib_info> get_agent(int agent_id) const;
      
      void dump_mac_stats() const;
      
      void dump_enb_configurations() const;

      std::string dump_all_mac_stats_to_string() const;
      std::string dump_all_mac_stats_to_json_string() const;
      bool dump_mac_stats_by_enb_id_to_json_string(uint64_t enb_id, std::string& out) const;
      bool dump_mac_stats_by_agent_id_to_json_string(int agent_id, std::string& out) const;

      static std::string format_mac_stats_to_json(const std::vector<std::string>& mac_stats_json);
      
      std::string dump_all_enb_configurations_to_string() const;
      std::string dump_all_enb_configurations_to_json_string() const;
      bool dump_enb_configurations_by_enb_id_to_json_string(uint64_t enb_id, std::string& out) const;
      bool dump_enb_configurations_by_agent_id_to_json_string(int agent_id, std::string& out) const;

      static std::string format_enb_configurations_to_json(const std::vector<std::string>& enb_configurations_json);

      bool dump_ue_by_rnti_to_json_string(rnti_t rnti, std::string& out) const;
      bool dump_ue_by_rnti_by_enb_id_to_json_string(rnti_t rnti, std::string& out, uint64_t enb_id) const;
      bool dump_ue_by_rnti_by_agent_id_to_json_string(rnti_t rnti, std::string& out, int agent_id) const;
      bool dump_ue_by_imsi_to_json_string(uint64_t imsi, std::string& out) const;
      bool dump_ue_by_imsi_by_enb_id_to_json_string(uint64_t imsi, std::string& out, uint64_t enb_id) const;
      bool dump_ue_by_imsi_by_agent_id_to_json_string(uint64_t imsi, std::string& out, int agent_id) const;

      int get_agent_id(uint64_t enb_id) const;
      
    private:
      std::map<int, std::shared_ptr<enb_rib_info>>::const_iterator find_agent(uint64_t enb_id) const;
      
      std::map<int, std::shared_ptr<enb_rib_info>> eNB_configs_;
      std::set<int> pending_agents_;
      
    };

  }
  
}

#endif
