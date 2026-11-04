#pragma once
namespace qol { inline bool should_skip(int mode,bool seen,int frame,int minimum){ return frame>=minimum && (mode==2 || (mode==1 && seen)); } }
