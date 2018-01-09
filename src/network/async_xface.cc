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

#include <boost/bind.hpp>

#include "async_xface.h"
#include "rt_wrapper.h"

void flexran::network::async_xface::run() {
  establish_xface();
}

void flexran::network::async_xface::end(){
  io_service.stop();
}

void flexran::network::async_xface::establish_xface() {
  manager_.reset(new connection_manager(io_service, endpoint_, *this));
  work_ptr_.reset(new boost::asio::io_service::work(io_service));
  io_service.run();
}

void flexran::network::async_xface::forward_message(tagged_message *msg) {
  in_queue_.push(msg);
}

bool flexran::network::async_xface::get_msg_from_network(std::shared_ptr<tagged_message>& msg) {
  return in_queue_.consume_one([&] (tagged_message *tm) {
      std::shared_ptr<tagged_message> p(std::move(tm));
      msg = p;});
}

bool flexran::network::async_xface::send_msg(const protocol::flexran_message& msg, int agent_tag) const {
  tagged_message *tm =  new tagged_message(msg.ByteSize(), agent_tag);
  msg.SerializeToArray(tm->getMessageArray(), msg.ByteSize());
  if (out_queue_.push(tm)) {
    io_service.post(boost::bind(&async_xface::forward_msg_to_agent, self_));
    return true;
  } else {
    return false;
  }
}

void flexran::network::async_xface::forward_msg_to_agent() {
  tagged_message *msg;
  out_queue_.pop(msg);
  std::shared_ptr<tagged_message> message(msg);
  manager_->send_msg_to_agent(message);
}


void flexran::network::async_xface::initialize_connection(int session_id) {
  tagged_message *th = new tagged_message(0, session_id);
  in_queue_.push(th);
}

void flexran::network::async_xface::release_connection(int session_id)
{
  manager_->close_connection(session_id);
}
