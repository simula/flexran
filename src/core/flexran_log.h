/* The MIT License (MIT)

   Copyright (c) 2017 Xenofon Foukas

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

#ifndef FLEXRAN_LOG_H_
#define FLEXRAN_LOG_H_

#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"

namespace flexran_log = log4cxx;

namespace flexran {

  namespace core {

    static flexran_log::LoggerPtr core_logger(flexran_log::Logger::getLogger("FLEXRAN_RTC"));

    static flexran_log::LoggerPtr rib_logger(flexran_log::Logger::getLogger("RIB"));

    static flexran_log::LoggerPtr net_logger(flexran_log::Logger::getLogger("NET"));

    static flexran_log::LoggerPtr app_logger(flexran_log::Logger::getLogger("APP"));
    
  }
}

namespace flog {
  static flexran_log::LoggerPtr core = flexran::core::core_logger;
  static flexran_log::LoggerPtr net  = flexran::core::net_logger;
  static flexran_log::LoggerPtr rib  = flexran::core::rib_logger;
  static flexran_log::LoggerPtr app  = flexran::core::app_logger;
}

#endif
