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

#include "agent_session.h"
#include "tagged_message.h"
#include "flexran_log.h"

void flexran::network::agent_session::start() {
  do_read_header();
}

void flexran::network::agent_session::deliver(std::shared_ptr<tagged_message> msg) {
  bool write_in_progress = !write_queue_.empty();
  protocol_message data;
  std::size_t size = msg->getSize();
  data.set_message(msg->getMessageContents(), size);
  write_queue_.push_back(data);
  if (!write_in_progress) {
      do_write();
  }
}

void flexran::network::agent_session::do_read_header() {
  boost::asio::async_read(socket_,
			  boost::asio::buffer(read_msg_.data(), protocol_message::header_length),
			  [this](boost::system::error_code ec, std::size_t /*length*/) {
			    if (!ec && read_msg_.decode_header()) {
			      do_read_body();
			    }
			    else {
                              generate_disconnect_msg();
			    }
			  });
}

void flexran::network::agent_session::do_read_body() {
  boost::asio::async_read(socket_,
			  boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
			  [this](boost::system::error_code ec, std::size_t /*length*/) {
			    if (!ec) {
			      tagged_message *th = new tagged_message(read_msg_.body(),
								    read_msg_.body_length(),
								    session_id_);
			      xface_.forward_message(th);
			      do_read_header();
			    } else {
                              generate_disconnect_msg();
			    }
			  });
}

void flexran::network::agent_session::do_write() {
  auto self(shared_from_this());

  boost::asio::async_write(socket_,
			   boost::asio::buffer(write_queue_.front().data(),
					       write_queue_.front().length()),
			   [this, self](boost::system::error_code ec, std::size_t ) {
    if (!ec) {
      write_queue_.pop_front();
      if (!write_queue_.empty()) {
	do_write();
      }
    } else {
      generate_disconnect_msg();
    }
			   });
}

void flexran::network::agent_session::generate_disconnect_msg()
{
  LOG4CXX_WARN(flog::net, "Connection for session " << session_id_ << " lost");
  protocol::flex_header *header1(new protocol::flex_header);
  header1->set_type(protocol::FLPT_DISCONNECT);
  header1->set_version(0);
  header1->set_xid(0);

  protocol::flex_disconnect *disconnect_msg(new protocol::flex_disconnect);
  disconnect_msg->set_allocated_header(header1);

  protocol::flexran_message msg;
  msg.set_allocated_disconnect_msg(disconnect_msg);

  /* We need to manually serialize it so that it can be put into the incoming
  * queue. */
  tagged_message *tm = new tagged_message(msg.ByteSize(), session_id_);
  msg.SerializeToArray(tm->getMessageArray(), msg.ByteSize());
  xface_.forward_message(tm);
}

void flexran::network::agent_session::close()
{
  if (socket_.is_open())
    socket_.close();
}
