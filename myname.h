//#define set_myname std::string myname = typeid(this).name(); myname += "::"; myname += __func__;
//#define set_myname std::string myname = typeid(this).name(); myname += "::"; myname += __FUNCTION__;
#define set_myname std::string myname = __PRETTY_FUNCTION__;
