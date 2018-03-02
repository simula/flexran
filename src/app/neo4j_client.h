/* The MIT License (MIT)

   Copyright (c) 2017 shahab SHARIAT BAGHERI

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

#ifndef NEO4J_CLIENT_H_
#define NEO4J_CLIENT_H_

#include <map>
#include <memory>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "periodic_component.h"
#include "enb_rib_info.h"
// #include "component.h"
// #include "ue_scheduling_info.h"
#include "enb_scheduling_info.h"
#include "rib_common.h"
#include "flexran.pb.h"

namespace flexran {

  namespace app {

    namespace management {

      class neo4j_client : public periodic_component {
	
      public:	
	
    neo4j_client(rib::Rib& rib, const core::requests_manager& rm)
	  : periodic_component(rib, rm) {}

	void update_graph();
	
	void update_node(rib::subframe_t subframe);

	void create_neo4j_graph();
    
        void periodic_task();

    // void neo4j_register();

	// void update_edge(rib::rnti_t rnti);
		
	private:

	int y = 0;
  int x = 0;
  int num_UEs_now = 0;
  int num_UEs_pre = 0;	

    };


   }
   

  }

}
      
#endif
