#ifndef TRAFT_DOUT_H
#define TRAFT_DOUT_H

#include "config.hpp"

namespace TRAFT{

#define dout_prefix *_dout

#define dout_impl(dcct,v)         \
    do {             \
        if (v <= dcct->conf->log_level) {              \
            auto _dout_e = dcct->conf->_log->create_entry(v);      \
            std::ostream _dout_os(&_dout_e->m_streambuf);   \
            std::ostream* _dout = &_dout_os;                 

#define ldout(dcct,v) dout_impl(dcct,v) dout_prefix
#define derr(dcct) dout_impl(dcct,-1) dout_prefix

#define dendl std::flush;           \
    cct->conf->_log->submit_entry(_dout_e); \
        } \
    }while(0)

#define dout(v) ldout(cct,v)

#define lderr derr(cct)

}

#endif