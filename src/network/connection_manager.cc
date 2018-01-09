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

#include "connection_manager.h"
#include "flexran_log.h"

flexran::network::connection_manager::connection_manager(boost::asio::io_service& io_service,
				       const boost::asio::ip::tcp::endpoint& endpoint,
				       async_xface& xface)
  : acceptor_(io_service, endpoint), socket_(io_service), next_id_(0), xface_(xface) {

  do_accept();

}

void flexran::network::connection_manager::send_msg_to_agent(std::shared_ptr<tagged_message> msg) {
  auto it = sessions_.find(msg->getTag());
  if (it == sessions_.end()) {
    LOG4CXX_WARN(flog::net, "Message for non-existent session " << msg->getTag() << " discarded");
    return;
  }
  sessions_[msg->getTag()]->deliver(msg);
}

void flexran::network::connection_manager::do_accept() {

  acceptor_.async_accept(socket_,
			 [this](boost::system::error_code ec) {
      if (!ec) {
	sessions_[next_id_] = std::make_shared<agent_session>(std::move(socket_), *this, xface_, next_id_);
	sessions_[next_id_]->start();
	xface_.initialize_connection(next_id_);
	next_id_++;
      }
      
      do_accept();
			 });
}

void flexran::network::connection_manager::close_connection(int session_id) {
  auto it = sessions_.find(session_id);
  if (it != sessions_.end()) {
    it->second->close();
    sessions_.erase(session_id);
  }
}
